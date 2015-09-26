/*
 * XBee Communications Program
 *
 * Author:	Jan Louis Evangelista
 *			UBCST Electrical Division
 *
 * Date:	September 9, 2015
 *
 * Purpose: To transmit data using the XBee module.
 *			The program initializes the serial port where
 *			the XBee is connected, and uses the serial 
 *			port to send and receive data to and from the
 *			XBee module.
 *			For test purposes, the program has a function
 *			that can send out [10...50] data packets, which
 *			can be counted by the XBee Receiver program.
 *
 * Note:	**THIS PROGRAM IS FOR WINDOWS SYSTEMS ONLY!**
 *			Serial communications is handled by Win32
 *			routines. Serial port functions will have to
 *			be added for Linux use (ie. Raspberry Pi.)
 *			*Linux serial comm functions to be added later,
 *			when the XBee is fully tested.
 */

#include <iostream>		// Standard IO library
#include <string>		// Strings library
#include <Windows.h>	// Windows API library
#include <tchar.h>		// For handling text encodings
#include "Serial.h"		// Serial library

using namespace std;

/* Macros */
#define _XBEEPORT "COM3"

/* Global Variables */
CSerial xbeeSerial;	/* New XBee serial class */ 

/* Function Declarations */
void startup();
void init_Xbee_Serial();
int promptMenu();
void configureXbee();
void receiveData();
void sendData(LPCSTR );

int main(void)
{
	// Local Variables
	string message("hello!");
	LPCSTR msgString = message.c_str();
	// Initialize the program 
	startup();

	if(promptMenu())	// Check if user wants to SEND (1) or EXIT (0)
	{
		// Send and receive data code goes here
		cout << message << "\n";
		sendData(msgString);
	}


	// Exit the program 
	xbeeSerial.Close(); // Close the serial port before exiting
	cin.get(); // Require a key press before quitting
	return 0;
}

/*
 * startup()
 *
 * Inputs:	None
 *
 * Outputs:	None
 *
 * Purpose:	This function initializes the program.
 *			This will print a display message, initialize
 *			the serial port and create the XBee serial handler.
 */
void startup(void)
{
	/* Display title bar */
	cout << "*************************************\n";
	cout << "*                                   *\n";
	cout << "*    XBee Communications Program    *\n";
	cout << "*         UBCST Electrical          *\n";
	cout << "*                                   *\n";
	cout << "*************************************\n\n";

	/* Initialize the XBee serial port */
	init_Xbee_Serial();
}

/*
 * init_Xbee_Serial()
 *
 * Inputs:	None
 *
 * Outputs:	None
 *
 * Purpose:	** THIS INITIALIZES THE SERIAL PORT ON WINDOWS ONLY **
 *			** USE A DIFFERENT FUNCTION FOR LINUX INITIALIZATION **
 *			This function initializes the serial port
 *			to be used by the XBee.
 *			This function uses the following resource for
 *			reference: https://msdn.microsoft.com/en-us/library/ff802693.aspx
 */
void init_Xbee_Serial(void)
{
	// Initialize the XBee serial port
	xbeeSerial.Open(_T(_XBEEPORT));

	// Setup the serial port to the defined values
	xbeeSerial.Setup(
		CSerial::EBaud9600, // Set to 9600 bps
		CSerial::EData8, // Set to 8 data bits
		CSerial::EParNone, // No Parity bit
		CSerial::EStop1 // No Stop bit
		);

	// Setup serial handshaking mode
	xbeeSerial.SetupHandshaking(CSerial::EHandshakeHardware);
}

/*
 * promptMenu()
 *
 * Inputs:	None
 *
 * Outputs:	1 - send data
 *			0 - exit the program
 *
 * Purpose: Prompts the user whether to send data or
 *			exit the program.
 */
int promptMenu(void)
{
	string response;	/* string to hold user response */

	/* Prompt the user for input */
	/* Any invalid responses are ignored */
	do
	{
		cout << "Type in \"send\" to start sending data\n";
		cout << "or type in \"exit\" to exit the program: ";
		cin >> response;
	} while((response != "send") && (response != "exit"));	/* Check for invalid inputs */
	
	/* Return a value to main() based on whether */
	/* response is "send" or "exit" */
	cout << "\n";
	if(response == "send")
		return 1;
	else
		return 0;
}

/*
 * configureXbee()
 *
 * Inputs:	None
 *
 * Outputs:	None
 *
 * Purpose: This function uses a state machine to configure
 *			the XBee connected to the computer with the 
 *			correct settings for proper communication.
 *			This function will use the sendData() function
 *			to send AT commands to the XBee in order to configure
 *			its firmware settings without using the 3rd party program.
 */
void configureXbee(void)
{
	/* Not yet necessary for range test */
}

/*
 * receiveData()
 *	
 * Inputs:	None
 *
 * Outputs:	None
 *
 * Purpose: This function will receive data sent from
 *			another XBee.
 *			This function will read the data from the XBee serial port.
 */
void receiveData(void)
{
	// Local Variables
	DWORD numBytesToRead = 0; // Stores number of bytes to read
	BYTE dataBuffer[100]; // Data buffer for incoming bytes

	// Setup timeout for serial reads
	// Setting is to read as much as possible without blocking the Read()
	xbeeSerial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);

	do
	{
		xbeeSerial.Read(dataBuffer, sizeof(dataBuffer), &numBytesToRead);
		if(numBytesToRead > 0)
		{
			cout << dataBuffer << "\n";
		}
	}while(numBytesToRead == sizeof(dataBuffer));


}

/*
 * sendData()
 *
 * Inputs:	None
 *
 * Outputs:	None
 *
 * Purpose:	This function will transmit data to a different
 *			XBee.
 *			This function will write the data to the XBee serial port.
 */
void sendData(LPCSTR message)
{
	xbeeSerial.Write(message);
	cout << "Data sent.\n";
}