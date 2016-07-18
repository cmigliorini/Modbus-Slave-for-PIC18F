/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
    #include <usart.h>
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
    #include <usart.h>
#endif

#if defined(__XC) || defined(HI_TECH_C)
    #include <stdint.h>        /* For uint8_t definition */
    #include <stdbool.h>       /* For true/false definition */
#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "modbus.h"

/******************************************************************************/
/* Modbus Global Variable Declaration                                         */
/******************************************************************************/
volatile unsigned int  HoldingRegister[50] = {0};
volatile unsigned char Coils[50]= {0};
extern volatile char ModbusMessage;

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/
void main(void)
{
  OpnUSART();
  ConfigInterrupts();
  
  //The InitPorts procedute initializes your ports
  InitPorts();   //configure your ports in user.c under InitPorts
  
  //The Iitiaize procedure is called before the program begins
  //commands here will only run once at program start
  Initialize();    //enter you initialization code in user.c under Initialize
  
  //main program loop
  while(1){
    if(ModbusMessage){
      DecodeIt();
      //The modbusEvents procedure is called when a Modbus message is received
      ModbusEvents();           //enter your code in user.c under ModbusEvents
    }
    //The ProgramLoop procedure will run in an endless loop forever
    ProgramLoop();   //enter your code in user.c under ProgramLoop
  }
}