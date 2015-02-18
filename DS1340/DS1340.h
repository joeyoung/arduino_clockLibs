// DS1340.h - header for DS1340 version
// I2C clock access package

// started: Oct 22, 2013 G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Oct 28/13 - NRREG moved to constructor
//			Nov 2/13 - modified so storage is with user
//          Mar 04/14 - gets return error, remove getErr
//			Mar 17/14 - getStatus, setStatus isolated block versions
//          Feb 18/15 - DS1340 version derived from DS3231

// Implementation for DS1340, I2C, temp compensated, clock IC
// The DS1340 does not have alarms or memory. The alarm access functions are
// nevertheless kept as stubs so that clock.h remains identical for all libraries,
// and against the possible implementation of alarms in some other way.

#ifndef DS1340_HDR
#define DS1340_HDR

#include "clock.h"
#include "Wire.h"

// datasheet bit name definitions
#define CEB 7	//century enable, in date.mo byte
#define CB 6	//century bit, in date.mo byte, 
                // if CEB=1, CB toggles when year goes 99-->00
// no alarms in DS1340
// control/status
#define EOSC 7  //in seconds register 0 (pwrOn) --> osc runs
#define OUT 7	//in Control 0x07
#define FT 6	//  freq test
#define S 5		//  sign of calib offset
#define CAL4 4
#define CAL3 3
#define CAL2 2
#define CAL1 1
#define CAL0 0
#define TCS3 7	//trickle charger 0x08
#define TCS2 6
#define TCS1 5
#define TCS0 4
#define DS1 3
#define DS0 2
#define ROUT1 1
#define ROUT0 0
#define OSF 7	//only bit in flag reg 0x09

#define DSSTATUS 0x07

class DS1340 : public clock {

public:

	DS1340( );

	byte getTime( Time & );
	byte getDate( Date & );
	byte getStatus( byte, byte, byte * );	//array: adr, count, array destination
	byte getStatus( byte, byte * );			//single-byte: adr, destination for status byte
	byte getAlarm( byte almNr, byte * );
	byte setTime( Time &setT );
	byte setDate( Date &setD );
	byte setStatus( byte, byte, byte *setS );	//array: adr, count, array source
	byte setStatus( byte, byte );				//single-byte, adr, value
	byte setAlarm( byte almNr, byte *setA );

//	byte getErr( );				// enable for debugging; not otherwise needed


private:
	static const byte NRREG = 10;		//number of clock chip's registers
	static const byte I2Cadr = 0x68;	//7-bit I2C address
	byte errCode;

//	byte dsRegs[NRREG];		// array to hold copy of clock internal values (simulate I2C access)

//	byte writeClock( byte regadr, byte count, byte *regs );
	void writeClock( byte regadr );
//	byte readClock( byte regadr, byte count, byte *regs );
	byte readClock( byte regadr, byte count );
	static const byte dsDate = 0x03;	//offset to start of Date register block
	static const byte dsStatus = 0x07;	//offset to start of Status regs (calib,trickle,flag)
//	static const byte dsAlarm = 0x07;	//offset to start of alarm registers
	static const byte dsTime = 0x00;	//offset to start of time registers
//	static const byte dsTemp = 0x11;	//offset to start of temp in status

}; // class DS1340

#endif


