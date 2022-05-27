#ifndef _FE_H_
#define _FE_H_

#include "params.h"
#include <inttypes.h>

#include "sha3/fips202.h"
void hash_1(unsigned char *k_1, unsigned char *k_2, uint8_t* r);
void hash_2(uint8_t* sigma, uint8_t* k_2, uint8_t* seed_a, int8_t* c);
void GenA(int8_t A[][PARAMS_N], uint8_t* seed);
void Gen(uint8_t* seed_a, int8_t* c, uint8_t* sigma, uint8_t* k_1, int8_t* w);
void Rep(uint8_t* r, uint8_t* seed_a, int8_t* c, int8_t* w_prime);

#endif