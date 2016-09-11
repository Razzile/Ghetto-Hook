#include "ghetto_hook.h"
#include <stdio.h>

int original() { return 0; }

int redirect() { return 99; }

int main() {
  ghetto_hook((vm_address_t)original, (vm_address_t)redirect);
  printf("original() returned %d\n", original());
}
