/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/

/* TODO Change all these values for your project */

/* Microcontroller MIPs (FCY) */
#define p18f4520    //either write p18f4520, p18f8722, or P18f458 or your own if added.
#define SYS_FREQ         20000000L  //10MHz & 20MHz confirmed working.. have a go
#define FCY              SYS_FREQ/4

//  The following sets timer0 for a 1.04mS delay
//  Timer0 is preset then counts up until it overflows
//  To Calculate:
//  0xFFFF - Hex(Clock Frequency/4 * 0.00104)
#define Timer0High       0xEB // 4MHz:0xFB, 8MHz:0xF7, 10MHz:0xF5, 20MHz:0xEB
#define Timer0Low        0xAF // 4MHz:0xEF, 8MHz:0xDF, 10MHz:0xD7, 20MHz:0xAF

#define BAUD_RATE        9600
#define SlaveAddress     2 // Change this to whatever you want..

//CHANGE THESE FOR EACH PROCESSOR
#ifdef p18f458
  #define BusyUsart        BusyUSART()       //For 18F458 this is BusyUSART()
  #define TransmitBuffer   TXREG             //For 18F8458 this is TXREG
  #define ReceiveBuffer    RCREG             //For 18F8458 this is RXREG
  #define WriteEnable      LATAbits.LATA0    //this is RO/RE
  #define writeEnConf      TRISAbits.TRISA0  //to config RO/RE as output
  #define ReceiveFlag1     PIR1bits.RCIF
  #define TransmitFlag1    PIR1bits.TXIF
  #define Timer0Flag       INTCONbits.TMR0IF
#endif
#ifdef p18f4520
  #define BusyUsart        BusyUSART()       //For 18F4520 this is BusyUSART()
  #define TransmitBuffer   TXREG             //For 18F4520 this is TXREG
  #define ReceiveBuffer    RCREG             //For 18F4520 this is RXREG
  #define WriteEnable      LATCbits.LATC1    //this is RO/RE
  #define writeEnConf      TRISCbits.TRISC1  //to config RO/RE as output
  #define ReceiveFlag1     PIR1bits.RCIF
  #define TransmitFlag1    PIR1bits.TXIF
  #define Timer0Flag       INTCONbits.TMR0IF
#endif
#ifdef p18f8722
  #define busyUsart        Busy1USART()      //For 18F8722 this is Busy1USART()
  #define TransmitBuffer   TXREG1            //For 18F8722 this is TXREG1
  #define ReceiveBuffer    RCREG1            //For 18F8722 this is RXREG1
  #define WriteEnable      LATCbits.LATC1    //this is RO/RE
  #define writeEnConf      TRISCbits.TRISC1  //to config RO/RE as output
  #define ReceiveFlag1     PIR1bits.RCIF
  #define TransmitFlag1    PIR1bits.TXIF
  #define Timer0Flag       INTCONbits.TMR0IF
#endif
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

void ConfigInterrupts(void);

void interrupt isr(void);

void ClsUSART(void);
void OpnUSART(void);

void OpenTmr0(void);

