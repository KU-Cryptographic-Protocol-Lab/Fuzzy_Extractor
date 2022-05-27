#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include "sha3/fips202.h"
#include "NIST/rng.h"
#include "FE.h"

#define TEST_LOOP 10000

int64_t cpucycles(void)
{
	unsigned int hi, lo;

    __asm__ __volatile__ ("rdtsc\n\t" : "=a" (lo), "=d"(hi));

    return ((int64_t)lo) | (((int64_t)hi) << 32);
}

void HWT(int8_t* delta, int num)
{
#ifdef PARAM_80
	int cnt = 0;

	memset(delta, 0, PARAMS_N);
	
	while(cnt < num)
	{
		uint8_t tmp1[2], tmp2;
		
		randombytes(tmp1, 2);

		tmp2 = tmp1[0] % PARAMS_N;

		if(delta[tmp2] == 0)
		{
			delta[tmp2] = (((tmp1[1] >> 7) & 0x1) << 1) - 1;
			cnt++;
		}
	}
#endif


#ifdef PARAM_128	
	int cnt = 0;

	memset(delta, 0, PARAMS_N);
	
	while(cnt < num)
	{
		uint8_t tmp[2];
	
		randombytes(tmp, 2);

		if(delta[tmp[0]] == 0)
		{
			delta[tmp[0]] = ((tmp[1] & 0x1) << 1) - 1;
			cnt++;
		}
	}
#endif

#ifdef PARAM_80_2
	int cnt = 0;

	memset(delta, 0, PARAMS_N);
	
	while(cnt < num)
	{
		uint8_t tmp1[2];
		int16_t tmp2;

		randombytes(tmp1, 2);
		tmp2 = ((tmp1[1] & 0x1) << 8) | tmp1[0];

		tmp2 = tmp2 % PARAMS_N;

		if(delta[tmp2] == 0)
		{
			delta[tmp2] = (((tmp1[1] >> 7) & 0x1) << 1) - 1;
			cnt++;
		}
	}
#endif

#ifdef PARAM_256
	int cnt = 0;

	memset(delta, 0, PARAMS_N);
	
	while(cnt < num)
	{
		uint8_t tmp1[2];
		int16_t tmp2;

		randombytes(tmp1, 2);

		tmp2 = ((tmp1[1] & 0x1) << 8) | tmp1[0];

		if(delta[tmp2] == 0)
		{
			delta[tmp2] = (((tmp1[1] >> 7) & 0x1) << 1) - 1;
			cnt++;
		}
	}
#endif
}

void HWT2(int8_t* delta, int num)
{
#ifdef PARAM_80
	int cnt = 0;
	int a =0 ,b=0,c=0,d = 0;

	memset(delta, 0, PARAMS_N);
	
	while(cnt < num/2)
	{
		uint8_t tmp1[2], tmp2;
		
		randombytes(tmp1, 2);

		tmp2 = tmp1[0] % PARAMS_N;

		if(delta[tmp2] == 0)
		{
			delta[tmp2] = ((((tmp1[1] >> 7) & 0x1) << 1) - 1) <<1;
			cnt++;
		}
	}
	while(cnt < num)
	{
		uint8_t tmp1[2], tmp2;
		
		randombytes(tmp1, 2);

		tmp2 = tmp1[0] % PARAMS_N;

		if(delta[tmp2] == 0)
		{
			delta[tmp2] = (((tmp1[1] >> 7) & 0x1) << 1) - 1  ;
			cnt++;
		}
	
	}

	//for (int i = 0; i < PARAMS_N; ++i)
	//	{
	//		if(delta[i] == 1)
	//			a++;
	//		if(delta[i] == -1)
	//			b++;
	//		if(delta[i] == 2)
	//			c++;
	//		if(delta[i] == -2)
	//			d++;
	//	}

	//	printf("#1 = %d, #-1 = %d, #2 = %d, #-2 = %d \n", a,b,c,d);


#endif


#ifdef PARAM_128	
	int cnt = 0;

	memset(delta, 0, PARAMS_N);
	
	while(cnt < num/2)
	{
		uint8_t tmp[2];
		
		randombytes(tmp, 2);

		if(delta[tmp[0]] == 0)
		{
			delta[tmp[0]] = (((tmp[1] & 0x1) << 1) - 1) <<1;
			cnt++;
		}
	}
	while(cnt < num)
	{
		uint8_t tmp[2];
		
		randombytes(tmp, 2);

		if(delta[tmp[0]] == 0)
		{
			delta[tmp[0]] = ((tmp[1] & 0x1) << 1) - 1 ;
			cnt++;
		}
	
	}

#endif

#ifdef PARAM_256
	int cnt = 0;

	memset(delta, 0, PARAMS_N);
	
	while(cnt < num/2)
	{
		uint8_t tmp1[2];
		int16_t tmp2;

		randombytes(tmp1, 2);

		tmp2 = ((tmp1[1] & 0x1) << 8) | tmp1[0];

		if(delta[tmp2] == 0)
		{
			delta[tmp2] = ((((tmp1[1] >> 7) & 0x1) << 1) - 1) << 1;
			cnt++;
		}
	}
	while(cnt < num)
	{
		uint8_t tmp1[2];
		int16_t tmp2;

		randombytes(tmp1, 2);

		tmp2 = ((tmp1[1] & 0x1) << 8) | tmp1[0];

		if(delta[tmp2] == 0)
		{
			delta[tmp2] = (((tmp1[1] >> 7) & 0x1) << 1) - 1;
			cnt++;
		}
	}
#endif
}

