/*******************************************************
Project : USART to ALL
Version : 0
Date    : 16.06.2018
Author  : Germidar
Company :
Comments:

Chip type               : ATmega16A
Program type            : Application
AVR Core Clock frequency: 14,745600 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*******************************************************/

#include <mega16a.h>
#include <delay.h>
#include <1-wire.c>

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)

unsigned char COMBUFFER[256];
unsigned char pBuf = 0x00;

void COM_Tx (unsigned char data)
{
while(!(UCSRA & (1<<UDRE)));
UDR = data;
}

// USART Receiver interrupt service routine
interrupt [USART_RXC] void usart_rx_isr(void)
{
char status,data;
status=UCSRA;
data=UDR;
if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
	{
    if (pBuf < 255)
    	{
        COMBUFFER[pBuf] = data;
        pBuf++;
    	}
    else
    	{
        COMBUFFER[pBuf] = data;
        pBuf = 0x00;
        }
    }
else
	{

	}

}


// USART Transmitter interrupt service routine
interrupt [USART_TXC] void usart_tx_isr(void)
{

}

void ComParser (void)
{

}

void main(void)
{

// USART initialization 115200 8N1
UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
UCSRB=(1<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
UBRRH=0x00;
UBRRL=0x07;
#asm("sei");

while (1)
{
COM_Tx('R');
COM_Tx('D');
COM_Tx('Y');
COM_Tx(13);
COM_Tx(10);
delay_ms(1000);
}
}
