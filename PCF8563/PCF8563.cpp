// PCF8563.cpp PCF8563 version of
// I2C clock access package

// started: Oct 28, 2013 G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar  5/14 - incorporate features from DS3231
// 			Mar 18/14 - modify getStatus setStatus for single byte, block access


// Implementation for PCF8563, I2C, clock IC

#include "PCF8563.h"

PCF8563::PCF8563( ) {
	errCode = 0;
} // constructor


byte PCF8563::getTime( Time &pct ) {
	byte err = readClock(  pcTime, 3 );	// request time
	if( err != 3 ) {
		errCode = RTC_TM_RD_ERR;
		return errCode;
	} else {
		pct.sec = Wire.read( );
		pct.min = Wire.read( );
		pct.hr = Wire.read( );
		return RTC_OK;
	} // if err
} // getTime


byte PCF8563::getDate( Date &pcd ) {
	byte err = readClock(  pcDate, 4 );	// request date
	if( err != 4 ) {
		errCode = RTC_DT_RD_ERR;
		return errCode;
	} else {
		pcd.dom = Wire.read( );
		pcd.dow = Wire.read( )+1;	// make 0..6 into 1..7
		pcd.mo = Wire.read( );
		pcd.yr = Wire.read( );
		return RTC_OK;
	} // if err
} // getDate


byte PCF8563::getStatus( byte regadr, byte cnt, byte *ckS ) {
	byte err = readClock( regadr, cnt );	// request cnt status bytes
	if( err != cnt ) {
		errCode = RTC_ST_RD_ERR;
		return errCode;
	} else {
		for( byte ix = 0; ix < cnt; ix++ ) *(ckS+ix) = Wire.read( );
	}
	if( regadr == PCSTATUS ) ckS[0] &= 0b10101000;		// mask off meaningless, variable bits
	if( cnt > 1 && regadr == 0 ) ckS[1] &= 0b00011111;
	if( regadr == PCSTATMR ) ckS[0] &= 0b10000011;		// TE, TD1, TD0
	if( regadr == 0x0D ) ckS[0] &= 0b10000011;		// keep FE, FD1, FD0
	return RTC_OK;
} // getStatus - block


byte PCF8563::getStatus( byte regadr, byte *val ) {
	byte err = readClock( regadr, 1 );		//request one status byte
	if( err != 1 ) {
		errCode = RTC_ST_RD_ERR;
		return errCode;
	} else {
		*val = Wire.read( );
	}
	if( regadr == PCSTATUS ) *val &= 0b10101000;
	if( regadr == (PCSTATUS+1) ) *val &= 0b00011111;
	if( regadr == PCSTATMR ) *val &= 0b10000011;		// TE, TD1, TD0
	if( regadr == 0x0D ) *val &= 0b10000011;		// keep FE, FD1, FD0
	return RTC_OK;
} // getStatus - single byte


byte PCF8563::getAlarm( byte almNr, byte *ckA ) {
	byte err = readClock( pcAlarm, 3 );		//ignore almNr
	if( err != 3 ) {
		errCode = RTC_AL_RD_ERR;
		return errCode;
	} else {
		for( byte ix = 0; ix < 3; ix++ ) *(ckA+ix) = Wire.read( );
		return RTC_OK;
	}
} // getAlarm


byte PCF8563::setTime( Time &setT ) {
	writeClock( pcTime );	// setup writing to time registers
	Wire.write( setT.sec );	// fill in the time values
	Wire.write( setT.min );
	Wire.write( setT.hr );
	errCode = Wire.endTransmission( );	// send to chip
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_TM_WR_ERR;
	}
} // setTime

byte PCF8563::setDate( Date &setD ) {
	writeClock( pcDate );	// setup writing to date registers
	Wire.write( setD.dom );	// fill in date values
	Wire.write( setD.dow );
	Wire.write( setD.mo );
	Wire.write( setD.yr );
	errCode = Wire.endTransmission( );	//send to chip
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_DT_WR_ERR;
	}
} // setDate


byte PCF8563::setStatus( byte regadr, byte cnt, byte *setS ) {
	if( regadr == PCSTATUS ) {
		setS[0] &= (1<<TEST1) | (1<<STOP) | (1<<TESTC);		// enforce data sheet requirement
		if( cnt > 1 ) {
			setS[1] &= (1<<TI_TP) | (1<<AF) | (1<<TF) | (1<<AIE) | (1<<TIE);
		}
	}
	writeClock( regadr );
	for( byte ix = 0; ix < cnt; ix++ ) Wire.write( setS[ix] );
	errCode = Wire.endTransmission( );
	if( errCode != 0 ) return RTC_ST_WR_ERR;
	return RTC_OK;
} // setStatus - block


byte PCF8563::setStatus( byte regadr, byte val ) {
	if( regadr == PCSTATUS ) val &= (1<<TEST1) | (1<<STOP) | (1<<TESTC);		// enforce data sheet requirement
	if( regadr == PCSTATUS+1 ) val &= (1<<TI_TP) | (1<<AF) | (1<<TF) | (1<<AIE) | (1<<TIE);
	writeClock( regadr );
	Wire.write( val );
	errCode = Wire.endTransmission( );
	if( errCode != 0 ) return RTC_ST_WR_ERR;
	return RTC_OK;
} // setStatus - single byte


byte PCF8563::setAlarm( byte almNr, byte *setA ) {
	writeClock( pcAlarm );
	for( byte ix = 0; ix < 3; ix++ ) Wire.write( setA[ix] );
	errCode = Wire.endTransmission( );
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_AL_WR_ERR;
	}
} // setAlarm


void PCF8563::writeClock( byte regadr ) {
	Wire.beginTransmission( I2Cadr );	// start write with addr
	Wire.write( regadr );				// then start of block register adr
} // I2C write block - beginning of Wire operation


byte PCF8563::readClock( byte regadr, byte count ) {
  Wire.beginTransmission( I2Cadr ); 	// start write to slave
  Wire.write( regadr );     // set chip's adr pointer for this read
  Wire.endTransmission();
  return( Wire.requestFrom( I2Cadr, count ) );
} // I2C read block


