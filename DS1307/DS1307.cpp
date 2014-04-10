// DS1307.cpp DS1307 version of
// I2C clock access package

// started: Oct 28, 2013 G. D. (Joe) Young <jyoung@islandnet.com>

// revised: Mar  5/14 - incorporate features from DS3231
// 			Mar 18/14 - modify getStatus setStatus for single byte, block access
//			Mar 22/14 - MCP7941x version
//			Mar 25/14 - DS1307 version. Use RWM for 'software alarm(s)'
//			Apr 08/14 - move timer to sketch for multiple alarm support


// Implementation for Maxim(formerly Dallas) DS1307, I2C, clock IC

#include "DS1307.h"

DS1307::DS1307( ) {
	errCode = 0;
} // constructor


byte DS1307::getTime( Time &ds7t ) {
	byte err = readClock(  ds7Time, 3 );	// request time
	if( err != 3 ) {
		errCode = RTC_TM_RD_ERR;
		return errCode;
	} else {
		ds7t.sec = Wire.read( )&0x7f;
		ds7t.min = Wire.read( );
		ds7t.hr = Wire.read( );
		return RTC_OK;
	} // if err
} // getTime


byte DS1307::getDate( Date &ds7d ) {
	byte err = readClock(  ds7Date, 4 );	// request date
	if( err != 4 ) {
		errCode = RTC_DT_RD_ERR;
		return errCode;
	} else {
		ds7d.dow = Wire.read( );
		ds7d.dom = Wire.read( );
		ds7d.mo = Wire.read( );
		ds7d.yr = Wire.read( );
		return RTC_OK;
	} // if err
} // getDate


byte DS1307::getStatus( byte regadr, byte cnt, byte *ckS ) {
	byte err = readClock( regadr, cnt );	// request cnt status bytes
	if( err != cnt ) {
		errCode = RTC_ST_RD_ERR;
		return errCode;
	} else {
		for( byte ix = 0; ix < cnt; ix++ ) *(ckS+ix) = Wire.read( );
	}
	return RTC_OK;
} // getStatus - block


byte DS1307::getStatus( byte regadr, byte *val ) {
	byte err = readClock( regadr, 1 );		//request one status byte
	if( err != 1 ) {
		errCode = RTC_ST_RD_ERR;
		return errCode;
	} else {
		*val = Wire.read( );
	}
	return RTC_OK;
} // getStatus - single byte


byte DS1307::getAlarm( byte almNr, byte *ckA ) {
	byte err = readClock( ds7Alarm+almNr*ALARM_SIZE, ALARM_SIZE );
	if( err != ALARM_SIZE ) {
		errCode = RTC_AL_RD_ERR;
		return errCode;
	} else {
		for( byte ix = 0; ix < ALARM_SIZE; ix++ ) *(ckA+ix) = Wire.read( );
		return RTC_OK;
	}
} // getAlarm


