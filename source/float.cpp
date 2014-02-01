
#include <string.h>
#include <stdio.h>

#include "gekkotest.h"

inline static u32 fti(float f) {
	union {float f; u32 i;} u;
	u.f = f;
	return u.i;
}
inline static u64 fti(double f) {
	union {double f; u64 i;} u;
	u.f = f;
	return u.i;
}

static void float_arith_test() {
	// float addition
	{
		float a = 1.0;
		for(int i=0; i<32; i++) {
			float b = a;
			for(int j=0; j<123; j++) {
				//b += a;
				asm("fadds %0, %1, %2;" : "=f"(b) : "f"(b), "f"(a));
			}
			a = b;
			if(i == 16 || i == 31 || DEBUG_ALL)
				print("float add step %i, result %g / %x\n", i, a, fti(a));
		}
	}

	// double addition
	{
		double a = 1.0;
		for(int i=0; i<128; i++) {
			double b = a;
			for(int j=0; j<1234; j++) {
				//b += a;
				asm("fadd %0, %1, %2;" : "=f"(b) : "f"(b), "f"(a));
			}
			a = b;
			if(i == 64 || i == 127 || DEBUG_ALL)
				print("double add step %i, result %g / %llx\n", i, a, fti(a));
		}
	}

	// ps addition
	{
		float a[] = {1.0, 2.0};
		for(int i=0; i<32; i++) {
			float b[] = {a[0], a[1]};
			for(int j=0; j<123; j++) {
				//b[0] += a[0];
				//b[1] += a[1];
				asm(
					"ps_merge00 %0, %2, %3;"
					"ps_merge00 %1, %4, %5;"
					"ps_add %0, %0, %1;"
					"ps_merge11 %1, %0, %0;"
					"ps_merge00 %0, %0, %0;"
					: "=f"(b[0]), "=f"(b[1]) : "f"(b[0]), "f"(b[1]), "f"(a[0]), "f"(a[1])
				);
			}
			a[0] = b[0];
			a[1] = b[1];
			if(i == 16 || i == 31 || DEBUG_ALL)
				print("ps add step %i, result %g / %x %g / %x\n", i, a[0], fti(a[0]), a[1], fti(a[1]));
		}
	}

	// float division
	{
		float a = 123456789.4321;
		float b = 1.79685746;
		for(int i=0; i<256; i++) {
			// a /= b
			asm("fdivs %0, %1, %2;" : "=f"(a) : "f"(a), "f"(b));
			if(i == 16 || i == 31 || DEBUG_ALL)
				print("float div step %i, result %g / %x\n", i, a, fti(a));
		}
	}

	// float addition 2
	{
		float a = 1.0;
		float c = 0.041f;
		for(int i=0; i<128; i++) {
			float b = a;
			for(int j=0; j<9; j++) {
				//b += a;
				asm("fadds %0, %1, %2;" : "=f"(b) : "f"(b), "f"(a));
			}
			//a = b * c;
			asm("fmuls %0, %1, %2;" : "=f"(a) : "f"(b), "f"(c));
			if(i == 64 || i == 127 || DEBUG_ALL)
				print("float add 2 step %i, result %g / %x\n", i, a, fti(a));
		}
	}

	// double addition 2
	{
		double a = 1.0;
		double c = 0.041;
		for(int i=0; i<1024; i++) {
			double b = a;
			for(int j=0; j<9; j++) {
				//b += a;
				asm("fadd %0, %1, %2;" : "=f"(b) : "f"(b), "f"(a));
			}
			//a = b * c;
			asm("fmul %0, %1, %2;" : "=f"(a) : "f"(b), "f"(c));
			if(i == 512 || i == 1023 || DEBUG_ALL)
				print("double add 2 step %i, result %g / %llx\n", i, a, fti(a));
		}
	}

	// ps addition 2
	{
		float a[] = {1.0, 2.0};
		float c = 0.041f;
		for(int i=0; i<128; i++) {
			float b[] = {a[0], a[1]};
			for(int j=0; j<9; j++) {
				//b[0] += a[0];
				//b[1] += a[1];
				asm(
					"ps_merge00 %0, %2, %3;"
					"ps_merge00 %1, %4, %5;"
					"ps_add %0, %0, %1;"
					"ps_merge11 %1, %0, %0;"
					"ps_merge00 %0, %0, %0;"
					: "=f"(b[0]), "=f"(b[1]) : "f"(b[0]), "f"(b[1]), "f"(a[0]), "f"(a[1])
				);
			}
			//a[0] = b[0] * c;
			//a[1] = b[1] * c;
			asm(
				"ps_merge00 %0, %2, %3;"
				"ps_merge00 %1, %4, %4;"
				"ps_mul %0, %0, %1;"
				"ps_merge11 %1, %0, %0;"
				"ps_merge00 %0, %0, %0;"
				: "=f"(a[0]), "=f"(a[1]) : "f"(b[0]), "f"(b[1]), "f"(c)
			);
			if(i == 64 || i == 127 || DEBUG_ALL)
				print("ps add 2 step %i, result %g / %x %g / %x\n", i, a[0], fti(a[0]), a[1], fti(a[1]));
		}
	}
}

static void float_memcpy_test() {
	// turn on non-IEEE mode:
	asm("mtfsb1 29");

        // all the NaNs are in Intel encoding since that is where things could go wrong
        u32 numbers[] = {
            0xff800000, // -infinity
            0x807fffff, // smallest denormal
            0x80000001, // largest negative denormal
            0x80000000, // -0
            0x00000000, // +0
            0x00000001, // smallest positive denormal
            0x007fffff, // largest denormal
            0x7f800000, // +infinity
            0x7fc00001, // +QNaN
            0x7f800001, // +SNaN
            0xffc00000, // indefinite value (special QNaN)
            0xffc00001, // -QNaN
            0xff800001, // -SNaN
            0xffffffff, // all 1s (NaN)
        };

        size_t i;
        for (i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++) {
                u32 input = numbers[i];
                u32 output = 0x12345678;
                float pseudovar;
                asm volatile(
                        "lfs %[temp], %[input];"
                        "stfs %[temp], %[output];"
                        : [output]"=m"(output), [temp]"=f"(pseudovar)
                        : [input]"m"(input)
                );
                if (output == input) {
                        print("PASS: 0x%08x\n", input);
                } else {
                        print("FAIL: copying a specific float in non-IEEE mode with lfs/stfs modifies its value:\n"
                              "expected 0x%08x, got 0x%08x\n", input, output);
                }
        }
}

void float_run_tests() {
	print("Begin floating point tests\n");
	/*
	for(int mode=0; mode<8; mode++) {

		print("round %i, non-ieee %i, rounding %i\n", mode, mode & 1, mode>>1);

		// non-ieee mode
		if (mode & 1)
			asm("mtfsb1 29;");
		else 
			asm("mtfsb0 29;");
		
		// rounding
		if (mode & 2)
			asm("mtfsb1 30;");
		else 
			asm("mtfsb0 30;");
		if (mode & 4)
			asm("mtfsb1 31;");
		else 
			asm("mtfsb0 31;");
		
		float_arith_test();
	}
	*/
	float_memcpy_test();
	print("finish floating point tests\n");
}
