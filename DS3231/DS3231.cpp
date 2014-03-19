// DS3231.cpp -  DS3231 version
// I2C clock access package

// started: Oct 22, 2013 G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Nov 2/13 - rearrange so base has storage
//			Feb 25/14 - Arduino version started
//          Mar  4/14 - gets return errCode, remove itermediate storage
//			Mar 17/14 - revised getStatus, setStatus


// Implementation for DS3231, I2C, temp compensated, clock IC

#include "DS3231.h"

DS3231::DS3231( ) {
	errCode = 0;
} // constructor


byte DS3231::getTime( Time &dst ) {
	byte err = readClock(  dsTime, 3 );	// request time
	if( err != 3 ) {
		errCode = RTC_TM_RD_ERR;
		return errCode;
	} else {
		dst.sec = Wire.read( );
		dst.min = Wire.read( );
		dst.hr = Wire.read( );
		return RTC_OK;
	} // if err
} // getTime


byte DS3231::getDate( Date &dsd ) {
	byte err = readClock(  dsDate, 4 );	// request date
	if( err != 4 ) {
		errCode = RTC_DT_RD_ERR;
		return errCode;
	} else {
		dsd.dow = Wire.read( );
		dsd.dom = Wire.read( );
		dsd.mo = Wire.read( );
		dsd.yr = Wire.read( );
		return RTC_OK;
	} // if err
} // getDate

byte DS3231::getStatus( byte regadr, byte cnt, byte *ckS ) {
	byte err = readClock( regadr, cnt );	// request cnt status bytes
	if( err != cnt ) {
		errCode = RTC_ST_RD_ERR;
		return errCode;
	} else {
		for( byte ix = 0; ix < cnt; ix++ ) *(ckS+ix) = Wire.read( );
	}
	return RTC_OK;
} // getStatus - array


byte DS3231::getStatus( byte regadr, byte *ckS ) {
	byte err = readClock( regadr, 1 );		//request single byte
	if( err != 1 ) {
		errCode = RTC_ST_RD_ERR;
		return errCode;
	} else {
		*ckS = Wire.read( );
	}
	return RTC_OK;
} // getStatus - single byte


byte DS3231::getAlarm( byte almNr, byte *ckA ) {
	byte nrbytes = 3;
	if( almNr == 0 ) nrbytes = 4;
	byte err = readClock( dsAlarm+almNr*4, nrbytes );
	if( err != nrbytes ) {
		errCode = RTC_AL_RD_ERR;
		return errCode;
	} else {
		for( byte ix = 0; ix < nrbytes; ix++ ) *(ckA+ix) = Wire.read( );
		return RTC_OK;
	}
} // getAlarm


byte DS3231::setTime( Time &setT ) {
	writeClock( dsTime );	// setup writing to time registers
	Wire.write( setT.sec );
	Wire.write( setT.min );
	Wire.write( setT.hr );
	errCode = Wire.endTransmission( );
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_TM_WR_ERR;
	}
} // setTime


byte DS3231::setDate( Date &setD ) {
	writeClock( dsDate );	// setup writing to date registers
	Wire.write( setD.dow );
	Wire.write( setD.dom );
	Wire.write( setD.mo );
	Wire.write( setD.yr );
	errCode = Wire.endTransmission( );
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_DT_WR_ERR;
	}
} // setDate


byte DS3231::setStatus( byte regadr, byte cnt, byte *setS ) {
	writeClock( regadr );
	for( byte ix = 0; ix < cnt; ix++ ) Wire.write( setS[ix] );
	errCode = Wire.endTransmission( );
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_ST_WR_ERR;
	}
} // setStatus - array of bytes


byte DS3231::setStatus( byte regadr, byte val ) {
	writeClock( regadr );
	Wire.write( val );
	errCode = Wire.endTransmission( );
	if( errCode != 0 ) return RTC_ST_WR_ERR;
	return RTC_OK;
} // setStatus - single byte


byte DS3231::setAlarm( byte almNr, byte *setA ) {
	writeClock( dsAlarm+4*almNr );
	if( almNr == 0 ) for( byte ix = 0; ix < 4; ix++ ) Wire.write( setA[ix] );
	if( almNr == 1 ) for( byte ix = 0; ix < 3; ix++ ) Wire.write( setA[ix] );
	errCode = Wire.endTransmission( );
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_AL_WR_ERR;
	}
} // setAlarm

void DS3231::writeClock( byte regadr ) {
	Wire.beginTransmission( I2Cadr );	// start write with addr
	Wire.write( regadr );				// then start of block register adr
} // I2C write block - beginning of Wire operation


byte DS3231::readClock( byte regadr, byte count ) {
  Wire.beginTransmission( I2Cadr ); 	// start write to slave
  Wire.write( regadr );     // set chip's adr pointer for this read
  Wire.endTransmission();
  return( Wire.requestFrom( I2Cadr, count ) );
} // I2C read block

// debug access for errCode
//byte DS3231::getErr( ) {
//	return errCode;
//} 

