#include "orangutan.h"

int main()
{
  motors_init();
  buzzer_init();
  lcd_init();

  clear();

  println("Press 0");

  wait_for_button_press(TOP_BUTTON);

  set_motors(100,-100);
  
  delay_ms(300);

  set_motors(0,0);
  
  play_note(0+12*4,100,15);

  while(1);
  return 0;
}
