/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/

#include <p18cxxx.h>    /* C18 General Include File */
#include "user.h"
#include "modbus.h"
#include "system.h"

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
volatile char EndOfMessage,NewMessage = 1;
volatile char TimerCount,MessageLength,ModbusMessage,z = 0;
volatile unsigned char Response[125]; //Enough to return all holding-r's
volatile unsigned char Received[125]; //Enough to write all holding-r's 

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

// start ISR code
//if using C18
//#pragma code isr = 0x08 // store the below code at address 0x08
//#pragma interrupt isr   // let the compiler know that the function isr() is int

void interrupt isr(void)
{
  if(ReceiveFlag1){        // USART receive interrupt flag has been set
    if((!EndOfMessage)&&(!NewMessage)){
      if(TransmitFlag1){   // check if the TXREG is empty
        Received[z] = ReceiveBuffer;
        z++;
        TimerCount = 0;
      }
    }
    if(NewMessage){
      OpenTmr0();
      if(TransmitFlag1){     // check if the TXREG is empty
        Received[z] = ReceiveBuffer;
        z++;
        NewMessage = 0;
        EndOfMessage = 0;
        MessageLength = 0;
        ModbusMessage = 0;
        TimerCount = 0;
        return;
      }
    }
  }
  else if(Timer0Flag){     //TMR0 interrupt flag
    ModbusDelay();         //Resets timer for 1.04ms
    TimerCount++;
    if(TimerCount > 4){
      EndOfMessage = 1;
      NewMessage = 1;
      MessageLength = z;
      ModbusMessage = 1;
      for(z=(MessageLength);z!=125;z++){ //Clear rest of message
        Received[z] = 0;
      }
      z=0;
      T0CONbits.TMR0ON = 0;  //Close timer0
      TimerCount = 0;
    }
   Timer0Flag = 0;           // Clear flag
  }
}