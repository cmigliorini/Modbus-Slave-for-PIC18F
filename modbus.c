/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
    #include <usart.h>
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
    #include <usart.h>
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "modbus.h"
#include "system.h"

#define CHECK_BIT(var,pos) !!((var) & (1 << (pos)))

/******************************************************************************/
/* Global variables                                                           */
/******************************************************************************/
extern volatile unsigned int  HoldingRegister[50];
extern volatile unsigned int  InputRegister[50];
extern volatile unsigned char Coils[50];
extern volatile unsigned char InputBits[50];
extern volatile unsigned char Response[125]; //Enough to return all holding-r's
extern volatile unsigned char Received[125]; //Enough to write all holding-r's
extern volatile char ModbusMessage,MessageLength;
/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/
void ModbusDelay(void)
{
  /* Writes to Timer0 for 1.04ms delay*/
  TMR0H = Timer0High;
  TMR0L = Timer0Low;
}

void ClearResponse(void)
{
  unsigned char i;
  for(i=0;i<125;i++){ //Response is 125 long
    Response[i] = 0;
  }
}

void DecodeIt(void)
{
  if(Received[0] == SlaveAddress){
    if(CheckCRC()){
      if(Received[1] == 0x01){
        ReadCoil();
      }
      else if(Received[1] == 0x02){
        ReadInputCoil();
      }
      else if(Received[1] == 0x03){
        ReadRegister();
      }
      else if(Received[1] == 0x04){
        ReadInputRegister();
      }
      else if(Received[1] == 0x05){
        WriteCoil();
      }	  
      else if(Received[1] == 0x06){
        WriteRegister();
      }
      else if(Received[1] == 0x10){
        WriteMultipleRegisters();
      }
      else if(Received[1] == 0x0F){
        WriteMultipleCoils();
      }
      else{
        Response[0] = 0; //error this does nothing though..
      }
    }
  }
  ModbusMessage = 0;
}

void ReadRegister(void)
{
  unsigned int rr_Address = 0;
  unsigned int rr_numRegs = 0;
  unsigned char j = 3;
  unsigned int crc = 0;
  unsigned int i = 0;

  //Combine address bytes
  rr_Address = Received[2];
  rr_Address <<= 8;
  rr_Address |= Received[3];

  //Combine number of regs bytes
  rr_numRegs = Received[4];
  rr_numRegs <<= 8;
  rr_numRegs |= Received[5];

  Response[0] = SlaveAddress;
  Response[1] = 0x03;
  Response[2] = rr_numRegs*2; //2 bytes per reg

  for(i=rr_Address;i<(rr_Address + rr_numRegs);i++){
    if(HoldingRegister[i] > 255){
      //Need to split it up into 2 bytes
      Response[j] = HoldingRegister[i] >> 8;
      j++;
      Response[j] = HoldingRegister[i];
      j++;
    }
    else{
      Response[j] = 0x00;
      j++;
      Response[j] = HoldingRegister[i];
      j++;
    }
  }
  crc = GenerateCRC(j+2);
  Response[j] = crc >> 8;
  j++;
  Response[j] = crc;
  j+=2;

  WriteEnable = 1;
  for(i=0;i!=j;i++){
   while(BusyUsart);
     TransmitBuffer = Response[i];
  }
  WriteEnable = 0;
  j=0;

  ClearResponse();
}

void ReadInputRegister(void)
{
  unsigned int rr_Address = 0;
  unsigned int rr_numRegs = 0;
  unsigned char j = 3;
  unsigned int crc = 0;
  unsigned int i = 0;

  //Combine address bytes
  rr_Address = Received[2];
  rr_Address <<= 8;
  rr_Address |= Received[3];

  //Combine number of regs bytes
  rr_numRegs = Received[4];
  rr_numRegs <<= 8;
  rr_numRegs |= Received[5];

  Response[0] = SlaveAddress;
  Response[1] = 0x04;
  Response[2] = rr_numRegs*2; //2 bytes per reg

  for(i=rr_Address;i<(rr_Address + rr_numRegs);i++){
    if(InputRegister[i] > 255){
      //Need to split it up into 2 bytes
      Response[j] = InputRegister[i] >> 8;
      j++;
      Response[j] = InputRegister[i];
      j++;
    }
    else{
      Response[j] = 0x00;
      j++;
      Response[j] = InputRegister[i];
      j++;
    }
  }
  crc = GenerateCRC(j+2);
  Response[j] = crc >> 8;
  j++;
  Response[j] = crc;
  j+=2;

  WriteEnable = 1;
  for(i=0;i!=j;i++){
   while(BusyUsart);
     TransmitBuffer = Response[i];
  }
  WriteEnable = 0;
  j=0;

  ClearResponse();
}

