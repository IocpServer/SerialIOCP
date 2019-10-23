/*
*  IocpServer Example
*   Use the MD_MAX72XX library to scroll text on the display depending on Prepar Plane Status
*
* by Juan R. Ascanio *
*
*/

#include <SerialIocpClient.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 11

#define CLK_PIN   13  // or SCK
#define DATA_PIN  11  // or MOSI
#define CS_PIN    10  // or SS

/////////////////////////////////////////////////
// OFFSETS DEFINITION GENERATED BY OffsetExplorer
/////////////////////////////////////////////////
#define AIRCRAFT_ON_GROUND   58 // Codification none
#define AIRSPEED  59 // Codification none

// SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#define SCROLL_DELAY  75  // in milliseconds

// Global message buffers shared by Serial and Scrolling functions
#define BUF_SIZE  75
char curMessage[BUF_SIZE];
char newMessage[BUF_SIZE];
bool newMessageAvailable = false;

uint16_t  scrollDelay;  // in milliseconds

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
        strcpy(newMessage, "    ");
        break;
      }
      case COMMAND_QUIT:
      {
        // Code executed When SerialIocp on the PC is dis-connected from the Simulator
        strcpy(newMessage, "     ");
        break;
      }
  }
  newMessageAvailable = true;
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
  if (nId == AIRCRAFT_ON_GROUND)
  {
    if (nVal!=0)
    {
        strcpy(newMessage, "Plane on Ground.   ");
    }
    else
    {
        strcpy(newMessage, "Plane on Air.   ");
    }
    newMessageAvailable = true;
  }
}

// Callback function for data that is being scrolled off the display
void scrollDataSink(uint8_t dev, MD_MAX72XX::transformType_t t, uint8_t col)
{
}

// Callback function for data that is required for scrolling into the display
uint8_t scrollDataSource(uint8_t dev, MD_MAX72XX::transformType_t t)
{
  static char   *p = curMessage;
  static uint8_t  state = 0;
  static uint8_t  curLen, showLen;
  static uint8_t  cBuf[8];
  uint8_t colData;

  // finite state machine to control what we do on the callback
  switch(state)
  {
    case 0: // Load the next character from the font table
      showLen = mx.getChar(*p++, sizeof(cBuf)/sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state++;

      // if we reached end of message, reset the message pointer
      if (*p == '\0')
      {
        p = curMessage;     // reset the pointer to start of message
        if (newMessageAvailable)  // there is a new message waiting
        {
          strcpy(curMessage, newMessage);	// copy it in
          newMessageAvailable = false;
        }
      }
      // !! deliberately fall through to next state to start displaying

    case 1: // display the next part of the character
      colData = cBuf[curLen++];
      if (curLen == showLen)
      {
        showLen = 0;//CHAR_SPACING;
        curLen = 0;
        state = 2;
      }
      break;

    case 2: // display inter-character spacing (blank column)
      colData = 0;
      if (curLen == showLen)
        state = 0;
      curLen++;
      break;

    default:
      state = 0;
  }

  return(colData);
}

 void scrollText(void)
{
  static uint32_t	prevTime = 0;

  // Is it time to scroll the text?
  if (millis()-prevTime >= scrollDelay)
  {
    mx.transform(MD_MAX72XX::TSL);  // scroll along - the callback will load all the data
    prevTime = millis();      // starting point for next time
  }
}

void setup()
{
  // Iocp Initialize
  iocp.Open(2000000);
  iocp.SetOnCommand(&OnIocpCommand);
  iocp.SetOnValue(&OnIocpValue);
  
  mx.begin();
  mx.setShiftDataInCallback(scrollDataSource);
  mx.setShiftDataOutCallback(scrollDataSink);

  scrollDelay = SCROLL_DELAY;
  strcpy(curMessage, "      ");
  newMessage[0] = '\0';
}

void loop()
{
  scrollText();
}
