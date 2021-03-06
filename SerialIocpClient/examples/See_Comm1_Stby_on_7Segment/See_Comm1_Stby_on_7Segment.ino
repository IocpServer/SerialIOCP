/*
*	IocpServer Example
*   See COM1 STBY FRQ on a MAX7219 7 segments Display
*	by Juan R. Ascanio *
*
*/

#include <SerialIocpClient.h>

/////////////////////////////////////////////////
// OFFSETS DEFINITION GENERATED BY SerialIocp
/////////////////////////////////////////////////
#define COM_FREQUENCY   21 // Codification bcd16
#define COM_STBY_FREQUENCY  615 // Codification bcd16

#include "LedControl.h"
/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 9 is connected to the DataIn 
 pin 8 is connected to the CLK 
 pin 7 is connected to LOAD 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(9,8,7,1);

//////////////////////////////////////////////////////////////////////////////////////////////
// OnIocpCommand called by Iocp Librarie, as declared as callback on SetUp function, when a
// command is recieved:
// Params: 
//    int nCommand, can be:
//      COMMAND_READY: When SerialIocp on the PC is connected to the Simulator
//      COMMAND_QUIT:  When SerialIocp on the PC is dis-connected from the Simulator
//////////////////////////////////////////////////////////////////////////////////////////////
void OnIocpCommand(int nCommand)
{
	switch (nCommand)
	{
	case COMMAND_READY:
	{
		// Code executed When SerialIocp on the PC is connected to the Simulator
		break;
	}
	case COMMAND_QUIT:
	{
		// Code executed When SerialIocp on the PC is dis-connected from the Simulator
    lc.clearDisplay(0);
		break;
	}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// OnIocpValue called by Iocp Librarie, as declared as callback on SetUp function,
// when any offset valus is changed on the Simulator recieved:
// Params: 
//    long nId, is the offset ID, to compare with the #define generated by SerialIocp or with 
//              the OffSet ID numbre.
//    long nVal, is the new value that OFFSET nId has in the Simulator.
//////////////////////////////////////////////////////////////////////////////////////////////
void OnIocpValue(long nId, long nVal)
{
	// Each time any Registered Offset Value has changed on the Simulator, the IocpSerial libarie call this function.
	if (nId == COM_STBY_FREQUENCY)
  {
      printNumber(nVal);
  }
	if (nId == COM_FREQUENCY)
  {
 //   printNumber(nVal);
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////
byte digits[10] = {B01111110,B00110000,B01101101,B01111001,B00110011,B01011011,B01011111,B01110000,B01111111,B01111011};
byte point = B10000000;
void printNumber(int v) 
{
    int d1;
    int d2;
    int d3;
    int d4;
    int d5;

    d1=v%10;
    v=v/10;
    d2=v%10;
    v=v/10;
    d3=v%10;     
    v=v/10;
    d4=v%10;     
    lc.setChar(0,4,'1',false);
    //Now print the number digit by digit
    lc.setRow(0,3,(byte)digits[d4]);
    lc.setRow(0,2,(byte)digits[d3]+point); // Plus decimal point
    lc.setRow(0,1,(byte)digits[d2]);
    lc.setRow(0,0,(byte)digits[d1]);
}
/////////////////////////////////////////////////////////////////////////////////////////////
// Standard Arduino setup() function
/////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);
   
	// Iocp Initialize
	iocp.Open(2000000);
	iocp.SetOnCommand(&OnIocpCommand);
	iocp.SetOnValue(&OnIocpValue);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Standard Arduino loop() function
/////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  // Include code you want, nd remove delay if necesary
  delay(25);
}
