#ifndef INTMATH_H
#define INTMATH_H

#define cosine(x) (sine((x)+90))

int sinetable[360];

void intmath_init();
int sine(int angle);

#endif