void TEST0()
{
	printf("bio data size : %d BYTES\n", PARAMS_N);
	printf("helper data size : %d BYTES\n", 32 + PARAMS_M + PARAMS_R_BYTES);
	printf("extracted key size : %d BYTES\n", PARAMS_R_BYTES);
}

void TEST1()
{

	int cnt = 0;

	uint8_t seed_a[32];
	int8_t c[PARAMS_M] = {0};

	uint8_t k_1[PARAMS_R_BYTES] = {0};
	uint8_t k_2[PARAMS_R_BYTES] = {0};
	uint8_t sigma1[PARAMS_R_BYTES] = {0};
	uint8_t sigma2[PARAMS_R_BYTES] = {0};
	
	int8_t w[PARAMS_N] = {0};
	int8_t w_prime[PARAMS_N] = {0};

	float error_rate = 0.3; 

//80-bit HWT_1
//30% => 85/1000000
//31% => 1/10000
//32% => 3/10000
//33% => 18/10000
//34% => 48/10000
//35% => 109/10000
//36% => 177/10000
//37% => 343/10000 -> 34259/1000000
//38% => 515/10000
//39% => 896/10000

//80-bit HWT_2
//15% => 47069/1000000

//80-bit CBD
//k=1 
//kxk => 138/1000000

//128-bit HWT_1
//21% => 0/10000
//22% => 15/10000
//23% => 35/10000
//24% => 223/10000
//25% => 754/10000
//26% => 1516/10000
//27% => 3082/10000
//28% => 4358/10000
//29% => 4358/10000

//256-bit HWT_1
//10% => 0/10000
//11% => 2/10000
//12% => 71/10000
//13% => 1226/10000
//14% => 5372/10000	

	printf("==========TEST1 : Correctness==========\n");
	printf("error rate : %f\n", error_rate);
	//Generate bioinfo w
	int cnt2 = 0;
	randombytes(w, PARAMS_N);
		#define PARAMS_E 1

		printf("PARAMS_E = %d \n", PARAMS_E);
	
	//for (int p = 15; p < 120; p++)
	//{
		//int cnt = 0;
	for (int t = 0; t < TEST_LOOP; ++t)
	{
		int8_t delta[PARAMS_N] = {0};
		int8_t tmp2[PARAMS_N] = {0};
		int8_t tmp3[PARAMS_N] = {0};
		int8_t tmp4[PARAMS_N] = {0};
		int8_t tmp5[PARAMS_N] = {0};
		int8_t tmp6[PARAMS_N] = {0};
		int8_t tmp7[PARAMS_N] = {0};
		int8_t tmp8[PARAMS_N] = {0};
		
		if(t%1000 == 0) printf("index : %d\n", t);

		//Generate Delta
		#ifdef HWT_1
		HWT(delta, PARAMS_N * error_rate);
		for (int i = 0; i < PARAMS_N; i++) //centered binomial part
		{

			if (delta[i] != 0)
			{
				cnt2++;
			}
		}
		//printf("cnt2 = %d \n", cnt2);
		#endif
		
		#ifdef HWT_2
		HWT2(delta, p);
		for (int i = 0; i < PARAMS_N; i++) //centered binomial part
		{
			//if (delta[i] != 0)
			//{ss
			//	cnt2++;
			//}
		}
		//printf("cnt2 = %d \n", cnt2);
		#endif
		
		#ifdef CBD

		//PRNG
		AES_XOF_struct ctx;
		uint8_t domain[8] = {'G','e','n','A',0,0,0,0};
		uint8_t seed[32] = {0};
		uint8_t tmp;
		randombytes(seed, 32);
		seedexpander_init(&ctx, seed, domain, 2* PARAMS_E * PARAMS_N + 1);
		
		for (int i = 0; i < PARAMS_N; i++) //centered binomial part
		{
			for(int j = 0; j < PARAMS_E; j++)
			{
				int tmp_j[8] = {0};
				int tmp_k[8] = {0};

				seedexpander(&ctx, &tmp, 1);

				tmp_j[0] = (tmp >> 7) & 0x1;
				tmp_j[1] = (tmp >> 6) & 0x1;
				tmp_j[2] = (tmp >> 5) & 0x1;
				tmp_j[3] = (tmp >> 4) & 0x1;
				tmp_j[4] = (tmp >> 3) & 0x1;
				tmp_j[5] = (tmp >> 2) & 0x1;
				tmp_j[6] = (tmp >> 1) & 0x1;
				tmp_j[7] = (tmp >> 0) & 0x1;
				
				seedexpander(&ctx, &tmp, 1);

				tmp_k[0] = (tmp >> 7) & 0x1;
				tmp_k[1] = (tmp >> 6) & 0x1;
				tmp_k[2] = (tmp >> 5) & 0x1;
				tmp_k[3] = (tmp >> 4) & 0x1;
				tmp_k[4] = (tmp >> 3) & 0x1;
				tmp_k[5] = (tmp >> 2) & 0x1;
				tmp_k[6] = (tmp >> 1) & 0x1;
				tmp_k[7] = (tmp >> 0) & 0x1;

				//delta[i] += (tmp_j[0] + tmp_j[1] + tmp_j[2] + tmp_j[3]);
				//delta[i] -= (tmp_j[4] + tmp_j[5] + tmp_j[6] + tmp_j[7]);
				delta[i] += (tmp_j[0]);
				delta[i] -= (tmp_j[4]);
				//printf("delta[%d] = %d \n", i, delta[i]);
			
				tmp2[i] += (tmp_j[1]);
				tmp2[i] -= (tmp_j[5]);
				
				tmp3[i] += (tmp_j[2]);
				tmp3[i] -= (tmp_j[6]);
				tmp4[i] += (tmp_j[3]);
				tmp4[i] -= (tmp_j[7]);

				tmp3[i] *= tmp4[i];

				delta[i] *= tmp2[i];
				delta[i] *= tmp3[i];

				//tmp5[i] += (tmp_k[0]);
				//tmp5[i] -= (tmp_k[1]);
				//printf("tmp5[%d] = %d \n", i, tmp5[i]);
				//tmp6[i] += (tmp_k[2]);
				//tmp6[i] -= (tmp_k[3]);
				//tmp7[i] += (tmp_k[2]);
				//tmp7[i] -= (tmp_k[6]);
				//tmp8[i] += (tmp_k[3]);
				//tmp8[i] -= (tmp_k[7])
				//tmp5[i] *= tmp6[i];
				//printf("tmp5[%d] = %d \n", i, tmp5[i]);
				//tmp5[i] *= tmp7[i];
				//tmp5[i] *= tmp8[i];

				//delta[i] *= tmp5[i];
 

				//printf("delta[%d] = %d \n", i, delta[i]);
				
			}
			if (delta[i] != 0)
				{
					cnt2++;
				}
				
		}
		//printf("cnt2 = %d \n", cnt2);
		#endif
		

		//Generate bioinfo w_prime
		for (int i = 0; i < PARAMS_N; ++i)
		{
			w_prime[i] = w[i] + delta[i];
		}

		Gen(seed_a, c, sigma1, k_1, w);
		Rep(sigma2, seed_a, c, w_prime);
		

		if(memcmp(sigma1, sigma2, PARAMS_R_BYTES) != 0)
		{
			cnt++;
			//printf("t= %d, XXX = %d \n", t, cnt2);
		}
	}
	//printf("cnt2 = %d \n", cnt2);
	printf("%d \n", cnt);
}
//}

