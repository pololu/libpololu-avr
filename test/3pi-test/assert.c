#include <pololu/orangutan.h>
#include "assert.h"

void assert(unsigned char truth)
{
  if(!truth)
  {
    play("O3c");
    wait_for_button(ALL_BUTTONS);
  }
}
