
#include <string.h>
#include <stdio.h>

#include "gekkotest.h"

int main() {
	network_init();

	float_run_tests();

	network_shutdown();
}
