#ifndef F_CPU
#define F_CPU 1000000UL // 16 MHz clock speed
#endif

//Atmega88A default fuses

//low fuse = 0x62
//high fuse = 0xdf
//extended action = 0xf9

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//PD5 = output (LED)
//PD6 = input


ISR(PCINT22_vect)
{
        PORTD = 0xBF;                    // if button pressed PD5 goes high (light LED), PD6 is set low
        delay(5000);
}

void main(void)            //   1011
{                                                             //76543210 (pin numbering)
   PCICR |= (1 << PCIE2);            // set PCIE2 so that PCINT22 (pin PD6) can be an external interrupt input
   //PCIFR |= (1 << PCIF2);            // When PCINT22 is triggered,  PCIF2 becomes set (one). IF SREG GIE and PCIE2 are set, MCU executes ISR. 
                                                              // Flag is cleared when the interrupt routine is executed
   PCMSK2 |= (1 << PCINT22);  // activate PCINT22 (PD6) as an (external) interrupt source.
   sei();                                                // set the Global Interrupt Enable bit
   DDRD = 0xBF;                              //PORTD output, 5 is output, 6 is input 
   PORTD &= 0xDF;                        //PORTD '0'       set 6 high and 5 low to start

while(1) 
{
	for(;;) {
		// infinite loop
	}
        PORTD = 0xDF;                  // default condition (PD5 low, PD6, high.
}

}