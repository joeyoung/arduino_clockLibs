// PCF8563.h - header for PCF8563 chip version
// I2C clock access package

// started: Oct 28, 2013 G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar 08/14 - add psStatush for high status bytes, bit names
// 			Mar 18/14 - modify getStatus setStatus for single byte, block access


// Implementation for PCF8563, I2C, clock IC (no temp compensation)

#ifndef PCF8563_HDR
#define PCF8563_HDR

#include "clock.h"
#include "Wire.h"

// data sheet bit names for PCF8563 status, alarm registers

// control_status_1 register address 0x00 (byte 0 from getStatus)
#define TEST1 7
#define STOP 6
#define TESTC 3
// control_status_2 register address 0x01 (byte 1 from getStatus)
#define TI_TP 4
#define AF 3
#define TF 2
#define AIE 1
#define TIE 0
// CLKOUT_Control register address 0x0D (byte 2 from getStatus)
#define FE 7
#define FD1 1
#define FD0 0
// Timer_control register address 0x0E (byte 3 from getStatus; byte 4 is timer value)
#define TE 7
#define TD1 1
#define TD0 0
// alarm registers address 0x09..0x0C
#define AE 7	// AE_M, AE_H, AE_D, AE_W all in same bit position
#define AE_M 7
#define AE_H 7
#define AE_D 7
#define AE_W 7
// misc - VL is in seconds time reg, century in year time reg
#define VL 7
#define CN

#define PCSTATUS 0
#define PCSTATMR 0x0e
#define ALARM_SIZE 4

class PCF8563 : public clock {

public:

	PCF8563( );

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
	static const byte NRREG = 16;		//number of clock chip's registers
	static const byte I2Cadr = 0x51;	//7-bit I2C address
	byte errCode;

//	byte pcRegs[NRREG];		// array to hold copy of clock internal values
	void writeClock( byte regadr );
//	byte writeClock( byte I2Cadr, byte count, byte *regs );
	byte readClock( byte regadr, byte count );
//	byte readClock( byte I2Cadr, byte count, byte *regs );
	static const byte pcDate = 0x05;	//offset to start of Date register block
//	static const byte pcStatus = 0x00;	//offset to start of Status registers
//	static const byte pcStatush = 0x0D;	//offset to start of high 3 Status registers
	static const byte pcAlarm = 0x09;	//offset to start of alarm registers
	static const byte pcTime = 0x02;	//offset to start of time registers


}; // class PCF8563

#endif


