#include <math.h>

#ifndef SR_SIN_EPS
    #define SR_SIN_EPS 0.01
#endif

#ifndef SR_MATH_PI
    #define SR_MATH_PI 3.14159
#endif

typedef union {
    __m128 sse;
    float values[4];
} sr_sse_t;

SR_INLINE float sr_sqrt(const register float x) {
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(x)));
}

SR_INLINE float sr_rsqrt(const register float x) {
    return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(x)));
}

/* Inverse Square Root Unit */
SR_INLINE float sr_isru(const register float x) {
    return x * sr_rsqrt(1 + x * x);
}

SR_INLINE float sr_vec_scalar_mul(const register __m128 vec_1, const register __m128 vec_2) {
    sr_vec_t result_vec = {
        .sse = _mm_mul_ps(vec_1, vec_2),
    };

    return result_vec.x + result_vec.y + result_vec.z;
}

SR_INLINE float sr_vec_length(const register __m128 vec) {
    return sr_sqrt(sr_vec_scalar_mul(vec, vec));
}

SR_INLINE __m128 sr_vec_normalize(const register __m128 vec, const register float vec_length) {
    return _mm_div_ps(vec, _mm_set_ps1(vec_length));
}

SR_INLINE float sr_fabs(const register float x) {
    union {
        uint32_t i;
        float f;
    } temp = {
        .f = x,
    };

    temp.i &= (uint32_t)(1 << 31) - 1;
    return temp.f;
}

inline sr_trig_t sr_trig(register float x) {
    sr_trig_t result;

    x -= (uint32_t)(x / (2 * SR_MATH_PI)) * (2 * SR_MATH_PI);

    result.sin = (x > SR_MATH_PI) ? -sr_sin(x - SR_MATH_PI) : sr_sin(x);
    result.cos = sr_sqrt(1 - result.sin * result.sin);

    if (x > (SR_MATH_PI / 2) && x < (3 * SR_MATH_PI / 2)) {
        result.cos = -result.cos;
    }

    return result;
}

float sr_sin(const register float x) {
    register __m128 numerators_step_sse;
    sr_sse_t numerators;

    {
        register float numerator_step = -(x * x), numerator_tmp = x;

        for (register uint8_t i = 0; SR_TRUE; numerator_tmp *= numerator_step) {
            numerators.values[i] = numerator_tmp;

            if (++i >= 4) {
                break;
            }
        }

        numerators_step_sse = _mm_set_ps1(numerator_tmp * x);
    }

    register __m128 result_sse = _mm_set_ps1(0);

    for (register float denominator_step = 2, denominator_tmp = 1; numerators.values[0] > SR_SIN_EPS; /* void */) {
        sr_sse_t denominators;

        for (register uint8_t i = 0; i < 4; i++) {
            denominators.values[i] = denominator_tmp;
            denominator_tmp /= denominator_step * (denominator_step + 1);
            denominator_step += 2;
        }

        result_sse = _mm_add_ps(result_sse, _mm_mul_ps(numerators.sse, denominators.sse));
        numerators.sse = _mm_mul_ps(numerators.sse, numerators_step_sse);
    }

    numerators.sse = result_sse;
    return numerators.values[0] + numerators.values[1] + numerators.values[2] + numerators.values[3];
}
