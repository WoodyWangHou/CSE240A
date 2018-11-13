//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <stdlib.h>
#include "predictor.h"
#include "helpers.h"

const char *studentName = "Hou Wang";
const char *studentID   = "A53241783";
const char *email       = "how038@eng.ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

/*
 * Data structure for Global History Register
 * 1. ghr: global history bits array - 32 bits
 * 2. ghrMask: mask used for masking ghr for indexing ghistoryBuffer
 * 3. ghistoryBuffer: the 2-bit prediction table for using global history
 * */

uint32_t ghr;
uint32_t ghrMask;
uint8_t* ghistoryBuffer;
uint32_t ghrSize; // number of entries

// 2-bit prediction buffer for local predictor
uint8_t* lhistoryBuffer;
uint32_t


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //

  // init predictor based on bpType
  switch (bpType) {
    case GSHARE:
      ghr = 0;
      ghrMask = left_shift(ghistoryBits);
      ghrSize = power(ghistoryBits);
      ghistoryBuffer = (uint8_t *) malloc(ghrSize * sizeof(uint8_t));
      init_counter(ghistoryBuffer, ghrSize);
      break;
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

  printf("init successful\n");

}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  uint32_t index = 0;
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      index = hash_ghr_to_index(pc, ghr, ghrMask);
      return parse_prediction_entry(ghistoryBuffer[index]);
    case TOURNAMENT:
    case CUSTOM:
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
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

  // update predictor based on bpType
  uint32_t index = 0;
  switch (bpType) {
    case GSHARE:
      // update global history buffer
      index = hash_ghr_to_index(pc, ghr, ghrMask);
      ghistoryBuffer[index] = next_state(ghistoryBuffer[index], outcome);
      ghr = ((ghr << 1) | outcome) & ghrMask;
      break;
    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }
}

void destructor(){
    free(ghistoryBuffer);
    free(lhistoryBuffer);
}
