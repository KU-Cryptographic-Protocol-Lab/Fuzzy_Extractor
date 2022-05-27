#ifndef _PARAMS_H_
#define _PARAMS_H_
//#define SHAKE shake128

#define PARAM_128

#define HWT_1

#define PARAMS_LOGQ 8
#define PARAMS_LOGL 4
#define PARAMS_L (1 << PARAMS_LOGL)

#ifdef PARAM_80
	#include "bch_255_87_26/bch_255_87_26.h"
	#include "bch_255_87_26/bch_255_87_26_poly.h"

	#define PARAMS_M ((1 << GF_M) - 1)
	#define PARAMS_N 160

	#define PARAMS_K 27
	#define PARAMS_R_BYTES 10
#endif

#ifdef PARAM_128
	#include "bch_511_130_55/bch_511_130_55.h"
	#include "bch_511_130_55/bch_511_130_55_poly.h"

	#define PARAMS_M ((1 << GF_M) - 1)
	#define PARAMS_N 256

	#define PARAMS_K 21
	#define PARAMS_R_BYTES 16
#endif

#ifdef PARAM_256
	#include "bch_1023_258_106/bch_1023_258_106.h"
	#include "bch_1023_258_106/bch_1023_258_106_poly.h"

	#define PARAMS_M ((1 << GF_M) - 1)
	#define PARAMS_N 512


	#define PARAMS_K 15
	#define PARAMS_R_BYTES 32
#endif

#endif