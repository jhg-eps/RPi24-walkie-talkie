#ifndef F_CPU
#define F_CPU 1000000UL // 1 MHz internal clock speed
#endif

//Atmega88A default fuses //low fuse = 0x62 //high fuse = 0xdf //extended action = 0xf9

// Preparing for PWM code:

// Using PB1 as the PWM output waveform pin.
// It is connected to Timer/Counter 1 (a 16-bit timer!)
// Will have to work with the following registers:

// TCCR1A
// TCCR1B
// TCCR1C
// DDRB (have to make sure PB1 is set for output!)
// OCR1AH and OCR1AL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void read_adc();

void main(void)            
{                                                             //76543210 (pin numbering)
   DDRD = 0xFF;                              // PORT D is entirely set for output                           PORT D is controlling the diagnostics LED
   PORTD &= 0x00;                        // PORT D is voltage low to begin with

   DDRB &= ~(1 << DDB0);        // PB0 is an input pin
   PORTB |= (1 << PORTB0);   // PB0 an input with pull-up enabled                            PORT B is reading from the PTT button

   EICRA |= 0x01;                           // external interrupt request triggered on voltage level change (no rising or falling edge consideration)

   PCICR |= (1 << PCIE0);            // set PCIE0 to enable PCMSK0 scan
                                                               // Flag is cleared when the interrupt routine is executed
   PCMSK0 |= (1 << PCINT0);   // set PCINT0 to trigger an interrupt on state change 
   sei();                                                // set the Global Interrupt Enable bit

  ADCSRA |= ((1 << ADPS1) | (1 << ADATE));    // set the ADC clock prescaler to 4, enable ADC auto-triggering (sampling rate of approximate 20 kSP/s)
  ADCSRA |= (1 << ADIE);                            // enable ADC_vect interrupt to fire off when A2D conversion occurs
  ADMUX |= (1 << REFS0);      // set AVCC (VCC) as our ADC reference voltage (relative to ground)
  ADCSRB &= ~((1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0));  //set up free-running ADC now that ADATE is enabled
  ADCSRA |= (1 << ADEN);                          // enable ADC!!!
  ADCSRA |= (1 << ADSC);                          // start doing those A2D conversions!!!!

while(1) 
{
           // waiting for something to happen!!!!!
}

}

void read_adc()
{
    uint8_t ADCL_val = ADCL;
    uint16_t ADC_cum_val = (ADCL_val | (ADCH << 8));                  // ADC_cum_val tells us a value between 0 and 1023 that maps to voltage between 0 and 5V

   if(ADC_cum_val > 500) {
            PORTD |= (1 << PORTB5);                // light up LED
            _delay_ms(1000.00);               // delay for 1 seconds
            PORTD &= ~(1 << PORTB5);            // turn off LED
   }
}

ISR(PCINT0_vect)
{
        while(!(PINB & (1 << PINB0))) {  // see if PB0 is the pin that generated the interrupt, continously monitor it.
            read_adc();
        }
}

