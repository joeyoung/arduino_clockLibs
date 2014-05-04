arduino clock libraries

A set of similar access functions for several I2C real-time clock IC's.

The goal is to hide the variations of different RTC's register layout and
access methods so that the same format of calling functions exists for the
arduino programmer's use. This goal is reasonably complete for getTime,
getDate (and corresponding setTime, setDate). The various chip's status,
control, and alarm functions are sufficiently arcane that the access
functions still require detailed knowledge of the various bit's function(s).

Example sketches tstClock, setClock, and clockDisp, identical except for an
#include line, illustrate the libraries' use. 

The clock chips available (so Far) are:
 DS3231
 PCF8563
 RV3029
 MCP7941x - April 9/14
 DS1307
 DS1302 - May 3/14

A subset of access functions to use time from gps NMEA $GPRMC sentences:
 gpsClock - April 24/14

An example sketch in the gpsClock library shows a rudimentary method of 
setting the date and time of the IC clocks from the gps time.


