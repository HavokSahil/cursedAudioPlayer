#pragma once

#include <cstdint>
#include <iostream>

typedef int32_t Err;

#define TAG "cap"

#define ERR_OK 0
#define ERR_NOMEM (-1)
#define ERR_ALLOC (-2)
#define ERR_UNKNOWN (-3)

#define CHECK_ERR(CODE) if (CODE != ERR_OK) return CODE

#ifdef LOGGING_ON
#define LOGI(...) fprintf(stdout, "%s ", TAG); fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n"); fflush(stdout)
#define LOGE(...) fprintf(stdout, "%s ", TAG); fprintf(stderr, __VA_ARGS__); fprintf(stderr, "\n"); fflush(stderr)
#else
#define LOGE(...) void()
#define LOGI(...) void()
#endif