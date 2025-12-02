#include <stdio.h>

int main(void) {
  // You should see `-Wformat-insufficient-args` warning by compiler and
  // `clang-analyzer-unix.Malloc` warning by clang-tidy
  return printf("Hello `%s`, world!") == EOF;
}
