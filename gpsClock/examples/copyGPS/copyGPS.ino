// copyGPS - copy GPS time to clock IC

// created: April 24, 2014  G. D. (Joe) Young <jyoung@islandnet.com>

#include <SoftwareSerial.h>
#include <gpsClock.h>

#include <Wire.h>

//#include <DS3231.h>  // un-comment for chip to be set
//#include <DS1307.h>
//#include <RV3029.h>
//#include <MCP7941x.h>
#include <PCF8563.h>

// Use pins 2 and 3 to talk to the GPS. 3 is the gps TX pin,
// pin 2 is the gps RX pin (SoftwareSerial RX, TX respectively)
SoftwareSerial gpsSerial(3, 2);
gpsClock gpc( gpsSerial );

// Set the GPSRATE to the baud rate of the GPS module. Most are 4800
// but some are 38400 or other. Check the datasheet!
#define GPSRATE 4800
//#define GPSRATE 9600
//#define GPSRATE 38400

#ifdef DS3231_HDR
DS3231 clk;
#endif

#ifdef DS1307_HDR
DS1307 clk;
#endif

#ifdef RV3029_HDR
RV3029 clk;
#endif

#ifdef MCP7941x_HDR
MCP7941x clk;
#endif

#ifdef PCF8563_HDR
PCF8563 clk;
#endif

Time time;
Date date;

boolean nogo;

void setup( ) { 
  Serial.begin(9600);
  Wire.begin( );
  // connect to the GPS at the desired rate
  gpsSerial.begin( GPSRATE );
  nogo = false;
  byte err = clk.getTime( time );    // check chip is readable
  if( err != RTC_OK ) {
    nogo = true;
    Serial.print( "clock read error: " );
    Serial.println( err, HEX );
  }

  err = gpc.getTime( time, date );  // read gps
  if( err != RTC_OK ) {
    nogo = true;
    Serial.print( "gps read error: " );
    Serial.println( err, HEX );
  }
  
  if( nogo ) {
    Serial.println( "setting failed" );
  } else {
    clk.setTime( time );
    clk.setDate( date );
    Serial.print( "clock set to: " );
    clk.getDate( date );
    clk.getTime( time );
    Serial.print( date.yr, HEX );
    Serial.print( '/' );
    Serial.print( date.mo, HEX );
    Serial.print( '/' );
    Serial.print( date.dom, HEX );
    Serial.print( " " );
    Serial.print( date.dow, HEX );
    Serial.print( "   " );
    clk.getTime( time );
    Serial.print( time.hr, HEX );
    Serial.print( ":" );
    Serial.print( time.min, HEX );
    Serial.print( ":" );
    Serial.println( time.sec, HEX );
  }

} // setup( )

void loop( ) {

}