void WriteRegister(void)
{
/******************************************************************************/
/* Works out which reg to write and then responds                             */
/******************************************************************************/
  unsigned int wr_AddressLow = 0;
  unsigned int wr_AddressHigh = 0;
  unsigned int wr_Address = 0;

  unsigned int wr_ValueToWrite = 0;
  unsigned int wr_ValueToWriteLow = 0;
  unsigned int wr_ValueToWriteHigh = 0;

  unsigned int crc = 0;
  unsigned int i = 0;

  //Combine address bytes
  wr_Address = Received[2];
  wr_Address <<= 8;
  wr_Address |= Received[3];

  wr_AddressLow = Received[3]; //useful to store
  wr_AddressHigh = Received[2];

  //Combine value to write regs
  wr_ValueToWrite = Received[4];
  wr_ValueToWrite <<= 8;
  wr_ValueToWrite |= Received[5];

  wr_ValueToWriteLow = Received[5];
  wr_ValueToWriteHigh = Received[4];

  HoldingRegister[wr_Address] = wr_ValueToWrite;

  Response[0] = SlaveAddress;
  Response[1] = 0x06;
  Response[3] = wr_AddressLow; //2 bytes per reg
  Response[2] = wr_AddressHigh;

//TO DO CHECK VALUE IS ACTUALLY WRITTEN//
  Response[4] = wr_ValueToWriteHigh;
  Response[5] = wr_ValueToWriteLow;

  crc = GenerateCRC(8);

  Response[6] = crc >> 8;
  Response[7] = crc;

  WriteEnable = 1;
  for(i=0;i!=9;i++){
   while(BusyUsart);
     TransmitBuffer = Response[i];
  }
  WriteEnable = 0;
  ClearResponse();
}

void WriteMultipleRegisters(void)
{
/******************************************************************************/
/* Works out which reg to write and then responds                             */
/******************************************************************************/
  unsigned int wmr_Address = 0;
  unsigned int wmr_AddressHigh = 0;
  unsigned int wmr_AddressLow = 0;
  unsigned int wmr_numRegs = 0;
  unsigned int wmr_numRegsHigh = 0;
  unsigned int wmr_numRegsLow = 0;
  unsigned int wmr_numBytes = 0;
  unsigned int wmr_numBytesTST = 0;
  unsigned int ValueToWrite = 0;
  unsigned int ValueToWriteHigh = 0;
  unsigned int ValueToWriteLow = 0;

  unsigned char j = 0;
  unsigned int crc = 0;
  unsigned int i = 0;

  //Combine address bytes
  wmr_Address = Received[2];
  wmr_Address <<= 8;
  //add address high and low bits
  wmr_Address |= Received[3];
  wmr_AddressHigh = Received[2];
  wmr_AddressLow = Received[3];

  //Combine number of regs bytes
  wmr_numRegs = Received[4];
  wmr_numRegs <<= 8;
  wmr_numRegs |= Received[5];
  wmr_numRegsHigh = Received[4];
  wmr_numRegsLow = Received[5];
  wmr_numBytes = Received[6];

  j = 7;

  //the following lines haven't been tested yet but were suggested
  //in a pull request, If you have any problems revert back to wmr_numBytes 
  //in the for loop from TST (the lines commented below)

  wmr_numBytesTST = wmr_numBytes /2;   

  for(i=0;i<wmr_numBytesTST;i++)
  {
    ValueToWrite = Received[j];
    ValueToWrite <<= 8;
    j++;
    ValueToWrite |= Received[j];
    j++;

    HoldingRegister[wmr_Address + i] = ValueToWrite;
  }

  //for(i=0;i<wmr_numBytes;i++)
  //{
  // ValueToWrite = Received[j];
  //  ValueToWrite <<= 8;
  //  j++;
  //  ValueToWrite |= Received[j];
  //  j++;

  //  HoldingRegister[wmr_Address + i] = ValueToWrite;
  //}


  Response[0] = SlaveAddress;
  Response[1] = 0x10;
  Response[2] = wmr_AddressHigh;
  Response[3] = wmr_AddressLow;
  Response[4] = wmr_numRegsHigh;
  Response[5] = wmr_numRegsLow;

  crc = GenerateCRC(8);

  Response[6] = crc >> 8;
  Response[7] = crc;

  WriteEnable = 1;
  for(i=0;i!=9;i++){
   while(BusyUsart);
     TransmitBuffer = Response[i];
  }
  WriteEnable = 0;
  j=0;

  ClearResponse();
  
}


