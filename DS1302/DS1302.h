// DS1302.h - header for DS1302 chip version of clockLibs

// created: April 29, 2014  G. D. (Joe) Young <jyoung@islandnet.com>
//
// revised: May 3/14 - setTime with burst write
//


// Implementation for DS1302 clock IC - does not use I2C

#ifndef DS1302_HDR
#define DS1302_HDR

#include "clock.h"

// data sheet bit names for DS1302 status, alarm registers

// Misc. status bits among time/date registers
#define CH 7
// control registers
#define WP 7
#define RS1 1
#define RS0 0
#define DS0 2
#define DS1 3
#define TCS 0xA0	// trickle charge enable (with RS, DS bits)
// alarm enable bit
#define DSAE 7
#define DSAF0 0
#define DSAF1 1
// misc, useful register adresses
#define DS2RWM 0x20
#define DS2ALMCTL 0x20
#define ALARM_SIZE 4

class DS1302 : public clock {

public:

	DS1302( byte ds2clk, byte ds2data, byte ds2ce );

	byte getTime( Time & );					//burst reads time only
	byte getTime( Time &, Date & );			//burst reads both
	byte getDate( Date & );
	byte getStatus( byte, byte, byte * );	//array form: adr, cnt, array
	byte getStatus( byte, byte * );			//single byte
	byte getAlarm( byte almNr, byte * );
	byte setTime( Time &setT );
	byte setTime( Time &, Date & );			//for burst write all clock regs
	byte setDate( Date &setD );
	byte setStatus( byte regadr, byte cnt, byte *setS );
	byte setStatus( byte regadr, byte val );
	byte setAlarm( byte almNr, byte *setA );

	bool checkAlarm( byte almNr, byte *, unsigned long * );	//for 'software alarm'


private:
	static const byte NRREG = 0x28;		//number of clock chip's registers (incl rwm)
	static const byte baseadr = 0x40;	//7-bit register addresses start at 0x40
	static const byte burstoff = 0x1f;	// address offset to burst access locations
	byte errCode, ds2clk, ds2data, ds2ce;
	byte allclock[7];

	void writeCmd( byte cmd );
	void writeClock( byte regadr, byte value );			//single byte write
	void writeClock( byte regadr, byte count, byte * );	//burst write
	void readClock( byte regadr, byte count, byte * );	//burst read
	byte readClock( byte regadr );		// single byte read
	static const byte ds2Date = 0x03;	//offset to start of Date register block
	static const byte ds2Alarm = 0x21;	//offset to start of alarm registers
	static const byte ds2Time = 0x00;	//offset to start of time registers

}; // class DS1302

#endif


