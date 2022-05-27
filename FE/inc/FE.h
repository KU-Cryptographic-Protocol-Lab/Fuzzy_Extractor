#ifndef _FE_H_
#define _FE_H_

#include "params.h"
#include <inttypes.h>

void GenA(int8_t A[][PARAMS_N], uint8_t* seed);
void Gen(uint8_t* seed_a, int8_t* c, uint8_t* r, int8_t* w);
void Rep(uint8_t* r, uint8_t* seed_a, int8_t* c, int8_t* w_prime);

#endif