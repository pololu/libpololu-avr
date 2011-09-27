#include <pololu/orangutan.h>

int main()
{
  svp_set_mode(SVP_MODE_RX);
  lcd_init_printf();

  while(1)
  {
    clear();
    print("Orangutan SVP"); 
    lcd_goto_xy(0, 1);
    printf("%d", get_ms());

    red_led(1);
    delay_ms(200);

    red_led(0);
    delay_ms(200);
  }
}
