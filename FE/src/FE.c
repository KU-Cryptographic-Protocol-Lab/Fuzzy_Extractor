#include "FE.h"
#include "NIST/rng.h"
#include "bch.h"
#include "params.h"
#include <string.h>

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
void Gen(uint8_t* seed_a, int8_t* c, uint8_t* r, int8_t* w)
{
	//c
	int8_t A[PARAMS_M][PARAMS_N];

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

			c[i] += (tmp_j[0] + tmp_j[1] + tmp_j[2] + tmp_j[3]);
			c[i] -= (tmp_j[4] + tmp_j[5] + tmp_j[6] + tmp_j[7]);
		}
	}

	//Sample r
	randombytes(r, PARAMS_R_BYTES);

	//BCH
	memcpy(msg, r, PARAMS_R_BYTES);
	bch_encode(rcv, bch_poly, msg);

	//EMBLEM & Compute Aw+e+Encode(r)
	for (int i = 0; i < PARAMS_M; i++)
	{
		tmp = (((rcv[i >> 3] >> (7 - (i & 0x7))) & 1) << 7) ^ 0x40;

		c[i] += tmp;
	}
}

void Rep(uint8_t* r, uint8_t* seed_a, int8_t* c, int8_t* w_prime)
{
	//c
	int8_t A[PARAMS_M][PARAMS_N];

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
}
