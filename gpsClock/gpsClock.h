// gpsClock - clock library to get time, date from gps nmea string

// created: April 11, 2014  G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Apr 24/14 - second getTime to fetch date as well


#ifndef GPSCLOCK_HDR
#define GPSCLOCK_HDR

#include "clock.h"
#include <SoftwareSerial.h>

class gpsClock : public clock, public SoftwareSerial {

public:
	gpsClock( SoftwareSerial & _gpss ) : SoftwareSerial( _gpss ), gpss(_gpss) { }

	byte getTime( Time & );
	byte getTime( Time &, Date & );		//get both from same sentence
	byte getDate( Date & );
	byte getStatus( byte, byte, byte * );	//array form: adr, cnt, array
	byte getStatus( byte, byte * );			//single byte
	byte getAlarm( byte almNr, byte * );
	byte setTime( Time &setT );
	byte setDate( Date &setD );
	byte setStatus( byte regadr, byte cnt, byte *setS );
	byte setStatus( byte regadr, byte val );
	byte setAlarm( byte almNr, byte *setA );


private:
	char buffer[90];	// string buffer for the sentence
	char *pptr;			// a character pointer for parsing
	char bix;			// an indexer into the buffer
	byte hour, minute, second, day, month, year, status;	//results of parse

	SoftwareSerial &gpss;	// reference to the softwareserial port being used

	void readline( );
	byte parse( );
	byte calcdow( );



}; // class gpsClock



#endif


