// RV3029.cpp RV3029 version of
// I2C clock access package

// started: March 11, 2014 G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar 17/14 - getStatus, setStatus for individual block access


// Implementation for RV3029, I2C, clock IC

#include "RV3029.h"

RV3029::RV3029( ) {
	errCode = 0;
} // constructor


byte RV3029::getTime( Time &rvt ) {
	byte err = readClock(  rvTime, 3 );	// request time
	if( err != 3 ) {
		errCode = RTC_TM_RD_ERR;
		return errCode;
	} else {
		rvt.sec = Wire.read( );
		rvt.min = Wire.read( );
		rvt.hr = Wire.read( );
		return RTC_OK;
	} // if err
} // getTime


byte RV3029::getDate( Date &rvd ) {
	byte err = readClock(  rvDate, 4 );	// request date
	if( err != 4 ) {
		errCode = RTC_DT_RD_ERR;
		return errCode;
	} else {
		rvd.dom = Wire.read( );
		rvd.dow = Wire.read( );	// 1..7
		rvd.mo = Wire.read( );
		rvd.yr = Wire.read( );
		return RTC_OK;
	} // if err
} // getDate


byte RV3029::getStatus( byte regadr, byte cnt, byte *ckS ) {
	byte err = readClock( regadr, cnt );	// request cnt status bytes from regadr on
	if( err != cnt ) {
		errCode = RTC_ST_RD_ERR;
		return errCode;
	} else {
		for( byte ix = 0; ix < cnt; ix++ ) *(ckS+ix) = Wire.read( );
	}
	return RTC_OK;
} // getStatus - array


byte RV3029::getStatus( byte regadr, byte *ckS ) {
	byte err = readClock( regadr, 1 );		// request selected byte
	if( err != 1 ) {
		errCode = RTC_ST_RD_ERR;
		return errCode;
	} else {
		*ckS = Wire.read( );
	}
	return RTC_OK;
} // getStatus - single byte


byte RV3029::getAlarm( byte almNr, byte *ckA ) {
	byte err = readClock( rvAlarm, 7 );		//ignore almNr
	if( err != 7 ) {
		errCode = RTC_AL_RD_ERR;
		return errCode;
	} else {
		for( byte ix = 0; ix < 7; ix++ ) *(ckA+ix) = Wire.read( );
		return RTC_OK;
	}
} // getAlarm


byte RV3029::setTime( Time &setT ) {
	writeClock( rvTime );	// setup writing to time registers
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

byte RV3029::setDate( Date &setD ) {
	writeClock( rvDate );	// setup writing to date registers
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


byte RV3029::setStatus( byte regadr, byte cnt, byte *setS ) {
	writeClock( regadr );
	for( byte ix = 0; ix < cnt; ix++ ) Wire.write( setS[ix] );
	errCode = Wire.endTransmission( );
	if( errCode != 0 ) return RTC_ST_WR_ERR;
	return RTC_OK;
} // setStatus - array to reg block


byte RV3029::setStatus( byte regadr, byte val ) {
	writeClock( regadr );
	Wire.write( val );
	errCode = Wire.endTransmission( );
	if( errCode != 0 ) return RTC_ST_WR_ERR;
	return RTC_OK;
} // setStatus - single byte


byte RV3029::setAlarm( byte almNr, byte *setA ) {
	writeClock( rvAlarm );
	for( byte ix = 0; ix < 7; ix++ ) Wire.write( setA[ix] );
	errCode = Wire.endTransmission( );
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_AL_WR_ERR;
	}
} // setAlarm


void RV3029::writeClock( byte regadr ) {
	Wire.beginTransmission( I2Cadr );	// start write with addr
	Wire.write( regadr );				// then start of block register adr
} // I2C write block - beginning of Wire operation


byte RV3029::readClock( byte regadr, byte count ) {
  Wire.beginTransmission( I2Cadr ); 	// start write to slave
  Wire.write( regadr );     // set chip's adr pointer for this read
  Wire.endTransmission();
  return( Wire.requestFrom( I2Cadr, count ) );
} // I2C read block


