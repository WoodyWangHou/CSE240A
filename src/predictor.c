//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"
#include "helpers.h"
#include <math.h>

const char *studentName = "Hou Wang";
const char *studentID = "A53241783";
const char *email = "how038@eng.ucsd.edu";
//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = {"Static", "Gshare",
                         "Tournament", "Custom"};

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;
int32_t theta;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//


// Data structure for Global History Register
// 1. ghr: global history bits array - 32 bits
// 2. ghrMask: mask used for masking ghr for indexing ghistoryBuffer
// 3. ghistoryBuffer: the 2-bit prediction table for using global history

uint32_t ghr;
uint32_t ghrMask;
uint8_t *ghistoryBuffer;
uint32_t ghrSize; // number of entries

// 2-bit predictor selector
uint8_t *selectorBuffer;

// 2-bit prediction buffer for local predictor
uint32_t *pht;             // pattern history table, size by pcIndexBits
uint32_t pcIndexMask;      // Mask for using pc to index pht
uint32_t phtMask;          // Mask using lhistoryBits to mask pht entry
uint32_t phtSize;          // equal to size of pht, 2^pcIndexBits
uint8_t *lpredictionTable; // local prediction table, 2-bit saturating counter, size by lhistoryBits
uint32_t lptSize;          // size by lhistoryBits

// Perceptron implementation (GHR only version)
int32_t **perceptronTable; // perceptron table

//---------------------------------------------//
//        Custom Predictor Description         //
//---------------------------------------------//

