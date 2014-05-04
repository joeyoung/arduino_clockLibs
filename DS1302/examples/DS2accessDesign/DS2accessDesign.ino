// read register for DS1302 RTC IC

// created: April 29, 2014  G. D. (Joe) Young <jyoung@islandnet.com>

// revised:


// See test sketch apr29a - max speed of digitalWrite( HIGH/LOW )
// is around 4 usec, well below datasheet minimum clock high/low
// duration spec (1 usec at minimum supply voltage). Consequently,
// these functions should be OK to clock the chip's serial I/O

// To read a register byte:
//  clock line low, then
//  Chip enable high, then
//  data direction out
//  loop: write next bit to data line  (LSB of command is first)
//  digitalWrite( clock, HIGH )
//  digitalWrite( clock, LOW )
//  goto loop 7 more times
//  switch data line direction to input
//  loopr: read data line for next bit  (LSB is first)
//  digitalWrite( clock, HIGH )
//  digitalWrite( clock, LOW )
//  goto loopr 7 more times

// Note that the LSB of the chip register value is placed on the
// data line right after clock line goes low from the final clocking
// of the command output. So you must read before clocking, and
// therefore the arduino library shiftIn( ) cannot be used.

byte clock = 8;  // three-wire connection to chip
byte data = 9;
byte ce = 10;

const byte command = 0b10000001;  //read the seconds register
byte secreg;

unsigned long timer;

void setup( ) {
  
  Serial.begin( 9600 );
  
  pinMode( ce, OUTPUT );
  digitalWrite( ce, LOW );
  pinMode( clock, OUTPUT );
  digitalWrite( clock, LOW );
  
  timer = millis( ) + 1000;    //setup 1/sec timing

} // setup( )

void loop( ) {

  if( timer < millis( ) ) {
    timer += 1000;
    
    
    secreg = 0;
    byte cmd = command;
    
    pinMode( data, OUTPUT );
    digitalWrite( ce, HIGH );
  
    for( byte ix = 0; ix < 8; ix++ ) {
      digitalWrite( data, cmd&0x01 );
      digitalWrite( clock, HIGH );
      digitalWrite( clock, LOW );
      cmd = cmd>>1;
    } // for command bits
  
    pinMode( data, INPUT );
  
    for( byte ix = 0; ix < 8; ix++ ) {
      secreg |= digitalRead( data ) << ix;
      digitalWrite( clock, HIGH );
      digitalWrite( clock, LOW );
    } // for data bits
    
    digitalWrite( ce, LOW );

    Serial.println( secreg, HEX );

  } // if second timer passed
  
} // loop( )
  
  