void ReadCoil(void)
{
/******************************************************************************/
/* Reads a coil and then responds                                             */
/******************************************************************************/
  unsigned int rc_Address = 0;
  unsigned int rc_numCoils = 0;
  unsigned int crc = 0;

  unsigned char HowManyBytes = 0;
  unsigned char Remainder = 0;
  unsigned char lsb = 0;
  unsigned char i,j,k,l = 0;

  //Combine address bytes
  rc_Address = Received[2];
  rc_Address <<=8;
  rc_Address |= Received[3];

  //Combine number of Coils bytes
  rc_numCoils = Received[4];
  rc_numCoils <<= 8;
  rc_numCoils |= Received[5];

  Response[0] = SlaveAddress;
  Response[1] = 0x01;

  HowManyBytes = rc_numCoils/8;
  Remainder = rc_numCoils % 8;

  if(Remainder){
    HowManyBytes += 1;
  }
  Response[2] = HowManyBytes;

  l = rc_Address;
  k = 3; //start at Response 3

  for(i=HowManyBytes; i!=0; i--){
    if(i>1){
      for(j=0;j!=8;j++){
    if(Coils[l]){
          lsb = 1;
    }
    else{
          lsb = 0;
    }
    Response[k] ^= (lsb << j);
    l++;
      }
      k++;
    }
    else{
      for(j=0;j!=Remainder;j++){
      if(Coils[l]){
        lsb = 1;
      }
      else{
        lsb = 0;
      }
      Response[k] ^= (lsb << j);
      l++;
      }
      k++;
    }
  }
  crc = GenerateCRC(k+2);

  Response[k] = crc >> 8;
  Response[k+1] = crc;

  WriteEnable = 1;
  for(i=0;i!=(k+3);i++){
   while(BusyUsart);
     TransmitBuffer = Response[i];
  }
  WriteEnable = 0;
  ClearResponse();
}

void ReadInputCoil(void)
{
/******************************************************************************/
/* Reads a coil and then responds                                             */
/******************************************************************************/
  unsigned int rc_Address = 0;
  unsigned int rc_numCoils = 0;
  unsigned int crc = 0;

  unsigned char HowManyBytes = 0;
  unsigned char Remainder = 0;
  unsigned char lsb = 0;
  unsigned char i,j,k,l = 0;

  //Combine address bytes
  rc_Address = Received[2];
  rc_Address <<=8;
  rc_Address |= Received[3];

  //Combine number of Coils bytes
  rc_numCoils = Received[4];
  rc_numCoils <<= 8;
  rc_numCoils |= Received[5];

  Response[0] = SlaveAddress;
  Response[1] = 0x02;

  HowManyBytes = rc_numCoils/8;
  Remainder = rc_numCoils % 8;

  if(Remainder){
    HowManyBytes += 1;
  }
  Response[2] = HowManyBytes;

  l = rc_Address;
  k = 3; //start at Response 3

  for(i=HowManyBytes; i!=0; i--){
    if(i>1){
      for(j=0;j!=8;j++){
        if(InputBits[l]){
          lsb = 1;
        }
        else{
          lsb = 0;
        }
        Response[k] ^= (lsb << j);
        l++;
      }
      k++;
    }
    else{
      for(j=0;j!=Remainder;j++){
        if(InputBits[l]){
          lsb = 1;
        }
        else{
          lsb = 0;
        }
        Response[k] ^= (lsb << j);
        l++;
      }
      k++;
    }
  }
  crc = GenerateCRC(k+2);

  Response[k] = crc >> 8;
  Response[k+1] = crc;

  WriteEnable = 1;
  for(i=0;i!=(k+3);i++){
   while(BusyUsart);
     TransmitBuffer = Response[i];
  }
  WriteEnable = 0;
  ClearResponse();
}

void WriteCoil(void)
{
/******************************************************************************/
/* Writes to a coil and then responds                                         */
/******************************************************************************/
  unsigned int wc_AddressLow = 0;
  unsigned int wc_AddressHigh = 0;
  unsigned int wc_Address = 0;

  unsigned int wc_ValueToWrite = 0;
  unsigned int wc_ValueToWriteLow = 0;
  unsigned int wc_ValueToWriteHigh = 0;
  int i = 0;
  unsigned int crc = 0;

  //Combine address bytes
  wc_Address = Received[2];
  wc_Address <<= 8;
  wc_Address |= Received[3];

  wc_AddressLow = Received[3]; //useful to store
  wc_AddressHigh = Received[2];

  //Combine value to write regs
  wc_ValueToWrite = Received[4];
  wc_ValueToWrite <<= 8;
  wc_ValueToWrite |= Received[5];

  wc_ValueToWriteLow = Received[5];
  wc_ValueToWriteHigh = Received[4];

  if(wc_ValueToWrite){
    Coils[wc_Address] = 0xFF;
  }
  else{
    Coils[wc_Address] = 0x00;
  }

  Response[0] = SlaveAddress;
  Response[1] = 0x02;
  Response[3] = wc_AddressLow; //2 bytes per reg
  Response[2] = wc_AddressHigh;

//TO DO CHECK VALUE IS ACTUALLY WRITTEN//
  Response[4] = wc_ValueToWriteHigh;
  Response[5] = wc_ValueToWriteLow;

  crc = GenerateCRC(8);

  Response[6] = crc >> 8;
  Response[7] = crc;

  WriteEnable = 1;
  for(i=0;i!=9;i++){
   while(BusyUsart);
     TransmitBuffer = Response[i];
  }
  WriteEnable = 0;
  ClearResponse();
}

