#include <core.h>
#include <stdlib.h>

#ifdef SR_DEBUG
    #include <stdio.h>
    #include <unistd.h>
    #include <stdarg.h>

    #define sr_err_msg(i) (i - SR_UNKNOWN_ERROR_CODE)

    SR_NORETURN void sr_exit(const sr_error_t error_code, ...) {
        static const char * error_messages[] = {
            [sr_err_msg(SR_UNKNOWN_ERROR_CODE)] = "Unknown error!\n",
            #if defined(SR_WINDOW_USE_XCB)
                [sr_err_msg(SR_WINDOW_XCB_CONNECT_ERROR_CODE)] = "Can`t connect to XOrg Server using XCB!\n",
            #else
                [sr_err_msg(SR_WINDOW_WINAPI_CREATE_ERROR_CODE)] = "Can`t create WinAPI window!\n",
            #endif
            [sr_err_msg(SR_THREAD_CREATE_ERROR_CODE)] = "Can`t create new thread!\n",
        };

        va_list error_args;
        va_start(error_args, error_code);

        vfprintf(stderr, error_messages[sr_err_msg(error_code)], error_args);

        va_end(error_args);
        exit(error_code);
    }
#else
    SR_INLINE SR_NORETURN void sr_exit(register const sr_error_t error_code) {
            exit(error_code);
    }
#endif
