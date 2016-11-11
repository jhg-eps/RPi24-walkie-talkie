#ifndef F_CPU
#define F_CPU 1000000UL // 16 MHz clock speed
#endif

//Atmega88A default fuses

//low fuse = 0x62
//high fuse = 0xdf
//extended action = 0xf9

#include <avr/io.h>
#include <util/delay.h>

//PD5 = output (LED)
//PD6 = input

void main(void)            //   1011
{                                                             //76543210 (pin numbering)
   DDRD = 0xBF;                              //PORTD output, 5 is output, 6 is input 
   PORTD &= 0xDF;                        //PORTD '0'       set 6 high and 5 low to start

while(1) //0x9F
{
    if( !(PIND & (1<<DDD6)) )   // if button pressed PD5 goes high (light LED), PD6 is set low
        PORTD = 0xBF;                    // see the above comment
    if(PIND & (1<<DDD6))         // if button not pressed (PD5 goes low (no LED), pd6 is set high in preparation for another button press
        PORTD = 0xDF;                  // go to original condition
}
}