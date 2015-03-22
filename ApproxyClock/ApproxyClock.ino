/*

  ApproxyClock with RGB LEDs
  (C) 2015 Richel Bilderbeek

2015-03-21: v.1.0: Initial version
2015-03-22: v.1.1: use of Time library
2015-03-22: v.1.2: added parallel display of rainbow format

*/

#include <Time.h>

const int blue_original_pin = 3;
const int red_original_pin = 5;
const int green_original_pin = 6;

const int blue_rainbow_pin = 9;
const int red_rainbow_pin = 10;
const int green_rainbow_pin = 11;

const int resetpin = 7; //The pin connected to RST, RST connected to 10 kOhm resistance connected to 5V
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
  digitalWrite(resetpin,HIGH);  //This line is first, so when the Arduino is reset it doesn't reset again untill it's told to
  pinMode(resetpin,OUTPUT);     //Very important, without this line resetting won't work

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

    //Reset just before midnight
    if (h == 23 && m == 59 && s == 58) 
    {
      digitalWrite(resetpin,LOW);
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
    red_value = static_cast<int>(f * 255.0);
    blue_value = 255 - red_value;
  }
  else if (hours < 16)
  {
    const double f = static_cast<double>(((hours-8)*60) + mins) / (8 * 60);
    green_value = static_cast<int>(f * 255.0);
    red_value = 255 - red_value;    
  }
  else if (hours < 24)
  {
    const double f = static_cast<double>(((hours-16)*60) + mins) / (8 * 60);
    blue_value = static_cast<int>(f * 255.0);
    green_value = 255 - red_value;    
  }
  analogWrite(red_rainbow_pin,red_value);
  analogWrite(green_rainbow_pin,green_value);
  analogWrite(blue_rainbow_pin,blue_value);
}
