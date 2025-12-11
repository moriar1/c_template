#include <stdio.h>

int func(int *a) { return *a; }

int main(void) {
  // You should see `-Wformat-insufficient-args` warning by compiler and
  // `clang-diagnostic-format-insufficient-args` warning by clang-tidy
  int placeholder = 8;
  return printf("Hello `%s`, world!") == EOF;
}
