/******************************************************************************/
/* Modbus Level #define Macros                                                  */
/******************************************************************************/

/******************************************************************************/
/* Modbus Function Prototypes                                                   */
/******************************************************************************/
unsigned char CheckCRC(void);
void ClearResponse(void);
void ReadRegister(void);         /* I/O and Peripheral Initialization */
void ReadInputRegister(void);
void WriteRegister(void);
void WriteMultipleRegisters(void);
void ReadCoil(void);
void ReadInputCoil(void);
void WriteCoil(void);
void WriteMultipleCoils(void);
void ModbusDelay(void);              /* Writes to Timer0 for 1.04ms delay*/
void DecodeIt(void);
unsigned int GenerateCRC(unsigned char);
