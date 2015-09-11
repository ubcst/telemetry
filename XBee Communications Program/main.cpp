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

#include <iostream>		/* Standard IO library */
#include <string>		/* Strings library */
#include <Windows.h>	/* Windows API library */


using namespace std;

/* Global Variables */
#define _XBEEBAUDRATE	9600	/* XBee port baud rate (9600 bps) */
#define _XBEEBYTESIZE	8		/* XBee port packet size */
#define _XBEEPARITY		1		/* XBee port parity bit on or off */
#define _XBEESTOPBIT	1		/* XBee port stop bit 1 or 2 bits */

LPCWSTR _XBeePort = L"COM4";	/* Address of XBee Serial Port for CreateFile() */
								/* L".." string prefix specifies a wide string literal */

/* Function Declarations */
void startup();
void init_Xbee_Serial();
int promptMenu();
void configureXbee();
void receiveData();
void sendData();

int main(void)
{

	/* Initialize the program */
	startup();

	if(promptMenu())	/* Check if user wants to SEND (1) or EXIT (0) */
	{
		/* Send and receive data code goes here */

		/* FOR TEST ONLY */
		cout << "Sending data...\n";
		for(int i = 0; i < 50; i++)
		{
			cout << i;
			cout << "\n";
		}
		/* FOR TEST ONLY */
	}


	/* Exit the program */
	cin.get();
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
	/* Initialize the DCB (Device Control Block) struct */
	/* Needed for setting XBee port settings */
	DCB xbeeDCB = {0};
	
	/* Create a COM port file */
	HANDLE xbeeHandle;	/* Create a handle for the XBee COM port */

	xbeeHandle = CreateFile(_XBeePort,	/* XBee port name, set above */
							GENERIC_READ | GENERIC_WRITE,	/* Set permissions */
							0,		/* Share mode (0 = do not share with other processes) */
							0,		/* Security (0 = None ) */
							OPEN_EXISTING,	/* Open file only if device exists */
							FILE_FLAG_OVERLAPPED,	/* Asynchronous IO */
							NULL		/* No template file */
							);

	/* Check if COM Port is opened successfully */
	if(xbeeHandle == INVALID_HANDLE_VALUE)
	{
		/* Error opening the port */
		cout << "\nError opening the COM port.\n";
		while(1);
	}

	/* Set serial port settings */
	/* Check current DCB for the XBee port */
	if(!GetCommState(xbeeHandle, &xbeeDCB))
	{
		/* Error getting port settings */
		cout << "\nError getting CommState.";
		while(1);
	}

	/* Set the port settings into the DCB */
	xbeeDCB.BaudRate = _XBEEBAUDRATE;
	xbeeDCB.ByteSize = _XBEEBYTESIZE;
	xbeeDCB.Parity = _XBEEPARITY;
	xbeeDCB.StopBits = _XBEESTOPBIT;

	/* Check if DCB is set properly */
	if(!SetCommState(xbeeHandle, &xbeeDCB))
	{
		/* Error setting DCB */
		cout << "\nError setting DCB.";
		while(1);
	}

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
	/* TODO: RECEIVE CODE GOES HERE */
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
void sendData(void)
{
	/* TODO: Send code goes here */
}