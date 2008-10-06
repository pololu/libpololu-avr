#include <pololu/orangutan.h>

/*
 */

int main()
{
  char buffer[20];

  clear();
  delay_ms(1000);

  // configure serial clock for 115.2 kbaud
  serial_set_baud_rate(115200);

  // read up to 20 characters
  serial_receive(buffer, 20);

  while(1)
  {
    if(button_is_pressed(BUTTON_A))
    {
      serial_send("Button A\n", 9);
      wait_for_button_release(BUTTON_A);
    }
    if(button_is_pressed(BUTTON_B))
    {
      serial_send("Button B\n", 9);
      wait_for_button_release(BUTTON_B);
    }
    if(button_is_pressed(BUTTON_C))
    {
      serial_send("Button C\n", 9);
      wait_for_button_release(BUTTON_C);
    }
    
    // display the message from the other side, if any
    if(serial_receive_buffer_full() || buffer[serial_get_received_bytes()-1] == '\n')
    {
      buffer[serial_get_received_bytes()-1] = 0;
      clear();
      print(buffer);

      // read up to 20 characters
      serial_receive(buffer, 20);

    }

  }
}
