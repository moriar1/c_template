#include <dlfcn.h>

int main(void) {
  void *handle = dlopen("libc.so.6", RTLD_LAZY);
  int (*printf_func)(const char *, ...) = dlsym(handle, "printf");
  printf_func("Hello World!\n");
  dlclose(handle);
}