void WriteMultipleCoils(void)
{
/******************************************************************************/
/* Reads a coil and then responds                                             */
/******************************************************************************/
  unsigned int wmc_Address = 0;
  unsigned int wmc_AddressHigh = 0;
  unsigned int wmc_AddressLow = 0;
  unsigned int wmc_numCoils = 0;
  unsigned int wmc_numCoilsHigh = 0;
  unsigned int wmc_numCoilsLow = 0;
  unsigned int wmc_numBytes = 0;
  unsigned int crc = 0;

  unsigned char HowManyBytes = 0;
  unsigned char Remainder = 0;
  unsigned char lsb = 0;
  unsigned char i,j,k,l = 0;

  //Combine address bytes
  wmc_Address = Received[2];
  wmc_AddressHigh = Received[2];
  wmc_Address <<=8;
  wmc_Address |= Received[3];
  wmc_AddressLow = Received[3];

  //Combine number of Coils bytes
  wmc_numCoils = Received[4];
  wmc_numCoilsHigh = Received[4];
  wmc_numCoils <<= 8;
  wmc_numCoils |= Received[5];
  wmc_numCoilsLow = Received[5];

  wmc_numBytes = Received[6];

  Response[0] = SlaveAddress;
  Response[1] = 0x0F;

  HowManyBytes = wmc_numCoils/8;
  Remainder = wmc_numCoils % 8;

  if(Remainder){
    HowManyBytes += 1;
  }
  Response[2] = wmc_AddressHigh;
  Response[3] = wmc_AddressLow;

  Response[4] = wmc_numCoilsHigh;
  Response[5] = wmc_numCoilsLow;

  l = wmc_Address;
  k = 3; //start at Response 3

  unsigned char bitSet;
  unsigned char ValueToWrite;
  unsigned char q = 7; //count through vals to write
  

  for(i=HowManyBytes; i!=0; i--){
    ValueToWrite = Received[q];
    q++;
    if(i>1){
      for(j=0;j!=8;j++){
        if(CHECK_BIT(ValueToWrite, j)){
          Coils[l] = 1;
        }
        else{
          Coils[l] = 0; //need to sort out Remainder stuff
        }
        l++;
      }
    }
    else{
      for(j=0;j!=Remainder;j++){
      if(CHECK_BIT(ValueToWrite, j)){
          Coils[l] = 1;
      }
      else{
        Coils[l] = 0;
      }
      l++;
      }
    }
  }
  crc = GenerateCRC(8);

  Response[6] = crc >> 8;
  Response[7] = crc;

  WriteEnable = 1;
  for(i=0;i!=9;i++){
   while(BusyUsart);
     TransmitBuffer = Response[i];
  }
  WriteEnable = 0;
  ClearResponse();
}


unsigned int GenerateCRC(unsigned char messageLength)
{
unsigned int crc = 0xFFFF;
unsigned int crcHigh = 0;
unsigned int crcLow = 0;
int i,j = 0;

  for(i=0;i<messageLength-2;i++){
    crc ^= Response[i];
    for(j=8; j!=0; j--){
      if((crc & 0x0001) != 0){
        crc >>= 1;
        crc ^= 0xA001;
      }
      else{
        crc >>= 1;
      }
    }
  }
//bytes are wrong way round so doing a swap here..
crcHigh = (crc & 0x00FF) <<8;
crcLow = (crc & 0xFF00) >>8;
crcHigh |= crcLow;
crc = crcHigh;
return crc;
}

unsigned char CheckCRC(void)
{
  unsigned int crc = 0xFFFF;
  unsigned int crcHigh = 0;
  unsigned int crcLow = 0;
  int i,j = 0;

    for(i=0;i<MessageLength-2;i++){
      crc ^= Received[i];
      for(j=8; j!=0; j--){
        if((crc & 0x0001) != 0){
          crc >>= 1;
          crc ^= 0xA001;
        }
        else{
          crc >>= 1;
        }
      }
    }
  //bytes are wrong way round so doing a swap here..
  crcHigh = (crc & 0x00FF);
  crcLow = (crc & 0xFF00) >>8;
  if((crcHigh == Received[i])&&(crcLow == Received[i+1]))
  {
    return 1;
  }
  else{
    return 0;
  }
}