// gpsTime.ino - simple sketch to test gpsClock library

// created: April 12, 2014  G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Apr 24/14 - use getTime variant to fetch both time, date
//                    - Include status character report

#include <SoftwareSerial.h>
#include <gpsClock.h>

// Use pins 2 and 3 to talk to the GPS. 3 is the gps TX pin,
// pin 2 is the gps RX pin (SoftwareSerial RX, TX respectively)
SoftwareSerial gpsSerial(3, 2);
gpsClock gpc( gpsSerial );

// Set the GPSRATE to the baud rate of the GPS module. Most are 4800
// but some are 38400 or other. Check the datasheet!
#define GPSRATE 4800
//#define GPSRATE 9600
//#define GPSRATE 38400

Time time;
Date date;

void setup( ) { 
  Serial.begin(9600);
  // connect to the GPS at the desired rate
  gpsSerial.begin( GPSRATE );
} // setup( )

void loop( ) {

//  byte err = gpc.getTime( time );
  byte err = gpc.getTime( time, date );  // see 1/sec output
  err = (err<<4);
//  err |= gpc.getDate( date );          // here get 1/2sec output
  if( err != RTC_OK ) {
    Serial.print( "clock read error: " );
    Serial.println( err, HEX );
  }
  
  byte lastStat;
  gpc.getStatus( 0, &lastStat );
  Serial.print( (char)lastStat );
  Serial.print( " " );

  Serial.print( date.dow, HEX );
  if( date.yr < 10 ) Serial.print( "  200" );
   else Serial.print( "  20" );
  Serial.print( date.yr, HEX );
  Serial.print( "-" );
  Serial.print( date.mo, HEX );
  Serial.print( "-" );
  Serial.print( date.dom, HEX );
  Serial.print( "  " );
  Serial.print( time.hr, HEX );
  Serial.print( ":" );
  Serial.print( time.min, HEX );
  Serial.print( ":" );
  Serial.println( time.sec, HEX );


} // loop( )


