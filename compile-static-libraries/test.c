#include "orangutan.h"

int main()
{
  motors_init();
  lcd_init();

  buzzer_init();
  leds_init();
  left_led(1);
  play("ceg>c<gec");

  while(is_playing());

  right_led(1);

  clear();

  println("Press 0");

  wait_for_button_press(TOP_BUTTON);

  set_motors(100,-100);
  
  delay_ms(300);

  set_motors(0,0);
  
  return 0;
}
