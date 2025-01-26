#include <unistd.h>

int main() {
  char flag[0x80];
  write(1, "FLAG: ", 6);
  read(0, flag, 0x80);
  write(1, "Wrong.\n", 7);
  return 0;
}
