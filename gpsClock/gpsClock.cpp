// gpsClock - clock library to get time, date from gps nmea string

// created: April 11, 2014  G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Apr 24/14 - getStatus, getTime( time, date ) and calculated day of week

#include "gpsClock.h"
#include <stdio.h>




byte gpsClock::getTime( Time &gtm ) {
	readline( );
	while( strncmp( buffer, "$GPRMC", 6 ) != 0 ) readline( );
	byte err = parse( );
	gtm.hr = hour;
	gtm.min = minute;
	gtm.sec = second;
	return err;
}

byte gpsClock::getTime( Time &gtm, Date &gdt ) {
	readline( );
	while( strncmp( buffer, "$GPRMC", 6 ) != 0 ) readline( );
	byte err = parse( );
	gtm.hr = hour;
	gtm.min = minute;
	gtm.sec = second;
	gdt.dom = day;
	gdt.mo = month;
	gdt.yr = year;
	gdt.dow = calcdow( );
	return err;
}

byte gpsClock::getDate( Date &gdt ) {
	readline( );
	while( strncmp( buffer, "$GPRMC", 6 ) != 0 ) readline( );
	byte err = parse( );
	gdt.dom = day;
	gdt.mo = month;
	gdt.yr = year;
	gdt.dow = calcdow( );
	return err;
}

// $GPRMC single-character status from last parse; A=>data valid, V=>data not valid
byte gpsClock::getStatus( byte dum, byte *gst ) {
	*gst = status;
	return RTC_OK;
}

// These clock library functions are not applicable for gps clock source
byte gpsClock::getStatus( byte, byte, byte * ){ }
byte gpsClock::getAlarm( byte almNr, byte * ) { }
byte gpsClock::setTime( Time &setT ) { }
byte gpsClock::setDate( Date &setD ) { }
byte gpsClock::setStatus( byte regadr, byte cnt, byte *setS ) { }
byte gpsClock::setStatus( byte regadr, byte val ) { }
byte gpsClock::setAlarm( byte almNr, byte *setA ) { }


byte gpsClock::calcdow( ) {
	byte index;
	int iday, imonth, iyear;

	iday = (int)( ((day&0x30)>>4)*10 ) + (int)( day & 0x0f );
	imonth = (int)( ((month&0x10)>>4)*10 ) + (int)( month&0x0f );
//	if( date.yr > 80 ) year = (int)date.yr + 1900;
//	else  year = (int)date.yr+2000;
	iyear = (int)( ((year&0xf0)>>4)*10 ) + (int)( year&0x0f );	// no century info
	if (imonth > 2)
		imonth -= 2;
	else {
		imonth += 10;
		iyear--;
	}

// eliminate the century leap year correction. 2000 WAS a leap year
// NG for dates before 2000.

//   index = ((13 * month - 1) / 5) + day + (year % 100) 
//           + ((year % 100) / 4) + ((year / 100) / 4) 
//           - 2 * (year / 100) + 77;
   index = ((13 * imonth - 1) / 5) + iday + (iyear) 
           + ((iyear) / 4) + 77; 
           
   index = index - 7 * (index / 7);

   if (index == 7)
      index = 0;

   return( (byte)index+1 );

} // calcdow( )


byte gpsClock::parse( ) {

	if( strncmp( buffer, "$GPRMC", 6 ) == 0 ) {
    
		pptr = buffer+7;
		hour = (byte)( ((pptr[0]&0x0f)<<4) |  (pptr[1]&0x0f) );
		minute = (byte)( ((pptr[2]&0x0f)<<4) |  (pptr[3]&0x0f) );
		second = (byte)( ((pptr[4]&0x0f)<<4) |  (pptr[5]&0x0f) );
    
		pptr = strchr(pptr, ',') + 1;
		status = pptr[0];
    
		// skip over lat, long, speed
		pptr += 2;
		pptr = strchr(pptr, ',') + 1;  // lat
		pptr = strchr(pptr, ',') + 1;  // N/S
		pptr = strchr(pptr, ',') + 1;  // long
		pptr = strchr(pptr, ',') + 1;  // E/W
		pptr = strchr(pptr, ',') + 1;  // speed over ground
		pptr = strchr(pptr, ',') + 1;  // course over ground

		// date
		day = (byte)( ((pptr[0]&0x0f)<<4) |  (pptr[1]&0x0f) );
		month = (byte)( ((pptr[2]&0x0f)<<4) |  (pptr[3]&0x0f) );
		year = (byte)( ((pptr[4]&0x0f)<<4) |  (pptr[5]&0x0f) );
    
//    sprintf( buffer, "%.4d/%.2d/%.2d %.2d:%.2d:%.2d", 
//               year+2000, month, day, hour, minute, second );
//    Serial.println( buffer );
		return RTC_OK;
	} // if $GPRMC    
	return RTC_RD_ERR;
} // parse( )


void gpsClock::readline(void) {
	char c;
  
	bix = 0; // start at begninning
	while (1) {
		c = gpss.read();
		if (c == -1)
			continue;
//		Serial.print( c );
		if (c == '\n')
			continue;
		if ((bix == 90-1) || (c == '\r')) {
			buffer[bix] = 0;
			return;
		}
		buffer[bix++]= c;
	} // while gathering characters in one line
} // readline( )
