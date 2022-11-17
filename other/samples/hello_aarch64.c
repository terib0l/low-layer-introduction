#include <unistd.h>
#include <sys/syscall.h>

int main() {
  syscall(64, 1, "Hello\n", 6);
  return 0;
}
