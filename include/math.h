#pragma once

#include <core.h>
#include <xmmintrin.h>

#ifndef SR_FLOAT_EPS
    #define SR_FLOAT_EPS 0.0001
#endif

typedef union {
    __m128 sse;
    struct {
        float x;
        float y;
        float z;
        float w;
    };
} sr_vec_t;

typedef struct {
    float sin;
    float cos;
} sr_trig_t;

sr_trig_t sr_trig(register float x);
float sr_sin(const register float x);
float sr_sqrt(const register float x);
float sr_isru(const register float x);
float sr_rsqrt(const register float x);

float sr_vec_length(const register __m128 vec);
float sr_vec_scalar_mul(const register __m128 vec_1, const register __m128 vec_2);
__m128 sr_vec_normalize(const register __m128 vec, const float register vec_length);
