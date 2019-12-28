#include "testing.h"

void assertTrue(int assertion, char *test) {
	if (assertion) {
		printf("[OK] - %s\n", test);
	} else {
		printf("[ERROR] - %s\n", test);
		printf("  Expected: true\n");
	}
}
