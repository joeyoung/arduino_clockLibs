// testing speed of digitalWrite

void setup( ) {
  pinMode( 12, OUTPUT );
}

void loop( ) {
 digitalWrite( 12, LOW );  // approx 4 usec
 delayMicroseconds( 1 );   // makes low slightly longer than high
 digitalWrite( 12, HIGH ); // approx 4.7 usec
}
