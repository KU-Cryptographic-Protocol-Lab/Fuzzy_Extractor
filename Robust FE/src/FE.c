#include "FE.h"
#include "NIST/rng.h"
#include "bch.h"
#include "params.h"
#include <string.h>
#include "sha3/fips202.h"

void hash_1(unsigned char *k_1, unsigned char *k_2, uint8_t* r)
{ // Hash-based function H to generate c'
  unsigned char k[2*PARAMS_R_BYTES];
  unsigned int i;

  shake128(k, 2*PARAMS_R_BYTES, r, PARAMS_R_BYTES);

  for (i=0; i<PARAMS_R_BYTES; i++) {
    k_1[i] = k[i];
    k_2[i] = k[i+PARAMS_R_BYTES];
    }
  
}

void hash_2(uint8_t* sigma, uint8_t* k_2, uint8_t* seed_a, int8_t* c)
{ // Hash-based function H to generate c'
  unsigned char t[PARAMS_R_BYTES + 32 + PARAMS_M];
  unsigned int i;




  for (i=0; i<PARAMS_R_BYTES; i++) {
    t[i] = k_2[i];
    }
  memcpy(&t[PARAMS_R_BYTES], seed_a, 32);   
  memcpy(&t[PARAMS_R_BYTES+32], c, PARAMS_M);
 	for (i=PARAMS_R_BYTES + 32; i<PARAMS_R_BYTES + 32 + PARAMS_M; i++) {
   // printf("t[%d] = %d \n", i, t[i]);
    }
  shake128(sigma, PARAMS_R_BYTES, t, PARAMS_R_BYTES + 32 + PARAMS_M);
}

void GenA(int8_t A[][PARAMS_N], uint8_t* seed)
{
	AES_XOF_struct ctx;
	uint8_t domain[8] = {'G','e','n','A',0,0,0,0};
	uint8_t tmp;

	seedexpander_init(&ctx, seed, domain, PARAMS_M*PARAMS_N+1);

	for(int i = 0; i < PARAMS_M; i++)
	{
		for (int j = 0; j < PARAMS_N >> 1; j++)
		{
			seedexpander(&ctx, &tmp, 1);

			A[i][(j << 1)    ] = ((tmp >> 4) & 0xf) - (PARAMS_L >> 1);
			A[i][(j << 1) + 1] = ((tmp     ) & 0xf) - (PARAMS_L >> 1);
		}
	}
}

//c = Aw+e+encode(r)
void Gen(uint8_t* seed_a, int8_t* c, uint8_t* sigma, uint8_t* k_1, int8_t* w)
{
	//c
	int8_t A[PARAMS_M][PARAMS_N];

	//r, k2
	uint8_t r[PARAMS_R_BYTES] = {0};
	uint8_t k_2[PARAMS_R_BYTES] = {0};
	//uint8_t sigma[PARAMS_R_BYTES] = {0};
	
	int8_t e[PARAMS_M] = {0};

	//PRNG
	AES_XOF_struct ctx;
	uint8_t domain[8] = {'G','e','n','A',0,0,0,0};
	uint8_t seed[32] = {0};
	

	//BCH
	uint8_t msg[BCH_K_BYTES] = {0};
	uint8_t rcv[BCH_N_BYTES] = {0};

	//??
	uint8_t tmp;

	//Generate A
	randombytes(seed_a, 32);
	GenA(A, seed_a);

	//c = Aw
	for (int i = 0; i < PARAMS_M; i++)
	{
		c[i] = 0;

		for (int j = 0; j < PARAMS_N; j++)
		{
			c[i] += A[i][j] * w[j]; 
		}
	}

	//c = Aw + e
	randombytes(seed, 32);
	seedexpander_init(&ctx, seed, domain, PARAMS_K * PARAMS_M + 1);

	for (int i = 0; i < PARAMS_M; i++) //centered binomial part
	{
		for(int j = 0; j < PARAMS_K; j++)
		{
			int tmp_j[8] = {0};

			seedexpander(&ctx, &tmp, 1);

			tmp_j[0] = (tmp >> 7) & 0x1;
			tmp_j[1] = (tmp >> 6) & 0x1;
			tmp_j[2] = (tmp >> 5) & 0x1;
			tmp_j[3] = (tmp >> 4) & 0x1;
			tmp_j[4] = (tmp >> 3) & 0x1;
			tmp_j[5] = (tmp >> 2) & 0x1;
			tmp_j[6] = (tmp >> 1) & 0x1;
			tmp_j[7] = (tmp >> 0) & 0x1;

			e[i] += (tmp_j[0] + tmp_j[1] + tmp_j[2] + tmp_j[3]);
			e[i] -= (tmp_j[4] + tmp_j[5] + tmp_j[6] + tmp_j[7]);
		}
		//printf("e[%d] = %d \n", i, e[i]);
		c[i] += e[i];
	}

	//Sample r
	randombytes(r, PARAMS_R_BYTES);

	//BCH
	memcpy(msg, r, PARAMS_R_BYTES);
	bch_encode(rcv, bch_poly, msg);

	hash_1(k_1, k_2, r);



	//EMBLEM & Compute Aw+e+Encode(r)
	for (int i = 0; i < PARAMS_M; i++)
	{
		tmp = (((rcv[i >> 3] >> (7 - (i & 0x7))) & 1) << 7) ^ 0x40;

		c[i] += tmp;
	}
	hash_2(sigma, k_2, seed_a, c);
	//hash_2(sigma2, k_2, seed_a, c);
	for (int i = 0; i < PARAMS_R_BYTES; i++){
		//printf("sigma[%d] = %d \n", i, sigma[i]);
	}
	for (int i = 0; i < PARAMS_R_BYTES; i++){
	//	printf("sigma2[%d] = %d \n", i, sigma2[i]);
	}
}

void Rep(uint8_t* sigma, uint8_t* seed_a, int8_t* c, int8_t* w_prime)
{
	//c
	int8_t A[PARAMS_M][PARAMS_N];

	//k1, k2
	uint8_t r[PARAMS_R_BYTES] = {0};
	uint8_t k_1[PARAMS_R_BYTES] = {0};
	uint8_t k_2[PARAMS_R_BYTES] = {0};

	//BCH
	uint8_t msg[BCH_K_BYTES] = {0};
	uint8_t rcv[BCH_N_BYTES] = {0};

	//Generate A
	GenA(A, seed_a);

	//Compute c - Aw
	for (int i = 0; i < PARAMS_M; i++)
	{
		uint8_t tmp = c[i];

		for (int j = 0; j < PARAMS_N; j++)
		{
			tmp -= A[i][j] * w_prime[j];
		}

		rcv[i >> 3] ^= (tmp >> 7) << (7 - (i & 0x7));
	}

	bch_decode(msg, rcv);

	memcpy(r, msg, PARAMS_R_BYTES);

	hash_1(k_1, k_2, r);
	hash_2(sigma, k_2, seed_a, c);
	for (int i = 0; i < PARAMS_R_BYTES; i++){
		//printf("sigma3[%d] = %d \n", i, sigma[i]);
	}
}