// The custom branch predictor is based on DANIEL A. JIME´NEZ's paper "Neural Methods for Dynamic
// Branch Prediction". The implemented predictor is a simplified version using only global history.
// Predictor consists of a global history register, a Perceptron Table storing weights that correspond to history bits
// which indicate the correlation between history bits and the specific pc xor ghr address (for anti-aliasing)
// In the design, ghistoryBits is 13 bits, thus the perceptron entry length is 14 bits (1 bit for w0)
// Hardware Size: since there is only a perceptron table used, its size is 2^(ghistoryBits + 1) = 2^14 = 16Kbits
// the GHR in this case is 13 bits
// Thus the total size is 16 Kbits + 13 bits < 16 Kbits + 256 bits
// Based on the paper, the suggested theta is floor(1.93 * ghistoryBits + 14) = 39, however after simple tuning
// the best theta found is 29

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor() {
    // init predictor based on bpType
    switch (bpType) {
        case CUSTOM:
            ghistoryBits = 13;
            theta = 29;
            ghr = 0;
            ghrMask = left_shift(ghistoryBits);
            ghrSize = power(ghistoryBits);
            init_perceptronTable(ghrSize, ghistoryBits, &perceptronTable);
            break;
        case TOURNAMENT:
            // init local predictor
            phtMask = left_shift(lhistoryBits);
            phtSize = power(pcIndexBits);
            pcIndexMask = left_shift(pcIndexBits);
            pht = (uint32_t *) malloc(phtSize * sizeof(uint32_t));

            lptSize = power(lhistoryBits);
            lpredictionTable = (uint8_t *) malloc(lptSize * sizeof(uint8_t));

            // init predictor selector
            ghrSize = power(ghistoryBits);
            selectorBuffer = (uint8_t *) malloc(ghrSize * sizeof(uint8_t));

            // global predictor can be init using the following
            init_counter(selectorBuffer, ghrSize);
            init_table(pht, phtSize);
            init_counter(lpredictionTable, lptSize);
        case GSHARE:
            ghr = 0;
            ghrMask = left_shift(ghistoryBits);
            ghrSize = power(ghistoryBits);
            ghistoryBuffer = (uint8_t *) malloc(ghrSize * sizeof(uint8_t));
            init_counter(ghistoryBuffer, ghrSize);
            break;
        default:
            break;
    }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc) {
    // Make a prediction based on the bpType
    uint32_t gindex = 0;
    uint32_t lindex = 0;
    uint32_t localPattern = 0;
    switch (bpType) {
        case STATIC:
            return TAKEN;
        case GSHARE:
            gindex = xor_ghr_pc_to_index(pc, ghr, ghrMask);
            return parse_prediction_entry(ghistoryBuffer[gindex]);
        case CUSTOM:
            gindex = xor_ghr_pc_to_index(pc, ghr, ghrMask);
            return parse_perceptron_entry(ghr, ghistoryBits, perceptronTable[gindex]);
        case TOURNAMENT:
            // query selector to choose local or global
            if (bpType == TOURNAMENT) {
                gindex = hash_ghr_to_index(ghr, ghrMask);
            }
            uint8_t choice = parse_prediction_entry(selectorBuffer[gindex]);
            switch (choice) {
                case LC:
                    lindex = hash_pc_to_index(pc, pcIndexMask);
                    lindex = pht[lindex] & phtMask; // get the local pattern as index
                    return parse_prediction_entry(lpredictionTable[lindex]);
                default:
                    // global predictor
                    return parse_prediction_entry(ghistoryBuffer[gindex]);
            }
        default:
            break;
    }

    // If there is not a compatable bpType then return NOTTAKEN
    return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome) {
    // update predictor based on bpType
    uint32_t index = 0;
    // update tournament
    uint32_t lindex = 0;
    uint32_t localPattern = 0;
    uint8_t localPrediction = 0;
    uint8_t lpredictionRes = 0;
    uint8_t gpredictionRes = 0;
    uint8_t globalPrediction = 0;

    uint8_t choice = 0;
    switch (bpType) {
        case GSHARE:
            // update global history buffer
            index = xor_ghr_pc_to_index(pc, ghr, ghrMask);
            ghistoryBuffer[index] = next_state(ghistoryBuffer[index], outcome);
            ghr = ((ghr << 1) | outcome) & ghrMask;
            break;
        case CUSTOM:
            index = xor_ghr_pc_to_index(pc, ghr, ghrMask);
            int32_t signed_outcome = (outcome == TAKEN) ? 1 : -1;
            train_perceptron(ghistoryBits, ghr, signed_outcome, perceptronTable[index], index);
            ghr = ((ghr << 1) | outcome) & ghrMask;
            break;
        case TOURNAMENT:
            // check current selector:

            // Train local predictor
            lindex = hash_pc_to_index(pc, pcIndexMask);
            localPattern = pht[lindex] & phtMask;
            localPrediction = lpredictionTable[localPattern];

            // update local 2-bit counter
            lpredictionTable[localPattern] = next_state(localPrediction, outcome);

            // update local pattern in pht
            pht[lindex] = ((localPattern << 1) | outcome) & phtMask;

            // Train Global Predictor
            // update global 2-bit counter
            if (bpType == TOURNAMENT) {
                index = hash_ghr_to_index(ghr, ghrMask);
            }
            globalPrediction = ghistoryBuffer[index];
            ghistoryBuffer[index] = next_state(globalPrediction, outcome);

            // update ghr
            ghr = ((ghr << 1) | outcome) & ghrMask;

            // check local and global prediction result and update selector
            lpredictionRes = parse_prediction_entry(localPrediction) ^ outcome;
            gpredictionRes = parse_prediction_entry(globalPrediction) ^ outcome;

            // if gpredictionRes < lpredictionRes, favor global, otherwise local
            if (gpredictionRes < lpredictionRes) {
                selectorBuffer[index] = next_state(selectorBuffer[index], CHOOSEGL);
            } else if (gpredictionRes > lpredictionRes) {
                selectorBuffer[index] = next_state(selectorBuffer[index], CHOOSELC);
            }// else does not change selector
            break;
        default:
            break;
    }
}

void destructor() {
    free(ghistoryBuffer);
    free(selectorBuffer);
    free(pht);
    free(lpredictionTable);
    free(perceptronTable);
}