byte DS1307::setTime( Time &setT ) {
	writeClock( ds7Time );	// setup writing to time registers
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

byte DS1307::setDate( Date &setD ) {
	writeClock( ds7Date );	// setup writing to date registers
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


byte DS1307::setStatus( byte regadr, byte cnt, byte *setS ) {
	writeClock( regadr );
	for( byte ix = 0; ix < cnt; ix++ ) Wire.write( setS[ix] );
	errCode = Wire.endTransmission( );
	if( errCode != 0 ) return RTC_ST_WR_ERR;
	return RTC_OK;
} // setStatus - block


byte DS1307::setStatus( byte regadr, byte val ) {
	writeClock( regadr );
	Wire.write( val );
	errCode = Wire.endTransmission( );
	if( errCode != 0 ) return RTC_ST_WR_ERR;
	return RTC_OK;
} // setStatus - single byte


byte DS1307::setAlarm( byte almNr, byte *setA ) {
	writeClock( ds7Alarm+almNr*ALARM_SIZE );
	for( byte ix = 0; ix < ALARM_SIZE; ix++ ) Wire.write( setA[ix] );
	errCode = Wire.endTransmission( );
	if( errCode == 0 ) {
		return RTC_OK;
	} else {
		return RTC_AL_WR_ERR;
	}
} // setAlarm


const byte as = ALARM_SIZE;
const byte alf = 2*as+1;	// alarm control flags at top of sketch array

bool DS1307::checkAlarm( byte almNr, byte *tm_al, unsigned long *timer ) {
	if( tm_al[alf] & 0x80 ) {			//setup timer on first time entry
		getStatus( ds7Time, as, tm_al ); 	//move time to sketch array, ..
		getStatus( ds7Alarm+almNr*as, as, (tm_al+as) );	//.. then this alarm, ..
		getStatus( DS7ALMCTL, (tm_al+2*as) );	// .. and alarm flags from IC
		if( tm_al[as+1] & (1<<DSAE) ) tm_al[alf] |= 0x42;	//indicate minute alarm set
		if( tm_al[as+2] & (1<<DSAE) ) tm_al[alf] |= 0x44;	//indicate hour alarm set
		if( tm_al[as+3] & (1<<DSAE) ) tm_al[alf] |= 0x48;	//indicate dow alarm set
		if( tm_al[as] & (1<<DSAE) ) {
			int sectoalarm = ( (int)( ((tm_al[as]&0x70)>>4)*10+(tm_al[as]&0xf) )
							 - (int)( ((tm_al[0]&0x70)>>4)*10+(tm_al[0]&0xf) ) );
			if( sectoalarm <= 0 ) {
				timer[almNr] = millis( ) + (unsigned long)((60 + sectoalarm))*1000;
			} else {
				timer[almNr] = millis( ) + (unsigned long)(sectoalarm)*1000;
			} // if time already passed
			tm_al[alf] |= 0x41;	// indicate alarm set
		} else {
			if( tm_al[alf] & 0x0e ) {	// means sec not set, but some other is
				int sectoalarm = ( (int)( 60 )
							 - (int)( ((tm_al[0]&0x70)>>4)*10+(tm_al[0]&0xf) ) );
				timer[almNr] = millis( ) + (unsigned long)( sectoalarm )*1000;	// set timer to next min
			} else {
				tm_al[alf] &= 0xb0;		// alarm not set
			}
		} // if seconds alarm enabled
		tm_al[alf] &= 0x7f;	// clear first-entry
	} else {
		if( ( tm_al[alf] & 0x40 )  && ( timer[almNr] < millis( ) ) ) {
			getStatus( DS7ALMCTL, (tm_al+2*as) );	// read current alarm flags from IC
			if( (tm_al[alf] & 0x0e) == 0 ) {
				tm_al[as+as] |= (1<<almNr);
				setStatus( DS7ALMCTL, tm_al[as+as] );	// put alarm flag to control register
				tm_al[alf] |= 0x80;	// prepare setup for next alarm
				tm_al[alf] &= 0xb0;
				return true;
			} else {
				getStatus( ds7Time, as, tm_al ); 	//move time to sketch array, ..
				getStatus( ds7Alarm+almNr*as, as, (tm_al+as) );	//.. then this alarm, ..
				bool test;				// check to see if all enabled conditions match
				if( tm_al[alf] & 0x02 ) test = ( (tm_al[1] & 0x7f) == (tm_al[as+1] & 0x7f) );	// minutes
				if( tm_al[alf] & 0x04 ) test &= ( (tm_al[2] & 0x3f) == (tm_al[as+2] & 0x3f) );	// hours
				if( tm_al[alf] & 0x08 ) test &= ( (tm_al[3] & 0x07) == (tm_al[as+3] & 0x07) );	// dow
				if( test ) {
					tm_al[as+as] |= (1<<almNr);
					setStatus( DS7ALMCTL, tm_al[as+as] );	// put alarm flag to control register
					tm_al[alf] |= 0x80;	// prepare setup for next alarm
					tm_al[alf] &= 0xb0;
					return true;
				} else {
					timer[almNr] += 60000;		// wait another minute
				}
			} // if only sec set
		} // if checking alarm
	} // if first entry
	return false;
} // check alarm - call from sketch loop( )


void DS1307::writeClock( byte regadr ) {
	Wire.beginTransmission( I2Cadr );	// start write with addr
	Wire.write( regadr );				// then start of block register adr
} // I2C write block - beginning of Wire operation


byte DS1307::readClock( byte regadr, byte count ) {
  Wire.beginTransmission( I2Cadr ); 	// start write to slave
  Wire.write( regadr );     // set chip's adr pointer for this read
  Wire.endTransmission();
  return( Wire.requestFrom( I2Cadr, count ) );
} // I2C read block


