#ifndef F_CPU
#define F_CPU 1000000UL // 1 MHz internal clock speed
#endif

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "nRF24L01.h"
void read_adc();

// Using http://gizmosnack.blogspot.com/2013/04/tutorial-nrf24l01-and-avr.html
// as my source for the SPI communication code

//The SPI data register is a read/write register used for data transfer between the register file and the SPI shift register. Writing
//to the register initiates data transmission. Reading the register causes the shift register receive buffer to be read.

//When a serial transfer is complete, the SPIF flag is set. An interrupt is generated if SPIE in SPCR is set and global interrupts
//are enabled. If SS is an input and is driven low when the SPI is in master mode, this will also set the SPIF flag. SPIF is
//cleared by hardware when executing the corresponding interrupt handling vector. Alternatively, the SPIF bit is cleared by
//first reading the SPI status register with SPIF set, then accessing the SPI data register (SPDR).

char SPI_write_byte(unsigned char uns_char_data)
{
   SPDR = uns_char_data; // SPI Data Register is being written to; initiates data transmission between the register file and the SPI shift register
   while(!(SPSR & (1 << SPIF))); // allow for the data transmission to complete (wait for SPIF to be set)
                                 // SPIF is set when when a serial transfer is complete
   return SPDR; // SPDR returns data from the NRF24L01
}

uint8_t get_reg_contents(uint8_t registerl)
{  // leaving 10us between commands because the NRF accepts 1 byte every 10 us
	_delay_us(10);   // give plenty of time for other commands to complete
	PORTB &= ~(1 << PORTB2); // set PB2 (CSN) low; then the NRF24 will start listening for commands
	_delay_us(10);
	SPI_write_byte(R_REGISTER + registerl); // stores (R_REGISTER + register in the SPDR register)
	                                      // SPDR will be read by the NRF24 and the next time the AVR writes to the SPDR (that is, initiates a data transmission, the NRF will return
	                                      // the contents of "register" (since it was asked to provide information via the "R_REGISTER" command
	_delay_us(10);
	registerl = SPI_write_byte(NOP); // put some random junk in SPDR, and it returns us what we asked for previously
	_delay_us(10);
	PORTB |= (1 << PORTB2);   // CSN set high again, the NRF24 will not be listening for command
	return registerl;
}

void write_to_NRF(uint8_t registerl, uint8_t information)
{
	_delay_us(10) // give that 10 microsecond delay.
	PORTB &= ~(1 << PORTB2) // set CSN (PB2) low so that the NRF listens for commands/data
	_delay_us(10);
	SPI_write_byte(W_REGISTER + registerl); // tell the NRF24 we want to write to one of its registers (register "registerl", specifically)
	_delay_us(10);
	SPI_write_byte(information); // write information to register registerl
	_delay_us(10);
	PORTB |= (1 << PORTB2); // CSN high again, NRF24 is left alone now
}

uint8_t * array_write_toNRF(uint8_t RorW, uint8_t registerl, uint8_t data_array, uint8_t no_of_entries)
{
  // Coming shortly!
}
//Atmega88A default fuses //low fuse = 0x62 //high fuse = 0xdf //extended action = 0xf9

// Preparing for PWM code:

// Using PB1 as the PWM output waveform pin.
// It is connected to Timer/Counter 1 (a 16-bit timer!)
// Will have to work with the following registers:


void main(void)
{                                                             //76543210 (pin numbering)
   DDRD |= (1 << DDD5);                              // PORT D is entirely set for output                           PORT D is controlling the diagnostics LED
   // Set PD5 as output                        // PORT D is voltage low to begin with

   DDRB &= ~(1 << DDB0);        // PB0 is an input pin

   // BEGIN SPI INIT SECTION
   DDRB |= ((1 << DDB5) | (1 << DDB3) | (1 << DDB2) | (1 << DDB1));
   // The above sets the SPI pins (which will communicate with the NRF24L01+) as outputs
   // Also sets the PWM pin as output

   SPCR |= ((1 << SPE) | (1 << MSTR));
   // Enable SPI communication, set the AVR as a master

   PORTB |= (1 << PORTB2);  // CSN set high; nRF24L01+ will not be listening to commands this way
   PORTD &= ~(1 << PORTD5);   // CE set low; we're in receiving mode this way

   // END SPI INIT SECTION
   PORTB |= (1 << PORTB0);   // PB0 an input with pull-up enabled                            PORT B is reading from the PTT button
   PORTB |= (1<< PORTB1);
   EICRA |= 0x01;                           // external interrupt request triggered on voltage level change (no rising or falling edge consideration)

   PCICR |= (1 << PCIE0);            // set PCIE0 to enable PCMSK0 scan
                                                               // Flag is cleared when the interrupt routine is executed
   PCMSK0 |= (1 << PCINT0);   // set PCINT0 to trigger an interrupt on state change
   sei();                                                // set the Global Interrupt Enable bit

  ADCSRA |= ((1 << ADPS1) | (1 << ADPS0) | (1 << ADATE));    // set the ADC clock prescaler to 4, enable ADC auto-triggering (sampling rate of approximate 20 kSP/s)
  ADCSRA |= (1 << ADIE);                            // enable ADC_vect interrupt to fire off when A2D conversion occurs
  ADMUX |= (1 << REFS0);      // set AVCC (VCC) as our ADC reference voltage (relative to ground)
  ADCSRB &= ~((1 << ADTS2) | (1 << ADTS1) | (1 << ADTS0));  //set up free-running ADC now that ADATE is enabled
 ADCSRA |= (1 << ADEN);                          // enable ADC!!!
  ADCSRA |= (1 << ADSC);                          // start doing those A2D conversions!!!!

while(1){
}

}

void read_adc()
{
    uint8_t ADCL_val = ADCL;          // read low (then high) ADC data bytes
    uint8_t ADCH_val = ADCH;
    //uint16_t ADC_cum = (ADCL_val | (ADCH_val << 8));
    OCR1AH = ADCH_val;
    OCR1AL = ADCL_val;
}

ISR(PCINT0_vect)
{
     TCCR1A |= ((1 << COM1A1) | (1 << WGM11) | (1 << WGM10));  // set up OC1A (mapped to PB1) to have non-inverting output
                                                 // note: setting up WGM (wave gen mode) 14
     	   	   	   	   	   	   	   	   	   	   	   // WGM 14 sets up: Fast PWM, TOP = ICR1
     TCCR1B |= ((1 << WGM12) | (1 << 5) | (1 << CS10));
     TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B));
     ADCSRA |= (1 << ADSC);
        while(!(PINB & (1 << PINB0))) {  // see if PB0 is the pin that generated the interrupt, continously monitor it.
            read_adc();
        }
        TCCR1A &= ~((1 << COM1A1) | (1 << WGM11) | (1 << WGM10));  // set up OC1A (mapped to PB1) to have non-inverting output
                                                     // note: setting up WGM (wave gen mode) 14
         	   	   	   	   	   	   	   	   	   	   	   // WGM 14 sets up: Fast PWM, TOP = ICR1
        TCCR1B &= ~((1 << WGM12) | (1 << 5) | (1 << CS10));
        TCCR1C &= ~((1 << FOC1A) | (1 << FOC1B));
        ADCSRA &= ~(1 << ADSC);
        OCR1AH = 0;            // write high (then low) PWM data
        OCR1AL = 0;
}
