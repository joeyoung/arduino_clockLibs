// DS1302.cpp - clockLibs format library for DS1302 RTC IC
//
// created: April 29, 2014  G. D. (Joe) Young <jyoung@islandnet.com>
//
// revised: May 2/14 - don't use block getStatus to copy clock, alarm in checkAlarm
//

#include "DS1302.h"

DS1302::DS1302( byte _ds2clk, byte _ds2data, byte _ds2ce ) {
	ds2clk = _ds2clk;
	ds2data = _ds2data;
	ds2ce = _ds2ce;
	pinMode( ds2ce, OUTPUT );
	digitalWrite( ds2ce, LOW );
	pinMode( ds2clk, OUTPUT );
	digitalWrite( ds2clk, LOW );
	errCode = 0;
} // constructor


byte DS1302::getTime( Time &ds2t ) {
	readClock( ds2Time, 3, allclock );
	ds2t.sec = allclock[0]&0x7f;		//mask off CH bit in secs
	ds2t.min = allclock[1];
	ds2t.hr = allclock[2];
	return RTC_OK;
} // getTime, time only

byte DS1302::getTime( Time &ds2t, Date &ds2d ) {
	readClock( ds2Time, 7, allclock );
	ds2t.sec = allclock[0]&0x7f;		//mask off CH bit in secs
	ds2t.min = allclock[1];
	ds2t.hr = allclock[2];
	ds2d.dow = allclock[5];
	ds2d.dom = allclock[3];
	ds2d.mo = allclock[4];
	ds2d.yr = allclock[6];
	return RTC_OK;
} // getTime, both

byte DS1302::getDate( Date &ds2d ) {
	readClock( ds2Time, 7, allclock );		//have to read all the clock
	ds2d.dow = allclock[5];
	ds2d.dom = allclock[3];
	ds2d.mo = allclock[4];
	ds2d.yr = allclock[6];
	return RTC_OK;
} // getDate


byte DS1302::getStatus( byte regadr, byte cnt, byte *ckS ) {
	readClock( regadr, cnt, ckS );	// request cnt status bytes
	return RTC_OK;
} // getStatus - block


byte DS1302::getStatus( byte regadr, byte *val ) {
	*val = readClock( regadr );		//request one status byte
	return RTC_OK;
} // getStatus - single byte


byte DS1302::getAlarm( byte almNr, byte *ckA ) {
	for( byte ix = 0; ix < ALARM_SIZE; ix++ ) {
		ckA[ix] = readClock( (byte)(ds2Alarm+almNr*ALARM_SIZE) + ix );
	}
	return RTC_OK;
} // getAlarm


byte DS1302::setTime( Time &setT ) {
	writeClock( ds2Time, setT.sec );
	writeClock( ds2Time+1, setT.min );
	writeClock( ds2Time+2, setT.hr );
	return RTC_OK;
} // setTime


byte DS1302::setTime( Time &setT, Date &setD ) {	//for burst write all clock regs
	allclock[0] = setT.sec;
	allclock[1] = setT.min;
	allclock[2] = setT.hr;
	allclock[3] = setD.dom;
	allclock[4] = setD.mo;
	allclock[5] = setD.dow;
	allclock[6] = setD.yr;
	writeClock( ds2Date, 7, allclock );
	return RTC_OK;
}


byte DS1302::setDate( Date &setD ) {
	writeClock( ds2Date, setD.dom );
	writeClock( ds2Date+1, setD.mo );
	writeClock( ds2Date+2, setD.dow );
	writeClock( ds2Date+3, setD.yr );
	return RTC_OK;
} // setDate


byte DS1302::setStatus( byte regadr, byte cnt, byte *setS ) {
	writeClock( regadr, cnt, setS );
	return RTC_OK;
} // setStatus - block


byte DS1302::setStatus( byte regadr, byte val ) {
	writeClock( regadr, val );
	return RTC_OK;
} // setStatus - single byte


byte DS1302::setAlarm( byte almNr, byte *setA ) {
	for( byte ix = 0; ix < ALARM_SIZE; ix++ ) {
		writeClock( (byte)(ds2Alarm+almNr*ALARM_SIZE)+ix, setA[ix] );
	}
	return RTC_OK;
} // setAlarm


const byte as = ALARM_SIZE;
const byte alf = 2*as+1;	// alarm control flags at top of sketch array

