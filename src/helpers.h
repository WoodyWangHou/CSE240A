//
// Created by Hou Wang on 11/12/18.
//

#ifndef CSE240A_HELPERS_H
#define CSE240A_HELPERS_H

#include <stdint.h>
#include <stdlib.h>

// unit tests for all helper functions
void unit_test();

// helper to create a mask for unsigned 32 bits
uint32_t left_shift(int bits);

// compute 2 to the power of x
uint32_t power(int exp);

// parse prediction entry to get the prediction
uint8_t parse_prediction_entry(uint8_t entry);

// compute next state for 2-bit counters based on current states
uint8_t next_state(uint8_t curState, uint8_t outcome);

// get index using pc and ghr to global prediction buffer
uint32_t xor_ghr_pc_to_index(uint32_t pc, uint32_t ghr, uint32_t mask);

// get index based on ghr
uint32_t hash_ghr_to_index(uint32_t ghr, uint32_t mask);

// get index based on pc
uint32_t hash_pc_to_index(uint32_t pc, uint32_t mask);

// initialize 2-bit counters  to WN
void init_counter(uint8_t* reg, uint32_t size);

// initialize a table to 0
void init_table(uint32_t* table, uint32_t size);

#endif //CSE240A_HELPERS_H
