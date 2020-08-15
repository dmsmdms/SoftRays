#include "thread.h"

#include <malloc.h>

#ifndef SR_THREAD_STACK_SIZE
    #define SR_THREAD_STACK_SIZE (32 * 1024)
#endif

#ifdef SR_PLATFORM_LINUX
    #define __USE_GNU
    #include <sched.h>
    #define CE_THREAD_FLAGS (CLONE_PARENT | CLONE_FILES | CLONE_VM)

    SR_INLINE void sr_thread_create(sr_thread_t thread_func, void * thread_args) {
        void * thread_stack = malloc(SR_THREAD_STACK_SIZE);

        if (clone(thread_func, thread_stack, CE_THREAD_FLAGS, thread_args) < 0) {
            sr_exit(SR_THREAD_CREATE_ERROR_CODE);
        }
    }

    SR_INLINE void sr_thread_spinlock_wait(volatile register atomic_uint_least8_t * restrict barier) {
        for (*barier--; barier[0]; /* void */ );
    }
#endif
