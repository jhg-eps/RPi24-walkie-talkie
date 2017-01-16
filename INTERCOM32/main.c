#ifndef F_CPU
#define F_CPU 1000000UL // 1 MHz internal clock speed
#endif

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "nRF24L01.h"
#include "mirf.h"
#include "spi.h"

//|------------------------------------------------------------------------|
//|            |2Flr Unit A  (M88)|Kchn Unit B (M328)| (Bsmt) Unit C (M32) |
//|------------------------------------------------------------------------|
//|Pipe 0 (RX) | 0xE7D3F03577     | 0x89B4852D52     | 0x771D9E6F77        |
//|------------------------------------------------------------------------|
//|Pipe 1 (RX) | 0xA6C0A956C6     | 0x6D025403EB     | 0xF62481D485        |
//|------------------------------------------------------------------------|
//|Pipe 2 (RX) | 0xA6C0A95667     | 0x6D025403D1     | 0xF62481D401        |
//|------------------------------------------------------------------------|
//|TX ADDR     | 0x622E805295     |                  |                     |
//|------------------------------------------------------------------------|


int main(void)
{
  uint8_t p0_raddr_val[5] = {0x77, 0x1D, 0x9E, 0x6F, 0x77 };
  uint8_t p1_raddr_val[5] = {0xF6, 0x24, 0x81, 0xD4, 0x85 };
  uint8_t tx_addr_val[5] = {0x62, 0x2E, 0x80, 0x52, 0x95 };

  // NRF24L01 Enabling and Configuration section
  mirf_init();               // initialize and configure the NRF24L01+ for our application
  sei();                    // Enable global interrupts
  mirf_config();
  mirf_set_RADDR(RX_ADDR_P0 , p0_raddr_val);  // set the receiving address for data pipe 0
  mirf_set_RADDR(RX_ADDR_P1 , p1_raddr_val);  // set the receiving address for data pipe 1
  mirf_set_TADDR(tx_addr_val);                // set the address of the transmitter unit

  // Interrrupt Enabling and Configuration section
  GICR |= (1 << INT1);      // Enable External Interrupt 1
  MCUCR |= (1 << ISC10);   // Configure Ext. Interrupt 1: "Any logical change on INT1 generates an interrupt request."
  MCUCR &= ~(1 << ISC11);  // Continued

  //Upcoming: a simple send and receive command list..
  // have to fine tune how many bytes I want the data pipe payloads to be... (it's at 16 (mirf_PAYLOAD) right now)
  //mirf_get_data(uint8_t reg, uint8_t * value, uint8_t len)
  //mirf_send(buffer,buffersize);
  //mirf_get_data(buffer);

  DDRA |= ((1 << PORTA0) | (1 << PORTA4) | (1 << PORTA5));    //PA0 = test LED, PA4 = CSN, PA5 == CE
  DDRB &= ~(1 << PORTD3);                                     // SET PD3 (INT1) as input; it is the NRF24 IRQ interrupt reading pin

  while(1) {  // just a little test blinking loop here
	  PORTA |= (1<<PORTA0); //Turns ON LED
	  _delay_ms(3000); //3 second delay
	  PORTA &= ~(1 <<PORTA0); // TURNS OFF LED
	  _delay_ms(3000);
  }
}
