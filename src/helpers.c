//
// Created by Hou Wang on 11/12/18.
//

#include <stdlib.h>
#include <stdio.h>
#include "unitTest.h"
#include "predictor.h"

/*
* Unit Test for Helpers
*/

// helper to create a mask for unsigned 32 bits
uint32_t left_shift(int bits) {
    if (bits < 0) return 0;

    uint32_t res = 0;
    for (int i = 0; i < bits; i++) {
        res = (res << 1) | 1;
    }
    return res;
}

// compute 2 to the power of x
uint32_t power(int exp) {
    if (exp < 0) return 0;

    uint32_t res = 1;
    for (int i = 0; i < exp; i++) {
        res = (res << 1);
    }

    return res;
}

// parse prediction entry to get the prediction
uint8_t parse_prediction_entry(uint8_t entry) {
    return (entry & 2) >> 1;
}

// compute next state for 2-bit counters based on current states
uint8_t next_state(uint8_t curState, uint8_t outcome) {
    if (curState == ST && outcome == TAKEN) {
        return ST;
    }

    if (curState == SN && outcome == NOTTAKEN) {
        return SN;
    }

    return (outcome == TAKEN) ? curState + 1 : curState - 1;
}

// get index using pc and ghr to global prediction buffer
uint32_t xor_ghr_pc_to_index(uint32_t pc, uint32_t ghr, uint32_t mask) {
    return (pc & mask) ^ (ghr & mask);
}

// get index based on ghr
uint32_t hash_ghr_to_index(uint32_t ghr, uint32_t mask) {
    return xor_ghr_pc_to_index(0, ghr, mask);
}

// get index based on pc
uint32_t hash_pc_to_index(uint32_t pc, uint32_t mask) {
    return xor_ghr_pc_to_index(pc, 0, mask);
}

// initialize 2-bit counters  to WN
void init_counter(uint8_t *reg, uint32_t size) {
    for (int i = 0; i < size; i++) {
        reg[i] = WT;
    }
}

// initialize a table to 0
void init_table(uint32_t *table, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        table[i] = 0;
    }
}

/**
 * Unit Test Runner
 * Add unit tests here
 * */
// unit test runner
// Exit if there is bug in unit tests
void unit_test() {
    // test bitShifts
    uint32_t bitShifts = 0;
    bitShifts = left_shift(1);
    assert_equal("bitShifts", bitShifts, 1);
    bitShifts = left_shift(0);
    assert_equal("bitShifts", bitShifts, 0);
    bitShifts = left_shift(2);
    assert_equal("bitShifts", bitShifts, 3);
    bitShifts = left_shift(4);
    assert_equal("bitShifts", bitShifts, 15);

    // test power
    uint32_t exp = 0;
    exp = power(0);
    assert_equal("power", exp, 1);
    exp = power(1);
    assert_equal("power", exp, 2);
    exp = power(4);
    assert_equal("power", exp, 16);

    // test parse_prediction_entry
    uint8_t pred = 0;
    pred = parse_prediction_entry(0);
    assert_equal("parse prediction", pred, 0);
    pred = parse_prediction_entry(1);
    assert_equal("parse prediction", pred, 0);
    pred = parse_prediction_entry(2);
    assert_equal("parse prediction", pred, 1);
    pred = parse_prediction_entry(3);
    assert_equal("parse prediction", pred, 1);

    // test next_state
    uint8_t next = 0;
    next = next_state(0, 0);
    assert_equal("next_state", next, 0);
    next = next_state(0, 1);
    assert_equal("next_state", next, 1);
    next = next_state(3, 1);
    assert_equal("next_state", next, 3);
    next = next_state(3, 0);
    assert_equal("next_state", next, 2);
    next = next_state(2, 0);
    assert_equal("next_state", next, 1);
    next = next_state(2, 1);
    assert_equal("next_state", next, 3);

    // Test gshare xor
    uint32_t index = 0;
    uint32_t mask = left_shift(13);
    index = xor_ghr_pc_to_index(0, power(12), mask);
    assert_equal("xor_ghr_pc_to_index", index, power(12));
    index = xor_ghr_pc_to_index(0, power(14), mask);
    assert_equal("xor_ghr_pc_to_index", index, 0);
    index = xor_ghr_pc_to_index(3, 0, mask);
    assert_equal("xor_ghr_pc_to_index", index, 3);
    index = xor_ghr_pc_to_index(4, 0, mask);
    assert_equal("xor_ghr_pc_to_index", index, 4);

    // Test hash_ghr_to_index
    mask = left_shift(13);
    index = hash_ghr_to_index(power(14), mask);
    assert_equal("hash_ghr_to_index", index, 0);
    index = hash_ghr_to_index(power(12), mask);
    assert_equal("hash_ghr_to_index", index, power(12));
    index = hash_ghr_to_index(3, mask);
    assert_equal("hash_ghr_to_index", index, 3);
    mask = left_shift(2);
    index = hash_ghr_to_index(4, mask);
    assert_equal("hash_ghr_to_index", index, 0);

    // Test hash_pc_to_index
    mask = left_shift(13);
    index = hash_pc_to_index(power(14), mask);
    assert_equal("hash_ghr_to_index", index, 0);
    index = hash_pc_to_index(power(12), mask);
    assert_equal("hash_ghr_to_index", index, power(12));
    index = hash_pc_to_index(3, mask);
    assert_equal("hash_ghr_to_index", index, 3);
    index = hash_pc_to_index(4, mask);
    assert_equal("hash_ghr_to_index", index, 4);

    // terminate if unit tests failed
    if (failedCounter > 0) {
        printf("Unit Tests Failed: %d tests fail", failedCounter);
        exit(1);
    } else {
        printf("All Unit Tests Passed!\n");
    }
}
