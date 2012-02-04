#include <pololu/orangutan.h>  
  
/*  
 * digital1: for the Orangutan LV, SV, SVP, X2, Baby-O and 3pi robot.
 *  
 * This example uses the OrangutanDigital functions to read a digital 
 * input and set a digital output.  It takes a reading on pin PC1, and 
 * provides feedback about the reading on pin PD1 (the red LED pin). 
 * If you connect a wire between PC1 and ground, you should see the 
 * red LED change state. 
 *  
 * http://www.pololu.com/docs/0J20 
 * http://www.pololu.com  
 * http://forum.pololu.com  
 */   
  
int main()  
{  
    // Make PC1 be an input with its internal pull-up resistor enabled.  
    // It will read high when nothing is connected to it.  
    set_digital_input(IO_C1, PULL_UP_ENABLED);  
  
    while(1)  
    {  
        if(is_digital_input_high(IO_C1))     // Take digital reading of PC1.  
        {  
            set_digital_output(IO_D1, HIGH); // PC1 is high, so drive PD1 high.  
        }  
        else  
        {  
            set_digital_output(IO_D1, LOW);  // PC1 is low, so drive PD1 low.  
        }  
    }  
}