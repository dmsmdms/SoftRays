#pragma once

#include <stdatomic.h>
#include <core.h>

typedef int (*sr_thread_t)(void *);
void sr_thread_create(sr_thread_t thread_func, void * thread_args);
