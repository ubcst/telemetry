/**
 * Author: Trevor Yu
 * Email: trevor.tyu@gmail.com
 * Date: October 24, 2015
 * Summary: Code to configure and read data from a GPS sensor.
 *
 * Code primarily taken from:
 * http://stackoverflow.com/questions/18108932/linux-c-serial-port-reading-writing
 */

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

using namespace std;

const char* USB_PATH = "/dev/ttyACM0";

int USB;
int n = 0,
      spot = 0;
char buf = '\0';

char response[1024];
vector<string> nmeaLine;

void readNmeaLine( void );
void splitString( void );
bool checkNmeaLine( void );
void printGpsData( void );

/**
 * Code to split a string. Taken from:
 * http://stackoverflow.com/questions/236129/split-a-string-in-c
 */
vector<string> &split( const string &s, char delim, vector<string> &elems ) {
   stringstream ss( s );
   string item;
   while( getline( ss, item, delim ) ) {
      elems.push_back( item );
   }
   return elems;
}

/**
 * Splits a string s based on the delim parameter.
 * Returns a vector of the split elements.
 */
vector<string> split( const string &s, char delim ) {
   vector<string> elems;
   split( s, delim, elems );
   return elems;
}

void openUsb( void ) {
   /* Open file descriptor */
   USB = open( USB_PATH, O_RDWR | O_NOCTTY );

   /* Error handling */
   if ( USB < 0 ) {
      cout << "Error " << errno << " opening " << USB_PATH << ": " \
      << strerror( errno ) << endl;
   }
}

void gpsInit( void ) {
   /* Configure port */
   struct termios tty;
   memset( &tty, 0, sizeof tty );

   /* Error handling */
   if( tcgetattr( USB, &tty ) != 0 ) {
      cout << "Error " << errno << " from tcgetattr: " << \
      strerror( errno ) << endl;
   }

   /* Set baud rate speed */
   cfsetospeed( &tty, (speed_t)B115200 );
   cfsetispeed( &tty, (speed_t)B115200 );

   /* Set other port stuff */
   tty.c_cflag &= ~PARENB;
   tty.c_cflag &= ~CSTOPB;
   tty.c_cflag &= ~CSIZE;
   tty.c_cflag |= CS8;

   tty.c_cflag &= ~CRTSCTS;
   tty.c_cc[VMIN] = 1;
   tty.c_cc[VTIME] = 5;
   tty.c_cflag |= CREAD | CLOCAL;

   /* Make raw */
   cfmakeraw( &tty );

   /* Flush port, then apply attributes */
   tcflush( USB, TCIFLUSH );
   if( tcsetattr( USB, TCSANOW, &tty ) != 0 ) {
      cout << "Error " << errno << " from tcsetattr" << endl;
   }
}

/**
 * Write configuration logic (used to configure the GPS update rate).
 * The GPS uses the PMTK protocol to control and change
 * various attributes and settings.
 * Documentation for this protocol can be found:
 * https://www.adafruit.com/datasheets/PMTK_A08.pdf
 * http://www.adafruit.com/datasheets/PMTK_A11.pdf
 *
 * Part of the PMTK message is the checksum field. Use this website
 * to compute the checksum: http://www.hhhh.org/wiml/proj/nmeaxor.html
 */
void gpsConfigure( void ) {
   const char *str;

   // Cold start can take up to 15 min...
   // Perform cold start. Can't seem to get working as the GPS doesn't
   // want to accept PMTK103 messages. Commenting out for now.
   /*str = "$PMTK103*30\r\n";
   cout << "PMTK String: " << str;
   if ( strlen( str ) != write( USB, str, strlen( str ) ) ) {
      cout << "GPS cold start failed." << endl;
   }*/

   /*while( true ) {
      readNmeaLine();
      splitString();
      if( nmeaLine[0] == "$PMTK001" ) {
         cout << response << endl;
         break;
      }
   }*/

   // Only receive GPSRMC messages and GPS Fix data
   //str = "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n";
   str = "$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n";
   cout << "PMTK String: " << str;
   if ( strlen( str ) != write( USB, str, strlen( str ) ) ) {
      cout << "Read only GPSRMC messages failed." << endl;
   }

   // Turn off the EASY function because it only works for 1Hz.
   // Commenting out because our GPS doesn't seem to support this
   // command.
   /*str = "$PMTK869,1,0*34\r\n";
   cout << "PMTK String: " << str;
   if ( strlen( str ) != write( USB, str, strlen( str ) ) ) {
      cout << "Turn off EASY function failed." << endl;
   }*/

   // Change update rate to 5Hz.
   str = "$PMTK220,200*2C\r\n";
   cout << "PMTK String: " << str;
   if ( strlen( str ) != write( USB, str, strlen( str ) ) ) {
      cout << "Change update rate to 5Hz failed." << endl;
   }
}

/* Read the GPS NMEA messages */
void readNmeaLine( void ) {
   spot = 0;
   /* Read a NMEA line */
   do {
      n = read( USB, &buf, 1 );
      if ( buf == '\n' ) {
         continue;
      }
      sprintf( &response[spot], "%c", buf );
      spot += n;
   } while( buf != '\r' && n > 0 );
}

void splitString( void ) {
   // Split the string based on the comma delimiter
   string resp( response );
   nmeaLine = split( resp, ',' );
}

/**
 * Perform various checks on the line that has been read.
 */
bool checkNmeaLine( void ) {
   // Check if read line is a PMTK message
   string prefix = "$PMTK";
   if ( !nmeaLine.at(0).compare( 0, prefix.size(), prefix ) ) {
      cout << "PMTK message: " << response << endl;
      return false;
   }

   // Check if number of values in the nmeaLine is the number of
   // values we expect the GPSRMC message to have.
   if ( nmeaLine.size() < 12 ) {
      cout << "Too few elements: " << response << endl;
      return false;
   }

   // Check if the NMEA line is valid
   if ( nmeaLine.at( 2 ) == "V" ) {
      cout << "Invalid message: " << response << endl;
      return false;
   }

   return true;
}

// Print out response message
void printGpsData( void ) {
   string timeStamp = nmeaLine.at(1);
   double latitude = atof( nmeaLine.at(3).c_str() ) / 100.00;
   string northSouth = nmeaLine.at(4);
   double longitude = atof( nmeaLine.at(5).c_str() ) / 100.00;
   string eastWest = nmeaLine.at(6);
   cout << "Timestamp: " << timeStamp << " Latitude: " << latitude <<
      northSouth << " Longitude: " << longitude << eastWest << endl;
}

int main( void ) {
   openUsb();

   // Initialize the GPS
   gpsInit();

   // Configure the GPS
   gpsConfigure();

   memset( response, '\0', sizeof( response ) );

   while ( true ) {
      readNmeaLine();

      splitString();

      if( !checkNmeaLine() ) {
         continue;
      }

      printGpsData();
   }

   return 0;
}


