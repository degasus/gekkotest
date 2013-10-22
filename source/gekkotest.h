#ifndef __GEKKOTEST_H__
#define __GEKKOTEST_H__

#include <gctypes.h>

static const u16 SERVER_PORT = 4123;
static const bool DEBUG_ALL = true;

// main.cpp
int main();

// network.cpp
void print(const char* str, u32 len);
void network_init();
void network_shutdown();

// prng.cpp
void prng_get(u8 *dst, u32 bytes);
template<typename T> inline void prng_get(T *dst) {prng_get(dst, sizeof(T));}

// floats.cpp
void float_checkall();

#endif