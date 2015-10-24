/**
 * Code primarily taken from:
 * http://stackoverflow.com/questions/18108932/linux-c-serial-port-reading-writing
 */

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

using namespace std;

int main( void ) {
   const char* USB_PATH = "/dev/ttyACM0";

   /* Open file descriptor */
   int USB = open( USB_PATH, O_RDONLY | O_NOCTTY );

   /* Error handling */
   if ( USB < 0 ) {
      cout << "Error " << errno << " opening " << USB_PATH << ": " \
      << strerror( errno ) << endl;
   }

   /* Configure port */
   struct termios tty;
   memset( &tty, 0, sizeof tty );

   /* Error handling */
   if( tcgetattr( USB, &tty ) != 0 ) {
      cout << "Error " << errno << " from tcgetattr: " << \
      strerror( errno ) << endl;
   }

   /* Set baud rate speed */
   cfsetospeed( &tty, (speed_t)B9600 );
   cfsetispeed( &tty, (speed_t)B9600 );

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

   /* Read stuff */
   int n = 0,
       spot = 0;
   char buf = '\0';

   /* Whole response */
   char response[1024];
   memset( response, '\0', sizeof( response ) );

   while ( true ) {
      spot = 0;
      do {
         n = read( USB, &buf, 1 );
         if ( buf == '\n' ) {
            continue;
         }
         sprintf( &response[spot], "%c", buf );
         spot += n;
      } while( buf != '\r' && n > 0 );

      if ( n < 0 ) {
         std::cout << "Error reading: " << strerror( errno ) << std::endl;
      } else if ( n == 0 ) {
         std::cout << "Read nothing!" << std::endl;
      } else {
         std::cout << "Response: " << response << std::endl;
      }
   }

   return 0;
}
