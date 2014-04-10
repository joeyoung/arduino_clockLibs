// DS1307.h - header for DS1307 chip version
// I2C clock access package

// started: Oct 28, 2013 G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar 08/14 - add psStatush for high status bytes, bit names
// 			Mar 18/14 - modify getStatus setStatus for single byte, block access
//			Mar 22/14 - MCP7941x version
//			Mar 25/14 - DS1307 version
//			Mar 27/14 - implement DS1307 'software alarm'
//			Apr 08/14 - move timer to sketch array for multiple alarms


// Implementation for DS1307, I2C, clock IC (no temp compensation)

#ifndef DS1307_HDR
#define DS1307_HDR

#include "clock.h"
#include "Wire.h"

// data sheet bit names for DS1307 status, alarm registers

// Misc. status bits among time/date registers
#define CH 7
// control register (adr 0x07)
#define OUT 7
#define SQWE 4
#define RS1 1
#define RS0 0
// alarm enable bit
#define DSAE 7
#define DSAF0 0
#define DSAF1 1
// misc, useful register adresses
#define DS7STATUS 0x07
#define DS7RWM 0x08
#define DS7ALMCTL 0x08
#define ALARM_SIZE 4

class DS1307 : public clock {

public:

	DS1307( );

	byte getTime( Time & );
	byte getDate( Date & );
	byte getStatus( byte, byte, byte * );	//array form: adr, cnt, array
	byte getStatus( byte, byte * );			//single byte
	byte getAlarm( byte almNr, byte * );
	byte setTime( Time &setT );
	byte setDate( Date &setD );
	byte setStatus( byte regadr, byte cnt, byte *setS );
	byte setStatus( byte regadr, byte val );
	byte setAlarm( byte almNr, byte *setA );

	bool checkAlarm( byte almNr, byte *, unsigned long * );	//for 'software alarm'


private:
	static const byte NRREG = 0x40;		//number of clock chip's registers (incl rwm)
	static const byte I2Cadr = 0x68;	//7-bit I2C address
	byte errCode;

	void writeClock( byte regadr );
	byte readClock( byte regadr, byte count );
	static const byte ds7Date = 0x03;	//offset to start of Date register block
	static const byte ds7Alarm = 0x09;	//offset to start of alarm registers
	static const byte ds7Time = 0x00;	//offset to start of time registers

}; // class DS1307

#endif


