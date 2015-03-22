/*

  ApproxyClock with RGB LEDs
  (C) 2015 Richel Bilderbeek

2015-03-21: v.1.0: Initial version
2015-03-22: v.1.1: use of Time library
2015-03-22: v.1.2: added parallel display of rainbow format
2015-03-22: v.1.3: can set the (approximate) time using capacitive sensors
2015-03-22: v.1.4: show rainbow time while setting the clock

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


Left capacitive sensor:
  
    2              4 
    |  +--------+  |
    +--+   RH   +--+
    |  +--------+
    |
  +-+-+
  |   |
  |   |
  |RS |
  |   |
  +-+-+
    |
    |
    X
  
  2: sensor pin
  4: helper pin
  RH: 'resistance helper', resistance of at least 1 Mega-Ohm (brown-black-green-gold)
  RS: 'resistance sensor', resistance of 1 kOhm (brown-black-red-gold)
  X: place to touch wire

Right capacitive sensor:
  
    7              4 
    |  +--------+  |
    +--+   RH   +--+
    |  +--------+
    |
  +-+-+
  |   |
  |   |
  |RS |
  |   |
  +-+-+
    |
    |
    X
  
  7: sensor pin
  4: helper pin
  RH: 'resistance helper', resistance of at least 1 Mega-Ohm (brown-black-green-gold)
  RS: 'resistance sensor', resistance of 1 kOhm (brown-black-red-gold)
  X: place to touch wire

*/

#include <CapacitiveSensor.h>
#include <Time.h>

const int blue_original_pin = 3;
const int red_original_pin = 5;
const int green_original_pin = 6;

const int blue_rainbow_pin = 9;
const int red_rainbow_pin = 10;
const int green_rainbow_pin = 11;

const int pin_sensor1 =  2;
const int pin_helper1 =  4;
CapacitiveSensor sensor1 
  = CapacitiveSensor(pin_helper1,pin_sensor1);        

const int pin_sensor2 =  7;
const int pin_helper2 =  4;
CapacitiveSensor sensor2 
  = CapacitiveSensor(pin_helper2,pin_sensor2);        


const int error_pin = 13;

void OnError(const String& error_message)

{
  Serial.print("ERROR: ");  
  Serial.println(error_message);  
  while (1)
  {
    //Blink LED
    digitalWrite(error_pin,!digitalRead(error_pin));
    //Write to serial
    delay(1000);
  }
}

void TestTime()
{
  //  setTime(hours, minutes, seconds, days, months, years);
  const int hours = 11;
  const int minutes = 22;
  const int seconds = 33;
  const int days = 0;
  const int months = 0;
  const int years = 0;
  setTime(hours, minutes, seconds, days, months, years);
  if (hour() != hours) { OnError("TestTime #1"); }
  if (minute() != minutes) { OnError("TestTime #1"); }
  if (second() != seconds) { OnError("TestTime #1"); }
  setTime(0,0,0,0,0,0);
}

void setup()
{
  pinMode(red_original_pin,OUTPUT);
  pinMode(green_original_pin,OUTPUT);
  pinMode(blue_original_pin,OUTPUT);
  pinMode(red_rainbow_pin,OUTPUT);
  pinMode(green_rainbow_pin,OUTPUT);
  pinMode(blue_rainbow_pin,OUTPUT);
  pinMode(error_pin,OUTPUT);
  Serial.begin(9600); //Cannot be used: chip is used stand-alone
  #ifndef NDEBUG
  Serial.println("ApproxyClock v. 1.1 (debug version)");
  #else //NDEBUG
  Serial.println("ApproxyClock v. 1.1 (release version)");
  #endif //NDEBUG
  TestTime();
}

//0 = 00 : none pressed
//1 = 01 : right pressed
//2 = 10 : left pressed
//3 = 11 : both pressed
const int state_no_sensor_pressed    = 0; //00
const int state_right_sensor_pressed = 1; //01
const int state_left_sensor_pressed  = 2; //10
const int state_both_sensors_pressed = 3; //11

///Obtain the current state of both capacitive sensors
int GetSensors()
{
  //The higher 'samples' is set, the more accurate the sensors measure
  const int samples = 30;
  //Measure the capacitive sensors
  const int r1 = sensor1.capacitiveSensor(samples);
  const int r2 = sensor2.capacitiveSensor(samples);
  //The threshold value, which determines the sensitivity of the sensors
  // - too low: the program will think more often there is a touch, possibly even when you do not touch
  // - too high: the program will think less often there is a touch, possibly even when you do touch 
  const int threshold = 200;
  const int state =  (r1 >= threshold ? 2 : 0) + (r2 >= threshold ? 1 : 0);
  return state;
}

