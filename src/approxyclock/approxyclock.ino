/*

  ApproxyClock with RGB LEDs
  (C) 2015 Richel Bilderbeek

2015-03-21: v.1.0: Initial version
2015-03-22: v.1.1: use of Time library
2015-03-22: v.1.2: added parallel display of rainbow format
2015-03-22: v.1.3: can set the (approximate) time using capacitive sensors
2015-03-22: v.1.4: show rainbow time while setting the clock
2015-03-22: v.1.5: bugfix in rainbow time
2015-04-01: v.1.6: capacitive sensor threshold is changed according to use of battery or USB/adapter
2026-06-05: v.1.7: simplify

Original RGB LEDs:
    ___
   /   \
  |     |
  +-+-+-+
  | | | |
  | | | |
  | | |
    |
    
  1 2 3 4

1: Blue, connect with resistance of 1000 (brown-black-red-gold) to Arduino pin 3 (note: must be a PWM pin)
2: GND
3: Red, connect with resistance of 1000 (brown-black-red-gold) to Arduino pin 5 (note: must be a PWM pin)
4: Green, connect with resistance of 2200 (red-red-red-gold) to Arduino pin 6 (note: must be a PWM pin)

Rainbow RGB LEDs:
    ___
   /   \
  |     |
  +-+-+-+
  | | | |
  | | | |
  | | |
    |
    
  1 2 3 4

1: Blue, connect with resistance of 1000 (brown-black-red-gold) to Arduino pin 9 (note: must be a PWM pin)
2: GND
3: Red, connect with resistance of 1000 (brown-black-red-gold) to Arduino pin 10 (note: must be a PWM pin)
4: Green, connect with resistance of 2200 (red-red-red-gold) to Arduino pin 11 (note: must be a PWM pin)

*/

// Install the 'Time' library, by Paul Stoffregen,
// by, in the Arduino IDE click 'Tools | Manage Libraries',
// then install this 'Time' library
#include <TimeLib.h>

const int blue_original_pin = 3;
const int red_original_pin = 5;
const int green_original_pin = 6;

const int blue_rainbow_pin = 9;
const int red_rainbow_pin = 10;
const int green_rainbow_pin = 11;

/* When using the PCB print:
const int blue_original_pin  = 6;
const int red_original_pin   = 3;
const int green_original_pin = 5;

const int blue_rainbow_pin  = 11;
const int red_rainbow_pin   = 9;
const int green_rainbow_pin = 10;
*/

const int heartbeat_pin = 13;

void setup()
{
  pinMode(red_original_pin,OUTPUT);
  pinMode(green_original_pin,OUTPUT);
  pinMode(blue_original_pin,OUTPUT);
  pinMode(red_rainbow_pin,OUTPUT);
  pinMode(green_rainbow_pin,OUTPUT);
  pinMode(blue_rainbow_pin,OUTPUT);
  pinMode(heartbeat_pin,OUTPUT);
}

void loop() 
{
  int last_sec = -1; //The previous second, used to detect a change in time, to be sent to serial monitor
  while (1)
  {
    //Show the time
    const int s = second();
    const int m = minute();
    const int h = hour();

    digitalWrite(heartbeat_pin , s % 2 ? HIGH : LOW);

    if (last_sec == s) 
    {
      continue;
    }

    last_sec = s;
    ShowTimeOriginal(s,m,h);
    ShowTimeRainbow(s,m,h);
  }
}

///Show the time on all RGB LEDs
void ShowTimeOriginal(const int secs, const int mins, const int hours)
{
  const int max_brightness = 255;
  const int red_value   = map(secs,0,60,0,max_brightness);
  const int green_value = map(mins,0,60,0,max_brightness);
  const int blue_value  = map(hours,0,24,0,max_brightness);
  analogWrite(red_original_pin,red_value);
  analogWrite(green_original_pin,green_value);
  analogWrite(blue_original_pin,blue_value);
}

///Show the time on all RGB LEDs using rainbow format
// B     R     G     B 
// +     +     +     +
// |\   / \   / \   / 
// | \ /   \ /   \ /
// |  X     X     X
// | / \   / \   / \
// |/   \ /   \ /   \
// +-----+-----+-----+
// 0     0     1     2
// 0     8     6     4
void ShowTimeRainbow(const int secs, const int mins, const int hours)
{
  int red_value = 0;
  int green_value = 0;
  int blue_value = 0;
  if (hours < 8)
  {
    const double f = static_cast<double>((hours*60) + mins) / (8 * 60);
    green_value = static_cast<int>(f * 255.0);
    red_value = 255 - red_value;    
  }
  else if (hours < 16)
  {
    const double f = static_cast<double>(((hours-8)*60) + mins) / (8 * 60);
    blue_value = static_cast<int>(f * 255.0);
    green_value = 255 - blue_value;    
  }
  else if (hours < 24)
  {
    const double f = static_cast<double>(((hours-16)*60) + mins) / (8 * 60);
    red_value = static_cast<int>(f * 255.0);
    blue_value = 255 - red_value;
  }
  
  analogWrite(red_rainbow_pin,red_value);
  analogWrite(green_rainbow_pin,green_value);
  analogWrite(blue_rainbow_pin,blue_value);
}
