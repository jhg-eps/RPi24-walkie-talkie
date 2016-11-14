#ifndef F_CPU
#define F_CPU 1000000UL // 16 MHz clock speed
#endif

//Atmega88A default fuses //low fuse = 0x62 //high fuse = 0xdf //extended action = 0xf9

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(PCINT0_vect)
{
        if (!(PINB & (1 << PINB0))) {  // see if PB0 is the pin that generated the interrupt
            PORTD |= (1 << PORTB5);                // light up LED
            _delay_ms(1000.00);               // delay for 5 seconds
            PORTD &= ~(1 << PORTB5);            // turn off LED
        }
}

void main(void)            
{                                                             //76543210 (pin numbering)
   DDRD = 0xFF;                              // PORT D is entirely set for output
   PORTD &= 0x00;                        // PORT D is voltage low to begin with

   DDRB &= ~(1 << DDB0);        // PB0 is an input pin
   PORTB |= (1 << PORTB0);   // PB0 an input with pull-up enabled

  EICRA |= 0x01;                           // external interrupt request triggered on voltage level change (no rising or falling edge consideration)
//  EIMSK  |= 0x01;

   PCICR |= (1 << PCIE0);            // set PCIE0 to enable PCMSK0 scan
                                                               // Flag is cleared when the interrupt routine is executed
   PCMSK0 |= (1 << PCINT0);   // set PCINT0 to trigger an interrupt on state change 
   sei();                                                // set the Global Interrupt Enable bit

while(1) 
{
           // waiting for something to happen!!!!!
}

}