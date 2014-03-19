// RV3029.h - header for RV3029 chip version
// I2C clock access package

// started: March 11, 2014 - G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar 17/14 - getStatus, setStatus for separate block access


// Implementation for RV3029, I2C, clock IC (no temp compensation)

#ifndef RV3029_HDR
#define RV3029_HDR

#include "clock.h"
#include "Wire.h"

// data sheet bit names for RV3029 status, alarm registers

// control_1 register address 0x00
#define RVSTAT0 0x00
#define CLK_INT 7
#define TD1 6
#define TD0 5
#define SRON 4
#define EERE 3
#define TAR 2
#define TE 1
#define WE 0
// control_INT register address 0x01
#define RVSTAT1 0x01
#define SRIE 4
#define V2IE 3
#define V1IE 2
#define TIE 1
#define AIE 0
// control_INT_FLAG register address 0x02
#define RVSTAT2 0x02
#define SRF 4
#define V2F 3
#define V1F 2
#define TF 1
#define AF 0
// control_status register address 0x3
#define RVSTAT3 0x03
#define EEBUSY 7
#define PON 5
#define SR 4
#define V2F 3
#define V1F 2
// control_reset register adress 0x4
#define RVSTAT4 0x04
#define SYSR 4
// Timer register address 0x18, low byte, 0x19 high byte
#define RVSTATMR 0x18
// Temperature register address 0x20
#define RVSTATMP 0x20
// alarm registers address 0x10..0x16
#define AE_S 7
#define AE_M 7
#define AE_H 7
#define AE_D 7
#define AE_W 7
#define AE_MO 7
#define AE_Y 7

// size of byte array to be used by getAlarm
#define ALARM_SIZE 7

#define RVSTATEEU 0x28
#define RVSTATEEC 0x30
#define RVSTATRWM 0x38


class RV3029 : public clock {

public:

	RV3029( );

	byte getTime( Time & );
	byte getDate( Date & );
	byte getStatus( byte regadr, byte cnt, byte * );	// block access
	byte getStatus( byte regadr, byte * );			// single-byte access
	byte getAlarm( byte almNr, byte * );
	byte setTime( Time &setT );
	byte setDate( Date &setD );
	byte setStatus( byte regadr, byte cnt, byte *setS );	// block write
	byte setStatus( byte regadr, byte val );				// write val to adr
	byte setAlarm( byte almNr, byte *setA );


private:
	static const byte I2Cadr = 0x56;	//7-bit I2C address
	byte errCode;

	void writeClock( byte regadr );		// regadr is the device's internal adr pointer
	byte readClock( byte regadr, byte count );
	static const byte rvTime = 0x08;	//offset to start of time registers
	static const byte rvDate = 0x0B;	//offset to start of Date register block
//	static const byte rvStatus = 0x00;	//offset to start of Status registers
//	static const byte rvStattmr = 0x18;	//offset to start of high 2 Status registers (timer)
//	static const byte rvStattmp = 0x20;	//offset to start of high 3 Status registers (temp)
//	static const byte rvStateeu = 0x28;	//offset to user eeprom
//	static const byte rvStateec = 0x30;	//offset to eeprom control block (xtal characteristics)
//	static const byte rvStatrwm = 0x38;	//offset to user ram
	static const byte rvAlarm = 0x10;	//offset to start of alarm registers


}; // class RV3029

#endif


