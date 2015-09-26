////////////////////////////////////////////////////
//
// XBee Communications - Linux
//
// Author: 	Jan Louis Evangelista
//
// Purpose: This program is used to configure the
//			ID's of the XBee module
//
////////////////////////////////////////////////////

// Header files
#include <stdio.h>		// Standard IO Definitions
#include <stdlib.h>		// Standard Library Definitions
#include <string.h>		// String Functions Definitions
#include <unistd.h>		// UNIX Standard IO Definitons
#include <fcntl.h>		// File Control Definitions
#include <errno.h>		// Error Numbers Definitions
#include <termios.h>	// Terminal Definitions

// Function Prototypes
void init_Xbee_Serial(void);
void init_GPS_Serial(void);
int config_Xbee_Commands(void);
void write_Xbee(char*);


// Macros
#define TRUE	1
#define FALSE	0

#define	_XBEEPORTPATH	"/dev/ttyUSB0" 	// USB port on heatsink side
#define _GPSPORTPATH	"/dev/ttyUSB1"	// USB port on CD side
#define _POSIX_SOURCE	1
#define _BAUDRATE		B9600			// Baud rate set to 9600 bps

// Global Variables
int Xbee_Serial_File;	// XBee Serial File
int GPS_Serial_File;	// GPS Serial File

int num_Bytes_Sent;		// number of bytes sent on XBee serial port
int num_Bytes_Rcvd;		// number of bytes received on port

char output[255];		// string output of XBee
char input[255];		// string input for XBee

struct termios XBEE_OPTIONS;	// Serial port configuration struct

// Start Main()
int main()
{
	// Initialize XBee Serial Port
	init_Xbee_Serial();
	
	// Initialize GPS Serial Port
	// DO NOT USE YET!!!
	// init_GPS_Serial();
	
	// Configure XBee AT Commands
	// DO NOT USE YET!!!
	// config_Xbee_Commands();

	while(strcmp(output,"exit") != 0)
	{
		// Prompt user for string to send to Arduino
		printf("Enter a string to send: ");
		scanf("%s", output);
		
		// Write data into the port
		write_Xbee(output);
	}
	
	// Close the ports
	close(Xbee_Serial_File);
	close(GPS_Serial_File);
	
	// Exit the program
	return 0;
}

////////////////////////////////////////////////////
//
// init_Xbee_Serial()
//
// Inputs:	None
//
// Outputs:	None
//
// Purpose:	This function initializes the XBee serial
//			port to the assigned value
//
////////////////////////////////////////////////////
void init_Xbee_Serial(void)
{
	// Initialize Xbee Serial File as a write-only, non-controlling
	// terminal with no delay
	Xbee_Serial_File = open( _XBEEPORTPATH,	// XBee port path 
							 O_RDWR | 		// Read-Write flag
							 O_NOCTTY | 	// Non-control flag
							 O_NDELAY );	// No delay flag
	
	// Port open error handler 
	if( Xbee_Serial_File < 0)
	{
		printf("Cannot open XBee serial port! Errno: %d\n", errno);

		////////////////////////////////////////////
		// FIX ERROR HANDLER
		////////////////////////////////////////////
		while(1);
	}
	
	// Configure XBee serial port
	XBEE_OPTIONS.c_cflag = 	_BAUDRATE |	// Set baud rate
							CRTSCTS |	// Enable hardware flow control
							CS8 | 		// Set 8 data bits, no parity, 1 stop bit
							CLOCAL | 	// Local owner
							CREAD;		// Enable receiver
	XBEE_OPTIONS.c_iflag = IGNPAR | ICRNL;
	XBEE_OPTIONS.c_oflag = 0;
	XBEE_OPTIONS.c_lflag = ICANON;
	XBEE_OPTIONS.c_cc[VMIN]=1;
	XBEE_OPTIONS.c_cc[VTIME]=0;
	tcsetattr(Xbee_Serial_File,TCSANOW,&XBEE_OPTIONS); 
}

////////////////////////////////////////////////////
//
// init_GPS_Serial()
//
// Inputs:	None
//
// Outputs:	None
//
// Purpose:	This function initializes the GPS serial
//			port to the assigned value
//
////////////////////////////////////////////////////
void init_GPS_Serial(void)
{
	// Initialize GPS Serial File as a read-only, non-controlling
	// terminal with no delay
	GPS_Serial_File = open( _GPSPORTPATH,	// GPS port path 
							O_RDONLY | 	// Read only flag
							O_NOCTTY | 	// Non-control flag
							O_NDELAY );	// No delay flag
	
	// Port open error handler 
	if( GPS_Serial_File < 0)
	{
		printf("Cannot open GPS serial port! Errno: %d\n", errno);
		
		////////////////////////////////////////////
		// FIX ERROR HANDLER
		////////////////////////////////////////////
		while(1);
	}
}

////////////////////////////////////////////////////
//
// config_Xbee_Commands()
//
// Inputs:	None
//
// Outputs:	0 - configuration of module failed
//			1 - configuration of module succeeded 
//
// Purpose:	This function uses an FSM to configure
//			the XBee using terminal commands
//			Configure to the following:
//			-9600 bps, 8 bits, 1 stop bit, no parity
//			- Enable FIFO
//			- Set as End Device (CE = 0)
//			- Disable End Device Assoc. (A1 = 0)
//			- DH = 0, DL = 0xBB, MY = 0xAA
//
////////////////////////////////////////////////////
int config_Xbee_Commands(void)
{
	int 	fsm_state = 0;	// state variable for current state
	
	char 	cmd_On[] = {'+','+','+'};
	
	// TODO: Complete the configuration state machine
	// Not yet required unless interference with other XBees an issue
	while(1)
	{
		switch(fsm_state)
		{
			case 0:
				sleep(2);	// Guard Time
				// send the "+++" command to enter config mode
				num_Bytes_Sent = write(	Xbee_Serial_File, 
										cmd_On, 
										(sizeof cmd_On));
				
				printf("Sent: %s", cmd_On);
				sleep(2);	// Guard Time

				num_Bytes_Rcvd = read(	Xbee_Serial_File,
										input, sizeof input);
				printf("\nReceived: %s\n", input);
				
				if(strcmp(input,"OK\r") == 0)
				{
					printf("\n+++ accepted!\n");
					fsm_state = 1;
				}
				else
				{
					printf("\n+++ rejected!\n");
					fsm_state = 6;
				}
				break;
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:			// Configure is successful
				return 1;
			case 6:			// Configuration failed
				return 0;
			default: 		// Configuration failed
				return 0;
		}
	}
}

////////////////////////////////////////////////////
//
// write_Xbee()
//
// Inputs:	message
//
// Outputs:	None
//
// Purpose:	This function writes the char message
//			into the serial port
//
////////////////////////////////////////////////////
void write_Xbee(char msg[])
{
	// Write the string onto the serial port
	num_Bytes_Sent = write(Xbee_Serial_File, msg, (sizeof msg) - 1);

	printf("\nXBee sent: ");
	printf("%s", msg);
	printf(" and sent %d bytes\n", num_Bytes_Sent);
	
	// Check if the write failed
	if(num_Bytes_Sent < 0)
	{
		printf("Error sending data\n");
	}	
}
