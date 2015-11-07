/*
 * XBee Comms Program
 *
 * Author: Jan Louis Evangelista
 *
 * This program uses the Master XBee to send and receive data
 * to the Slave XBee connected to an Arduino.
 * The program uses the LibXBee library to do all XBee functions.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xbee.h" // LibXBee Library

#define XBEEMODE "xbee1" // xbee1 = short range, xbee3 = long range
#define XBEEPORT "/dev/ttyUSB0" // Change according to order plugged in
#define XBEEBAUD 9600 // As configured in XCTU

struct xbee *xbee;
struct xbee_con *con;
unsigned char txRet;
xbee_err ret;
struct xbee_pkt *pkt;
struct xbee_conAddress address;

 
FILE *logFile;

int xbeeSetup(void);

int main( int argc, char **argv )
{
	//void *d;

	int i; // Loop counter
	
	char data[256]; // Data string array 
	
	// Address of receiver XBee attached to Arduino
	// Configured using XCTU, change when conflicts detected
	// Destination Address HI: 0x00
	// Destination Address LO: 0x00
	memset(&address, 0, sizeof(address));
	address.addr16_enabled = 1;
	address.addr16[0] = 0x00;
	address.addr16[1] = 0x00;

	// Setup the XBee Module
	xbeeSetup();

	// Create a new 2-way remote communication with 16-bit addressing
	// Return error code if a connection cannot be made
	if((ret = xbee_conNew(xbee, &con, "16-bit Data", &address)) != XBEE_ENONE)
	{
		printf("Con New Ret: %d, (%s)\n", ret, xbee_errorToStr(ret));
		return ret;
	}
	
	printf("ConNew Ret: %d (%s)\n", ret, xbee_errorToStr(ret));
	
	printf("Start transmitting TX data...\n");
	printf("-----------------------------\n");
	
	// Transmission Loop
	// The transmit loop takes an input string {data} and sends it to the
	// XBee for transmission using xbee_conTx(). If transmission fails,
	// an error code is returned.
	// Transmission loop can be exited by entering "exit" into {data}
	// The {data} can be modified to be anything, be it GPS data or telemetry
	// data, as long as it is string-formatted. User input is optional.	
	do
	{
		// Read a string from the console
		printf("\nEnter string to send: ");
		scanf("%s", data);
		printf("\nData entered: %s", data);
		
		// Check for "exit" keyword
		if(strcmp(data, "exit") != 0)
		{
			// Transmit the data, check for error code returned
			if((ret = xbee_conTx(con, &txRet, data)) != XBEE_ENONE)
			{
				printf("ConTX Returned: %d, (%s)\n", ret, xbee_errorToStr(ret));
				return ret;
			}
		}	
		
	}while(strcmp(data, "exit") != 0); // exit transmit loop on keyword "exit"

	// Reception Loop
	// TODO: Cleanup the code
	//
	// If the return code =/= 0, then an error occured
	if(ret)    
	{
		printf("Error - TXRet: %d (%s)\n", txRet, xbee_errorToStr(txRet));
	}
	else
	{
		if((ret = xbee_conRx(con, &pkt, NULL)) != XBEE_ENONE)
		{
			printf("Error calling xbee_conRx(): %s\n", xbee_errorToStr(ret));
		}
		else
		{
			printf("Response is %d bytes long.\n", pkt -> dataLen);
			for(i = 0; i < pkt -> dataLen; i++)
			{
				unsigned char c = (((pkt -> data[i] >= ' ') && (pkt->data[i] <= '~')) 
					? pkt->data[i]:'.');

				printf("%3d: 0x%02X - %c\n", i, pkt->data[i], c);

			}
		}
	}


	// Close the XBee connection
	if((ret = xbee_conEnd(con)) != XBEE_ENONE)
	{
		xbee_log(xbee, -1, "xbee_conEnd() returned: %d", ret);
		return ret;
	}
	
	// Shutdown the XBee library
	xbee_shutdown(xbee);
	
	// Exit the program
	return 0;
}

int xbeeSetup(void)
{
	// Setup the XBee at specified serial port, specified baud rate
	// Return errpr code if the XBee doesn't initialize properly
	if((ret = xbee_setup(&xbee, XBEEMODE, XBEEPORT, XBEEBAUD)) != XBEE_ENONE)
	{
		printf("Mode Setup Ret: %d, (%s)\n", ret, xbee_errorToStr(ret));
		return ret;
	}
	
	printf("Setup returned: %d (%s)\n", ret, xbee_errorToStr(ret));
	
	if ((logFile = fopen("libxbee.log", "w")) == NULL) 
	{
		printf("Log file is NULL\n");
	}
	
	if ((ret = xbee_logTargetSet(xbee, logFile)) != XBEE_ENONE)
	{
		printf("XBee Log File Target Set Ret: %d (%s)\n", ret, xbee_errorToStr(ret));
	} 
	
	if ((ret = xbee_logLevelSet(xbee, 100)) != XBEE_ENONE)
	{
		printf("XBee Log Level Set Ret: %d (%s)\n", ret, xbee_errorToStr(ret));
	} 	
	
	if ((ret = xbee_logTxSet(xbee, 100)) != XBEE_ENONE)
	{
		printf("XBee Log TX Set Ret: %d (%s)\n", ret, xbee_errorToStr(ret));
	} 
	
	printf("Log File Setup Complete\n");
	printf("Setup Complete!\n");
	
	return 1;
}
