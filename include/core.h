#pragma once

#include <stdint.h>

#define SR_INLINE inline __attribute__((always_inline))
#define SR_NORETURN __attribute__((noreturn))
#define SR_PACKED __attribute__((packed))

typedef enum {
    SR_FALSE,
    SR_TRUE,
} SR_PACKED sr_bool_t;

typedef enum {
    SR_UNKNOWN_ERROR_CODE = 32,

    #if defined(SR_WINDOW_USE_XCB)
        SR_WINDOW_XCB_CONNECT_ERROR_CODE,
    #elif defined(SR_WINDOW_USE_WAYLAND)
        SR_WINDOW_WAYLAND_CONNECT_ERROR_CODE,
    #endif

    SR_THREAD_CREATE_ERROR_CODE,

    SR_SUCCESS_EXIT_CODE = 0,
} SR_PACKED sr_error_t;

#ifdef SR_DEBUG
    SR_NORETURN void sr_exit(const sr_error_t error_code, ...);
#else
    SR_NORETURN void sr_exit(register const sr_error_t error_code);
#endif