bool DS1302::checkAlarm( byte almNr, byte *tm_al, unsigned long *timer ) {
	if( tm_al[alf] & 0x80 ) {			//setup timer on first time entry
//		getStatus( ds2Time, as, tm_al ); 	//move time to sketch array, ..
//		getStatus( ds2Alarm+almNr*as, as, (tm_al+as) );	//.. then this alarm, ..
		readClock( ds2Time, 7, allclock );	// can't use separate status block reads
		for( byte ix=0; ix<as; ix++ ) {
			tm_al[ix] = allclock[ix];	// copy current time
			getStatus( ds2Alarm+almNr*as+ix, tm_al+ix+as );	//and selected alarm
		} // for alarm size
		getStatus( DS2ALMCTL, (tm_al+2*as) );	// .. and alarm flags from IC
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
			getStatus( DS2ALMCTL, (tm_al+2*as) );	// read current alarm flags from IC
			if( (tm_al[alf] & 0x0e) == 0 ) {
				tm_al[as+as] |= (1<<almNr);
				setStatus( DS2ALMCTL, tm_al[as+as] );	// put alarm flag to control register
				tm_al[alf] |= 0x80;	// prepare setup for next alarm
				tm_al[alf] &= 0xb0;
				return true;
			} else {
//				getStatus( ds2Time, as, tm_al ); 	//move time to sketch array, ..
//				getStatus( ds2Alarm+almNr*as, as, (tm_al+as) );	//.. then this alarm, ..
				readClock( ds2Time, 7, allclock );	// can't use separate status block reads
				for( byte ix=0; ix<as; ix++ ) {
					tm_al[ix] = allclock[ix];	// copy current time
					getStatus( ds2Alarm+almNr*as+ix, tm_al+ix+as );	//and selected alarm
				} // for alarm size
				bool test;				// check to see if all enabled conditions match
				if( tm_al[alf] & 0x02 ) test = ( (tm_al[1] & 0x7f) == (tm_al[as+1] & 0x7f) );	// minutes
				if( tm_al[alf] & 0x04 ) test &= ( (tm_al[2] & 0x3f) == (tm_al[as+2] & 0x3f) );	// hours
				if( tm_al[alf] & 0x08 ) test &= ( (tm_al[3] & 0x07) == (tm_al[as+3] & 0x07) );	// dow
				if( test ) {
					tm_al[as+as] |= (1<<almNr);
					setStatus( DS2ALMCTL, tm_al[as+as] );	// put alarm flag to control register
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


void DS1302::writeClock( byte regadr, byte count, byte *array ) {
	byte cmd = (regadr+baseadr+burstoff)<<1;	//7-bit address to write command 8-bit
// should verify that address is correct for burst operation
	writeCmd( cmd );

    for( byte tx = 0; tx < count; tx++ ) {
		byte val = array[tx];
		for( byte ix = 0; ix < 8; ix++ ) {
			digitalWrite( ds2data, val&0x01 );
			digitalWrite( ds2clk, HIGH );
			digitalWrite( ds2clk, LOW );
			val = val>>1;
		} // for 8 val bits
	} // for count bytes

    digitalWrite( ds2ce, LOW );

} // DS1302 write block


void DS1302::writeClock( byte regadr, byte val ) {
	byte cmd = (regadr+baseadr)<<1;		//7-bit address to write command 8-bit

	writeCmd( cmd );

	for( byte ix = 0; ix < 8; ix++ ) {
		digitalWrite( ds2data, val&0x01 );
		digitalWrite( ds2clk, HIGH );
		digitalWrite( ds2clk, LOW );
		val = val>>1;
	} // for 8 val bits

	digitalWrite( ds2ce, LOW );

} // DS1302 write single byte

void DS1302::readClock( byte regadr, byte count, byte *array ) {
	byte cmd = ( (regadr+baseadr+burstoff)<<1) | 1;	//7-bit address to read command 8-bit
// should verify that address is correct for burst operation
	writeCmd( cmd );
  
	pinMode( ds2data, INPUT );
  
	for( byte tx = 0; tx < count; tx++ ) {

		array[tx] = 0;
		for( byte ix = 0; ix < 8; ix++ ) {
			array[tx] |= digitalRead( ds2data ) << ix;
			digitalWrite( ds2clk, HIGH );
			digitalWrite( ds2clk, LOW );
		} // for 8 data bits

	} // for each burst byte
    
	digitalWrite( ds2ce, LOW );

} // DS1302 read block


byte DS1302::readClock( byte regadr ) {
	byte cmd = ( (regadr+baseadr)<<1) | 1;	//7-bit address to read command 8-bit
    
	writeCmd( cmd );

	pinMode( ds2data, INPUT );
  
	byte reg = 0;
	for( byte ix = 0; ix < 8; ix++ ) {
		reg |= digitalRead( ds2data ) << ix;
		digitalWrite( ds2clk, HIGH );
		digitalWrite( ds2clk, LOW );
	} // for data bits
      
	digitalWrite( ds2ce, LOW );

	return reg;

} // DS1302 read single register


void DS1302::writeCmd( byte cmd ) {
	pinMode( ds2data, OUTPUT );
	digitalWrite( ds2ce, HIGH );

	for( byte ix = 0; ix < 8; ix++ ) {
		digitalWrite( ds2data, cmd&0x01 );
		digitalWrite( ds2clk, HIGH );
		digitalWrite( ds2clk, LOW );
		cmd = cmd>>1;
	} // for 8 command bits
} // writeCmd( )

