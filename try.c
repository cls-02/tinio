#include <stdio.h>

int main () {
  unsigned a = 10;
  for (a -= 1; a; a--) {
    printf("%d", a);
  }
}
