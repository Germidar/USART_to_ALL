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
//#include <CommandManager.c>

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<DOR)

unsigned char COMBUFFER[256];
unsigned char pBuf = 0x00;
unsigned char pFlag = 0x00;     // Buffer status flags
                                // bit 7    1 Прийшов байт даних
                                // bit 6    1 Прийшов пакет даних

unsigned char TMP;
unsigned int TEMP;

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
    	pFlag |= 0x80;      // Встановлення флагу що є нові дані.
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

unsigned short crc16 (unsigned char *pcBlock, unsigned short len)
{
unsigned short crc = 0xFFFF;
unsigned char i;

while (len--)
    {
    crc ^= *pcBlock++ << 8;
    for (i=0;i<8;i++)
    crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
return crc;
}

// USART Transmitter interrupt service routine
interrupt [USART_TXC] void usart_tx_isr(void)
{

}

void ClearCOMBUFFER (void)
{
unsigned char p = 0xFF;
do
    {
    COMBUFFER[p] = 0x00;
    }
while (p--);
}

void ShiftComBuffer (unsigned char position)
{
unsigned char a, b;
PORTA.0 = 0xFF;
a = 0x00;
for (b=position;b<254;b++)
    {
    COMBUFFER[a] = COMBUFFER[b];
    a++;
    }
PORTA.0 = 0x00;
}

void ComParser (void)
{
unsigned int cTEMP = 0x00;
unsigned char cTMP = 0x00;
unsigned char cOffset = 0x00;
unsigned char n;

if (pFlag & 0x80)
    {
    for(n=0;n<=pBuf;n++)
        {
        if (COMBUFFER[n] == 0x1B)
            {
            cOffset = n;
            cTMP = COMBUFFER[n+1];
            if (pBuf >= ((n + cTMP) + 4))
                {
                cTEMP = crc16(COMBUFFER + cOffset, cTMP + 4);
                TEMP = cTEMP;

                if (!cTEMP)
                    {
                    n=0xFE;
                    pFlag |= 0x40;  // Отримано пакет даних.
                    //ShiftComBuffer(cOffset + 8);
                    }
                }



            }
        else
            {

            }
        }


    pFlag &= 0x7F;



    }
}

void main(void)
{
DDRA=0x01;
// USART initialization 115200 8N1
UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<UPE) | (0<<U2X) | (0<<MPCM);
UCSRB=(1<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
UBRRH=0x00;
UBRRL=0x07;
#asm("sei");

while (1)
{
//COM_Tx('R');
//COM_Tx('D');
//COM_Tx('Y');
//COM_Tx(' ');
//COM_Tx(13);
//COM_Tx(10);

if (pFlag & 0x40)
    {
    pFlag &= 0xBF;

    COM_Tx('O');
    COM_Tx('K');
    COM_Tx(13);
    COM_Tx(10);
    //ClearCOMBUFFER();
    }


if (pFlag & 0x01)
    {
    //ClearCOMBUFFER();
    }
//delay_ms(1000);
ComParser();
}
}
