////////////////////////////////////////////////////
// Program:  XBee Test 1
// Author:   Jan Louis Evangelista
// Purpose:  This program reads the XBee and sends
//           the message on the serial monitor.
////////////////////////////////////////////////////

#include <SoftwareSerial.h>  // use software serial

// XBee's TX is connected to pin 3
// XBee's RX is connected to pin 2
SoftwareSerial XBEE(2,3);
void setup()
{
  // Initialize XBee serial port to 9600 bps
  XBEE.begin(9600);
  
  // Initialize Arduino serial port to 9600 bps
  Serial.begin(9600);
}
void loop()
{
  // Display data received by XBee on the terminal
  if (XBEE.available())
  { 
    // If data comes in from XBee, send it out to serial monitor
    Serial.write(XBEE.read());
  }
}

