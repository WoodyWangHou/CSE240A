//
// Created by Hou Wang on 11/13/18.
//

#ifndef CSE240A_UNITTEST_H
#define CSE240A_UNITTEST_H
#include <stdint.h>
/**
 *  Utilities for running unit tests
 *  1. all methods pass if no error
 *  2. all methods terminates if there is error and print a output to output stream
 * */
extern uint32_t failedCounter;

void assert_equal(char* testName, uint32_t a, uint32_t b);
#endif //CSE240A_UNITTEST_H