/// User can choose to set the hours
/// - left sensor: cancel
/// - right sensor: set hours
void SetHours()
{
  Serial.println("Setting hours");
  int h = hour();
  bool accept = false; //Accept editing the hours
  while (1)
  {
    //The line below is commented out, because the user must first accept to edit it
    //ShowTimeOriginal(0,0,h); //secs, mins, hours
    const int sensor_state = GetSensors();
    if (sensor_state == state_left_sensor_pressed)
    {
      //  setTime(hours, minutes, seconds, days, months, years);
      setTime(h,minute(),second(),0,0,0);
      break; 
    }
    if (sensor_state == state_right_sensor_pressed)
    {
      if (accept) { h = (h + 1) % 24; }
      else { accept = true; }
    }
    //This line is put here, because the user must first accept to edit the time
    ShowTimeOriginal(0,0,h); //secs, mins, hours
    ShowTimeRainbow(second(),minute(),h); //secs, mins, hours
    delay(100);
  }
  #ifndef NDEBUG
  if (h <  0) { OnError("h <  0, h = " + String(h)); }
  if (h > 23) { OnError("h > 23, h = " + String(h)); }
  #endif // NDEBUG
}

/// User can choose to set the minutes
/// - left sensor: cancel
/// - right sensor: set minutes
void SetMinutes()
{
  Serial.println("Setting minutes");
  int m = minute();
  bool accept = false; //Accept editing the minutes
  while (1)
  {
    ShowTimeOriginal(0,m,0); //secs, mins, hours
    ShowTimeRainbow(second(),m,hour()); //secs, mins, hours
    const int sensor_state = GetSensors();
    if (sensor_state == state_left_sensor_pressed)
    {
      //  setTime(hours, minutes, seconds, days, months, years);
      setTime(hour(),m,second(),0,0,0);
      break; 
    }
    if (sensor_state == state_right_sensor_pressed)
    {
      if (accept) { m = (m + 1) % 60; }
      else { accept = true; }
    }
    delay(100);
  }
  #ifndef NDEBUG
  if (m <  0) { OnError("m <  0, h = " + String(m)); }
  if (m > 59) { OnError("m > 59, h = " + String(m)); }
  #endif // NDEBUG
}

/// User can choose to set the seconds
/// - left sensor: cancel
/// - right sensor: set seconds
void SetSeconds()
{
  Serial.println("Setting seconds");
  int s = second();
  bool accept = false; //Accept editing the seconds
  while (1)
  {
    ShowTimeOriginal(s,0,0); //secs, mins, hours
    ShowTimeRainbow(s,minute(),hour()); //secs, mins, hours
    const int sensor_state = GetSensors();
    if (sensor_state == state_left_sensor_pressed)
    {
      //  setTime(hours, minutes, seconds, days, months, years);
      setTime(hour(),minute(),s,0,0,0);
      break; 
    }
    if (sensor_state == state_right_sensor_pressed)
    {
      if (accept) { s = (s + 1) % 60; }
      else { accept = true; }
    }
    delay(100);
  }
  #ifndef NDEBUG
  if (s <  0) { OnError("s <  0, h = " + String(s)); }
  if (s > 59) { OnError("s > 59, h = " + String(s)); }
  #endif // NDEBUG
}


/// User can choose to set the time
/// - left sensor: select hours, minutes, seconds, cancel
/// - right sensor: change hours, minutes, seconds
void SetTime()
{
  Serial.println("Setting time");
  //The user can choose to set the hours, make the hours blink
  while (1)
  {
    //Blink hours
    ShowTimeOriginal(0,0,23); //Secs, mins, hours
    delay(100);
    ShowTimeOriginal(0,0,0); //Secs, mins, hours
    delay(100);
    const int sensor_state = GetSensors();
    if (sensor_state == state_right_sensor_pressed)
    {
      SetHours();
      break;
    }
    if (sensor_state == state_left_sensor_pressed)
    {
      break; 
    }
  }  
  //The user can choose to set the minutes, make the minutes blink
  while (1)
  {
    //Blink minutes
    ShowTimeOriginal(0,59,0); //Secs, mins, hours
    delay(100);
    ShowTimeOriginal(0,0,0); //Secs, mins, hours
    delay(100);
    const int sensor_state = GetSensors();
    if (sensor_state == state_right_sensor_pressed)
    {
      SetMinutes();
      break;
    }
    if (sensor_state == state_left_sensor_pressed)
    {
      break; 
    }
  }  
  //The user can choose to set the seconds, make the seconds blink
  while (1)
  {
    //Blink second
    ShowTimeOriginal(59,0,0); //Secs, mins, hours
    delay(100);
    ShowTimeOriginal(0,0,0); //Secs, mins, hours
    delay(100);
    const int sensor_state = GetSensors();
    if (sensor_state == state_right_sensor_pressed)
    {
      SetSeconds();
      break;
    }
    if (sensor_state == state_left_sensor_pressed)
    {
      break; 
    }
  }  
}

