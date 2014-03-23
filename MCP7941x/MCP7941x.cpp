// MCP7941x.cpp MCP7941x version of
// I2C clock access package

// started: Oct 28, 2013 G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar  5/14 - incorporate features from DS3231
// 			Mar 18/14 - modify getStatus setStatus for single byte, block access
//			Mar 22/14 - MCP7941x version


// Implementation for Microchip MCP7941x, I2C, clock IC

#include "MCP7941x.h"

MCP7941x::MCP7941x( ) {
	errCode = 0;
} // constructor


byte MCP7941x::getTime( Time &mct ) {
	byte err = readClock(  mcTime, 3 );	// request time
	if( err != 3 ) {
		errCode = RTC_TM_RD_ERR;
		return errCode;
	} else {
		mct.sec = Wire.read( )&0x7f;
		mct.min = Wire.read( );
		mct.hr = Wire.read( );
		return RTC_OK;
	} // if err
} // getTime


byte MCP7941x::getDate( Date &mcd ) {
	byte err = readClock(  mcDate, 4 );	// request date
	if( err != 4 ) {
		errCode = RTC_DT_RD_ERR;
		return errCode;
	} else {
		mcd.dow = Wire.read( )&0x07;
		mcd.dom = Wire.read( );
		mcd.mo = Wire.read( );
		mcd.yr = Wire.read( );
		return RTC_OK;
	} // if err
} // getDate


byte MCP7941x::getStatus( byte regadr, byte cnt, byte *ckS ) {
	byte err = readClock( regadr, cnt );	// request cnt status bytes
	if( err != cnt ) {
		errCode = RTC_ST_RD_ERR;
		return errCode;
	} else {
		for( byte ix = 0; ix < cnt; ix++ ) *(ckS+ix) = Wire.read( );
	}
	return RTC_OK;
} // getStatus - block


byte MCP7941x::getStatus( byte regadr, byte *val ) {
	byte err = readClock( regadr, 1 );		//request one status byte
	if( err != 1 ) {
		errCode = RTC_ST_RD_ERR;
		return errCode;
	} else {
		*val = Wire.read( );
	}
	return RTC_OK;
} // getStatus - single byte


byte MCP7941x::getAlarm( byte almNr, byte *ckA ) {
	byte err = readClock( mcAlarm+almNr*7, 6 );
	if( err != 6 ) {
		errCode = RTC_AL_RD_ERR;
		return errCode;
	} else {
		for( byte ix = 0; ix < 6; ix++ ) *(ckA+ix) = Wire.read( );
		return RTC_OK;
	}
} // getAlarm


byte MCP7941x::setTime( Time &setT ) {
	writeClock( mcTime );	// setup writing to time registers
	Wire.write( setT.sec | 0x80 );	// fill in the time values
	Wire.write( setT.min );
	Wire.write( setT.hr );
	errCode = Wire.endTransmission( );	// send to chip
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_TM_WR_ERR;
	}
} // setTime

byte MCP7941x::setDate( Date &setD ) {
	writeClock( mcDate );	// setup writing to date registers
	Wire.write( setD.dow );	// fill in date values
	Wire.write( setD.dom );
	Wire.write( setD.mo );
	Wire.write( setD.yr );
	errCode = Wire.endTransmission( );	//send to chip
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_DT_WR_ERR;
	}
} // setDate


byte MCP7941x::setStatus( byte regadr, byte cnt, byte *setS ) {
	writeClock( regadr );
	for( byte ix = 0; ix < cnt; ix++ ) Wire.write( setS[ix] );
	errCode = Wire.endTransmission( );
	if( errCode != 0 ) return RTC_ST_WR_ERR;
	return RTC_OK;
} // setStatus - block


byte MCP7941x::setStatus( byte regadr, byte val ) {
	writeClock( regadr );
	Wire.write( val );
	errCode = Wire.endTransmission( );
	if( errCode != 0 ) return RTC_ST_WR_ERR;
	return RTC_OK;
} // setStatus - single byte


byte MCP7941x::setAlarm( byte almNr, byte *setA ) {
	writeClock( mcAlarm+almNr*7 );
	for( byte ix = 0; ix < 6; ix++ ) Wire.write( setA[ix] );
	errCode = Wire.endTransmission( );
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_AL_WR_ERR;
	}
} // setAlarm


void MCP7941x::writeClock( byte regadr ) {
	Wire.beginTransmission( I2Cadr );	// start write with addr
	Wire.write( regadr );				// then start of block register adr
} // I2C write block - beginning of Wire operation


byte MCP7941x::readClock( byte regadr, byte count ) {
  Wire.beginTransmission( I2Cadr ); 	// start write to slave
  Wire.write( regadr );     // set chip's adr pointer for this read
  Wire.endTransmission();
  return( Wire.requestFrom( I2Cadr, count ) );
} // I2C read block


