/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* Global variables                                                           */
/******************************************************************************/
extern volatile unsigned int  HoldingRegister[50];
extern volatile unsigned char Coils[50];
extern volatile unsigned char Response[125]; //Enough to return all holding-r's
extern volatile unsigned char Received[125]; //Enough to write all holding-r's
extern volatile char ModbusMessage,MessageLength;

/******************************************************************************/
/* User Functions  Define your own functions here                             */
/******************************************************************************/

//example user function
void long_delay_ms (unsigned int ms)
{
  unsigned int i;
   for (i = 0; i < ms; i++)
   __delay_ms(1)   ;
}

void InitPorts (void)
{ //initialize your ports here.  The following is just an example
  TRISBbits.TRISB0 = 0;    // set Port B bit 0 as output
  TRISBbits.TRISB1 = 0;    // set Port B bit 1 as output
  TRISBbits.TRISB2 = 1;    // set Port B bit 2 as input
  TRISBbits.TRISB6 = 0;    // set Port B bit 6 as output
  TRISD = 0;               // set Port D as OUTPUT
  TRISAbits.TRISA1 = 0;    // set Port A bit 1 as output
  TRISAbits.TRISA5 = 0;    // set Port A bit 5 as output
  TRISCbits.TRISC0 = 0;    // set Port C bit 0 as output
  TRISCbits.TRISC1 = 0;    // set Port C bit 1 as output
  TRISCbits.TRISC2 = 0;    // set Port C bit 2 as output
  TRISEbits.TRISE0 = 0;    // set Port E bit 0 as output
  TRISEbits.TRISE1 = 0;    // set Port E bit 1 as output
  TRISEbits.TRISE2 = 0;    // set Port E bit 2 as output
  INTCON2bits.RBPU = 1; // enable PORTB internal pullups
}

void Initialize (void)
{ //Enter your initilaization code here.  The following is just an example
  PORTDbits.RD0 = 1;   //blinks some leds to show it's working
  long_delay_ms(250);
  PORTDbits.RD0 = 0;
  long_delay_ms(1000);
  PORTDbits.RD1 = 1;
  long_delay_ms(1000);
  PORTDbits.RD1 = 0;
  long_delay_ms(250);
  PORTDbits.RD2 = 1;
  long_delay_ms(250);
  PORTDbits.RD2 = 0;
  long_delay_ms(2000);
  PORTDbits.RD3 = 1;
  long_delay_ms(250);
  PORTDbits.RD3 = 0;
  long_delay_ms(2000);
}
void ModbusEvents (void)
{ //Enter what you want to happen during a modbus event here. The following is just an example
  LATD = HoldingRegister[0]; //whatever is in register 0 goes on the LEDs
  LATC = HoldingRegister[0]>>8 & 0x07;
  LATE = HoldingRegister[0]>>11 & 0x07;
  PORTBbits.RB0=HoldingRegister[0]>>14;
  PORTBbits.RB1=HoldingRegister[0]>>15;
  PORTBbits.RB6=Coils[0];  //set bit depending on coil state
}
void ProgramLoop(void)
{
  //Enter what your program here.  The following is just an example
  PORTDbits.RD0 = 1;   //blinks some leds to show it's working
  long_delay_ms(250);
  PORTDbits.RD0 = 0;
  long_delay_ms(250);
}