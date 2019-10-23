#ifndef SerialIocpClient_h
#define SerialIocpClient_h

#include "Arduino.h"

#define COMMAND_NONE  0
#define COMMAND_READY 1
#define COMMAND_QUIT  2

class SerialIocpClient
{
public:
	SerialIocpClient()
	{
		_CommandFuncitonPointer = NULL;
		_ValueFuncitonPointer = NULL;
	}
	void Open(long nBaudRate = 115200)
	{
		Serial.begin(nBaudRate);
		Serial.setTimeout(50); 
		inputString.reserve(200);
		inputString = "";
	}
	void GetLine()
	{
		while (Serial.available())
		{
			// get the new byte:
			char inChar = (char)Serial.read();
			// add it to the inputString:
			// if the incoming character is a newline, set a flag so the main loop can
			// do something about it:
			if (inChar == '\n')
			{
				if (inputString == "RDY")
				{
					if (_CommandFuncitonPointer)
						_CommandFuncitonPointer(COMMAND_READY);
				}
				else
					if (inputString == "QIT")
					{
						if (_CommandFuncitonPointer)
							_CommandFuncitonPointer(COMMAND_QUIT);
					}
					else
					{
						if (_ValueFuncitonPointer)
						{
							int nPos = inputString.indexOf('=');
							if (nPos>0)
							{
								String sId = inputString.substring(0, nPos);
								long nId = sId.toDouble();
								String sVal = inputString.substring(nPos + 1);
								long nVal = sVal.toDouble();
								_ValueFuncitonPointer(nId, nVal);
							}
						}
					}
				inputString = "";
			}
			else
			{
				if (inChar >= 32)
					inputString += inChar;
			}
		}
	}
	void SetOffset(long nId, long nVal)
	{
		String sSend(nId);
		sSend += "=";
		sSend += String(nVal);
		sSend += ":";

		Serial.print(sSend);
	}
	void SendKey(long nId, long nVal)
	{
		String sSend(nId);
		sSend += ">";
		sSend += String(nVal);
		sSend += ":";

		Serial.print(sSend);
	}
private:
	typedef void(*voidFuncCom)(int);
	typedef void(*voidFuncVal)(long, long);
	voidFuncCom  _CommandFuncitonPointer;
	voidFuncVal  _ValueFuncitonPointer;
	String inputString;

public:
	void SetOnCommand(voidFuncCom OnCommandFunctionPointer)
	{
		_CommandFuncitonPointer = OnCommandFunctionPointer;
	}
	void SetOnValue(voidFuncVal OnValueFunctionPointer)
	{
		_ValueFuncitonPointer = OnValueFunctionPointer;
	}
};

SerialIocpClient iocp;
void serialEvent()
{
	iocp.GetLine();
}

#endif
