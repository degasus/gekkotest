
#include <string.h>
#include <stdio.h>

#include "gekkotest.h"

int main() {
	network_init();
	
	char buffer[128];
	
	for(int mode=0; mode<2; mode++) {
	switch(mode) {
		case 0:
		print(buffer, sprintf(buffer, "round %i, IEEE mode\n", mode));
		asm("mtfsb0 29;");
		break;
		
		case 1:
		print(buffer, sprintf(buffer, "round %i, non-IEEE mode\n", mode));
		asm("mtfsb1 29;");
		break;
	}
	
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
				print(buffer, sprintf(buffer, "float add step %i, result %g / %x\n", i, a, *(u32*)&a));
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
				print(buffer, sprintf(buffer, "double add step %i, result %g / %llx\n", i, a, *(u64*)&a));
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
				print(buffer, sprintf(buffer, "ps add step %i, result %g / %x %g / %x\n", i, a[0], *(u32*)&a[0], a[1], *(u32*)&a[1]));
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
				print(buffer, sprintf(buffer, "float add 2 step %i, result %g / %x\n", i, a, *(u32*)&a));
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
				print(buffer, sprintf(buffer, "double add 2 step %i, result %g / %llx\n", i, a, *(u64*)&a));
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
				print(buffer, sprintf(buffer, "ps add 2 step %i, result %g / %x %g / %x\n", i, a[0], *(u32*)&a[0], a[1], *(u32*)&a[1]));
		}
	}
	
	};
	
	network_shutdown();
}
