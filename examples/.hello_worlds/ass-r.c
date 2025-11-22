int main(void) {
  register char *arg2 asm("rsi") = "hello, world!\n";
  asm("mov $1, %rax; mov $1, %rdi; mov $14, %rdx; syscall;");
}
