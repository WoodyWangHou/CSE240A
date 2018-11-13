//
// Created by Hou Wang on 11/13/18.
//

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
uint32_t failedCounter = 0;

void assert_equal(char* testName, uint32_t a, uint32_t b){
    if(a != b){
        printf("Assert Equal failed at %s: a: %d, b: %d\n", testName, a, b);
        failedCounter++;
    }
}

