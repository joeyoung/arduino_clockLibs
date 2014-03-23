// MCP7941x.h - header for MCP7941x chip version
// I2C clock access package

// started: Oct 28, 2013 G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar 08/14 - add psStatush for high status bytes, bit names
// 			Mar 18/14 - modify getStatus setStatus for single byte, block access
//			Mar 22/14 - MCP7941x version


// Implementation for MCP7941x, I2C, clock IC (no temp compensation)

#ifndef MCP7941x_HDR
#define MCP7941x_HDR

#include "clock.h"
#include "Wire.h"

// data sheet bit names for MCP7941x status, alarm registers

// Misc. status bits among time/date registers
#define ST 7
#define OSCON 5
#define VBAT 4
#define VBATEN 3
#define LP 7
// control register (adr 0x07)
#define OUT 7
#define SQWE 6
#define ALM1 5
#define ALM0 4
#define EXTOSC 3
#define RS2 2
#define RS1 1
#define RS0 0
// alarm 0 controls (adr 0x0D)
#define ALM0POL 7
#define ALM0C2 6
#define ALM0C1 5
#define ALM0C0 4
#define ALM0IF 3
// alarm1 controls (adr 0x14)
#define ALM1POL 7
#define ALM1C2 6
#define ALM1C1 5
#define ALM1C1 4
#define ALM1IF 3
// misc, useful register adresses
#define ALARM0ADR 0x0A
#define ALARM1ADR 0x11
#define MCSTATUS 0x07
#define MCSTATMS 0x18
#define ALARM_SIZE 6

class MCP7941x : public clock {

public:

	MCP7941x( );

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


private:
	static const byte NRREG = 0x20;		//number of clock chip's registers
	static const byte I2Cadr = 0x6F;	//7-bit I2C address
	byte errCode;

	void writeClock( byte regadr );
	byte readClock( byte regadr, byte count );
	static const byte mcDate = 0x03;	//offset to start of Date register block
	static const byte mcAlarm = 0x0a;	//offset to start of alarm registers
	static const byte mcAlarm1 = 0x11;	//offset to start of alarm registers
	static const byte mcTime = 0x00;	//offset to start of time registers


}; // class MCP7941x

#endif


