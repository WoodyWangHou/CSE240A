//========================================================//
//  predictor.h                                           //
//  Header file for the Branch Predictor                  //
//                                                        //
//  Includes function prototypes and global predictor     //
//  variables and defines                                 //
//========================================================//

#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <stdint.h>
#include <stdlib.h>

//
// Student Information
//
extern const char *studentName;
extern const char *studentID;
extern const char *email;

//------------------------------------//
//      Global Predictor Defines      //
//------------------------------------//
#define NOTTAKEN  0
#define TAKEN     1

// Perceptron TAKEN NOTTAKEN
#define PNOTTAKEN  -1
#define PTAKEN     1

// The Different Predictor Types
#define STATIC      0
#define GSHARE      1
#define TOURNAMENT  2
#define CUSTOM      3
extern const char *bpName[];

// Definitions for 2-bit counters
#define SN  0            // predict NT, strong not taken
#define WN  1            // predict NT, weak not taken
#define WT  2            // predict T, weak taken
#define ST  3            // predict T, strong taken

// Definitions for tournament predictor chooser
#define SG  0           // predict local, strong local
#define WG  1           // predict local, weak local
#define WL  2           // predict global, weak global
#define SL  3           // predict local, strong local
#define CHOOSEGL 0      // used to train selector to favor global
#define CHOOSELC 1      // used to train selector to favor local

// Definitions for predictor selector
#define GL 0
#define LC 1

// Definition of perceptron threhold
extern int32_t theta;
extern uint32_t perceptronBits;
//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//
extern int ghistoryBits; // Number of bits used for Global History
extern int lhistoryBits; // Number of bits used for Local History
extern int pcIndexBits;  // Number of bits used for PC index
extern int bpType;       // Branch Prediction Type
extern int verbose;

//------------------------------------//
//    Predictor Function Prototypes   //
//------------------------------------//

// Initialize the predictor
//
void init_predictor();

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc);

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome);

// free memory
void destructor();

#endif