void TEST2()
{
	int cnt = 0;
	uint8_t seed_a[32];
	int8_t c[PARAMS_M] = {0};

	uint8_t k_1[PARAMS_R_BYTES] = {0};
	uint8_t k_2[PARAMS_R_BYTES] = {0};
	uint8_t sigma1[PARAMS_R_BYTES] = {0};
	uint8_t sigma2[PARAMS_R_BYTES] = {0};

	int8_t w[PARAMS_N] = {0};
	int8_t w_prime[PARAMS_N] = {0};
	int8_t delta[PARAMS_N] = {0};

	printf("==========TEST2 : Soundness==========\n");
	
	//Generate bioinfo w
	randombytes(w, PARAMS_N);

	for (int i = 0; i < TEST_LOOP; ++i)
	{
		if(i%1000 == 0) printf("index : %d\n", i);

		Gen(seed_a, c, sigma1, k_1, w);

		randombytes(w_prime, PARAMS_N);
		Rep(sigma2, seed_a, c, w_prime);

		if(memcmp(sigma1, sigma2, PARAMS_R_BYTES) != 0)
		{
			cnt++;
		}
	}

	printf("count : %d\n", cnt);
}
void TEST3()
{
    unsigned long long gcycles, rcycles;
    unsigned long long cycles1, cycles2;

	int cnt = 0;
	uint8_t seed_a[32];
	int8_t c[PARAMS_M] = {0};

	uint8_t r1[PARAMS_R_BYTES] = {0};
	uint8_t r2[PARAMS_R_BYTES] = {0};	
	uint8_t k_1[PARAMS_R_BYTES] = {0};
	uint8_t k_2[PARAMS_R_BYTES] = {0};
	uint8_t sigma[PARAMS_R_BYTES] = {0};

	int8_t w[PARAMS_N] = {0};
	int8_t delta[PARAMS_N] = {0};

	printf("==========TEST3 : Speed==========\n");

	//Generate bioinfo w
	randombytes(w, PARAMS_N);


	gcycles=0;
	for (int i = 0; i < TEST_LOOP; i++)
	{
		cycles1 = cpucycles();
		Gen(seed_a, c, sigma, k_1, w);
        cycles2 = cpucycles();
        gcycles += cycles2-cycles1;
	}
    printf("  Gen runs in ................. %8lld cycles\n", gcycles/TEST_LOOP);
	printf("  Gen runs in ................. %8f ms\n", (gcycles * 1000.0)/(TEST_LOOP * CLOCKS_PER_SEC));

    printf("\n"); 


	rcycles=0;
	for (int i = 0; i < TEST_LOOP; i++)
	{
		cycles1 = cpucycles();
		Rep(sigma, seed_a, c, w);
        cycles2 = cpucycles();
        rcycles += cycles2-cycles1;
	}
    printf("  Rep runs in ................. %8lld cycles\n", rcycles/TEST_LOOP); 
	printf("  Rep runs in ................. %8f ms\n", (rcycles * 1000.0)/(TEST_LOOP * CLOCKS_PER_SEC));
    printf("\n"); 

    printf("cps : %ld\n", CLOCKS_PER_SEC);
}

int main(void)
{
	//TEST0();
	//TEST1();
	//TEST2();
	TEST3();

	return 0;
}