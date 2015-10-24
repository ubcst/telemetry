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

#define XBEEMODE "xbee3"
#define XBEEPORT "/dev/ttyUSB0"
#define XBEEBAUD 57600

struct xbee *xbee;
struct xbee_con *con;
unsigned char txRet;
xbee_err ret;
struct xbee_pkt *pkt;

int xbeeSetup(void);

int main( int argc, char **argv )
{
	//void *d;

	int i;

	// Setup the XBee Module
	xbeeSetup();


	// Create a new AT connection to the local XBee at COM4
	// If the connection fails to be made, print an error code
	if((ret = xbee_conNew(xbee, &con, "Local AT", NULL)) != XBEE_ENONE)
	{
		printf("Ret: %d, (%s)\n", ret, xbee_errorToStr(ret));
		return ret;
	}
	
	printf("ConNew Ret: %d (%s)\n", ret, xbee_errorToStr(ret));

	// Send an AT command NI to ask for Node Identity
	ret = xbee_conTx(con, &txRet, "NI");

	// Print out the return code
	printf("TX: %d (%s)\n", ret, xbee_errorToStr(ret));

	// If the return code =/= 0, then an error occured
	if(ret)    
	{
		printf("Error - TXRet: %d\n", txRet);
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
	// Setup the XBee of Series 1 at COM4, Baud Rate 115200
	// If the XBee doesn't initialize, print an error code
	if((ret = xbee_setup(&xbee, XBEEMODE, XBEEPORT, XBEEBAUD)) != XBEE_ENONE)
	{
		printf("Ret: %d, (%s)\n", ret, xbee_errorToStr(ret));
		return ret;
	}
	
	printf("Setup returned: %d (%s)\n", ret, xbee_errorToStr(ret));
	
	return 1;
}
