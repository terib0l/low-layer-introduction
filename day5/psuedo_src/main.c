#include <stdio.h>

#include "tcb.h"
#include "mini_os.h"

int main(int argc, char* argv[])
{
  // _led_initialize();

  // _systick_initialize();

  _minios_start();

  return 0;
}
