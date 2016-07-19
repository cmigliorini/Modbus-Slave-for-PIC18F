/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define _XTAL_FREQ SYS_FREQ  //used for __delay
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void long_delay_ms (unsigned int ms);  //sample user function
void InitPorts (void);
void Initialize (void);
void ModbusEvents (void);
void ProgramLoop(void);
