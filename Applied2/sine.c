#include <math.h>
#include <stdio.h>

double rad(int deg) { return (double)deg / 180 * M_PI; }

int main() {
  double sine = 0;
  for (int deg = 0; deg <= 360; deg += 10) {
    sine = sin(rad(deg));
    printf("%3d\t%.3f\n", deg, sine);
  }
  return 0;
}