void SetTimeFromSerial()
{
  //Serial.println("Start of SetTimeFromSerial");
  const int  h  = Serial.available() ? Serial.parseInt() : -1;
  delay(10);
  const char c1 = Serial.available() ? Serial.read() : '0';
  delay(10);
  const int  m  = Serial.available() ? Serial.parseInt() : -1;
  delay(10);
  const char c2 = Serial.available() ? Serial.read() : '0';
  delay(10);
  const int  s  = Serial.available() ? Serial.parseInt() : -1;
  delay(10);
  const String used = String(h) + String(c1) + String(m) + String(c2) + String(s);
  if (h == -1) 
  {
    Serial.println(String("No hours, use e.g. '12:34:56' (used '") + used + String("')"));
    return;
  }
  if (c1 == '0') 
  {
    Serial.println("No first seperator, use e.g. '12:34:56' (used '" + used + "')");
    return;
  }
  if (m == -1) 
  {
    Serial.println("No minutes, use e.g. '12:34:56' (used '" + used + "')");
    return;
  }
  if (c2 == '0') 
  {
    Serial.println("No second seperator, use e.g. '12:34:56'");
    return;
  }
  if (s == -1) 
  {
    Serial.println("No seconds, use e.g. '12:34:56'");
    return;
  }
  if (h < 0 || h > 23)
  {
    Serial.println("Hours must be in range [0,23]");
    return;
  }
  if (m < 0 || h > 59)
  {
    Serial.println("Minutes must be in range [0,59]");
    return;
  }
  if (s < 0 || s > 59)
  {
    Serial.println("Seconds must be in range [0,59]");
    return;
  }
  setTime(h,m,s,0,0,0); 
}

void loop() 
{
  int last_sec = -1; //The previous second, used to detect a change in time, to be sent to serial monitor
  while (1)
  {
    //Respond to touches
    const int sensors_state = GetSensors();
    if (sensors_state == state_left_sensor_pressed) { SetTime(); delay(100); }

    if (Serial.available())
    {
      delay(100);
      SetTimeFromSerial();  
    }
    //Show the time
    const int s = second();
    const int m = minute();
    const int h = hour();

    if (last_sec == s) 
    {
      continue;
    }

    last_sec = s;
    ShowTimeOriginal(s,m,h);
    ShowTimeRainbow(s,m,h);

    //if (sensors_state == state_right_sensor_pressed) 
    { 
      const String time_now = String(h) + ":" + String(m) + ":" + String(s);
      Serial.println(time_now);
      delay(100);
    }
  }
}

///Show the time on all RGB LEDs
void ShowTimeOriginal(const int secs, const int mins, const int hours)
{
  #ifndef NDEBUG
  if (hours <  0) { OnError("ShowTime: hours <  0, hours = " + String(hours)); }
  if (hours > 23) { OnError("ShowTime: hours > 23, hours = " + String(hours)); }
  if (mins <  0) { OnError("ShowTime: mins <  0, mins = " + String(mins)); }
  if (mins > 59) { OnError("ShowTime: mins > 59, mins = " + String(mins)); }
  if (secs <  0) { OnError("ShowTime: secs <  0, secs = " + String(secs)); }
  if (secs > 59) { OnError("ShowTime: secs > 59, secs = " + String(secs)); }
  #endif // NDEBUG
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
  #ifndef NDEBUG
  if (hours <  0) { OnError("ShowTime: hours <  0, hours = " + String(hours)); }
  if (hours > 23) { OnError("ShowTime: hours > 23, hours = " + String(hours)); }
  if (mins <  0) { OnError("ShowTime: mins <  0, mins = " + String(mins)); }
  if (mins > 59) { OnError("ShowTime: mins > 59, mins = " + String(mins)); }
  if (secs <  0) { OnError("ShowTime: secs <  0, secs = " + String(secs)); }
  if (secs > 59) { OnError("ShowTime: secs > 59, secs = " + String(secs)); }
  #endif // NDEBUG
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
    green_value = 255 - red_value;    
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
