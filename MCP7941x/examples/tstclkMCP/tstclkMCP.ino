// testclk - use library clock routines to send date and time to serial

// created: Feb 25, 2014  G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar 4/14 - use get's error return
//          Mar 13/14 - RV3029, overwrite time line
//          Mar 18/14 - PCF8563 version. Same source for all clocks
//          Mar 22/14 - MCP7941x verion


#include "MCP7941x.h"
//#include "PCF8563.h"  // uncomment whichever lib corresponds to connected chip
//#include "DS3231.h"
//#include "RV3029.h"
#include "Wire.h"

#define INTERVAL 1000		// number of msec between printing times
#ifdef MCP7941x_HDR
MCP7941x clk;
#endif
#ifdef DS3231_HDR
DS3231 clk;
#endif
#ifdef RV3029_HDR
RV3029 clk;
#endif
#ifdef PCF8563_HDR
PCF8563 clk;
#endif

Time time;
Date date;

unsigned long timer;

void setup( ) {
  Serial.begin( 9600 );
  Wire.begin( );
  Serial.print( "begin testclk - " );
  byte err = clk.getDate( date );
  if( err != 0 ) {
    Serial.print( "read err: " );
    Serial.print( err, HEX );
    Serial.print( "   " );
  }
  Serial.print( date.yr, HEX );
  Serial.print( '/' );
  Serial.print( date.mo, HEX );
  Serial.print( '/' );
  Serial.println( date.dom, HEX );
  timer = millis( ) + INTERVAL;		//set up time printing
}


void loop( ) {
  if( timer < millis( ) ) {
    timer += INTERVAL;
    byte err = clk.getTime( time );
    if( err != 0 ) {
      Serial.print( "read err: " );
      Serial.print( err, HEX );
      Serial.print( "   " );
    }
    Serial.print( time.hr, HEX );
    Serial.print( ':' );
    Serial.print( time.min, HEX );
    Serial.print( ':' );
    Serial.println( time.sec, HEX );
  }
}

