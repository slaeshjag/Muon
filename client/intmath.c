#include <math.h>

#include "intmath.h"

void intmath_init() {
	int i;

	for (i = 0; i < 360; i++) {
		sinetable[i] = sin(M_PI * i / 180)*3600;// * 0x100;
	}
	return;
}

int sine(int angle) {
	angle %= 360;
	if (angle < 0)
		angle = 360 + angle;
	return sinetable[angle];
}
