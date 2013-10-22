#include "gekkotest.h"

static u32 x = 123456789;
static u32 y = 362436000;
static u32 z = 521288629;
static u32 c = 7654321;

static u32 generate() {
	u64 t;
	
	x = 69069 * x + 12345;
	y ^= y << 13;
	y ^= y >> 17;
	y ^= y << 5;
	
	t = 698769069ULL * z + c;
	c = t >> 32;
	z = t;
	
	return x + y + z;
}

void prng_get(u8 *dst, u32 bytes) {
	while(bytes >= 4) {
		*(u32*)dst = generate();
		dst += 4;
		bytes -= 4;
	}
	while(bytes >= 1) {
		*dst = generate();
		dst += 1;
		bytes -= 1;
	}
}