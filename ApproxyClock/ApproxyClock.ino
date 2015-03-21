/*

  ApproxyClock with RGB LEDs
  (C) 2015 Richel Bilderbeek

2015-03-21: v.1.0: Initial version

*/

const int red_pin = 5;
const int green_pin = 6;
const int blue_pin = 3;
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

class Time
{
  public:
  Time(const int delta_secs, const int delta_mins, const int delta_hours, const long my_millis = -1)
    : m_total_msecs(my_millis == -1 ? millis() : my_millis)
  {
    #ifndef NDEBUG
    if (m_total_msecs < 0) { OnError("Time: m_total_msecs < 0, m_total_msecs = " + String(m_total_msecs)); }
    if (delta_secs <  0) { OnError("Time: delta_secs <  0, delta_secs = " + String(delta_secs)); }
    if (delta_secs > 59) { OnError("Time: delta_secs > 59, delta_secs = " + String(delta_secs)); }
    if (delta_mins <  0) { OnError("Time: delta_mins <  0, delta_mins = " + String(delta_mins)); }
    if (delta_mins > 59) { OnError("Time: delta_mins > 59, delta_mins = " + String(delta_mins)); }
    if (delta_hours <  0) { OnError("Time: delta_hours <  0, delta_hours = " + String(delta_hours)); }
    if (delta_hours > 23) { OnError("Time: delta_hours > 23, delta_hours = " + String(delta_hours)); }
    #endif // NDEBUG

    m_total_secs = delta_secs + (m_total_msecs / 1000);
    m_secs = m_total_secs % 60;
    m_total_mins = delta_mins + (m_total_secs / 60);
    m_mins = m_total_mins % 60;
    m_total_hours = delta_hours + (m_total_mins / 60);
    m_hours = m_total_hours % 24;    

    #ifndef NDEBUG
    if (m_total_secs  < 0) { OnError("Time: m_total_secs < 0, m_total_secs = "   + String(m_total_secs )); }
    if (m_total_mins  < 0) { OnError("Time: m_total_mins < 0, m_total_mins = "   + String(m_total_mins )); }
    if (m_total_hours < 0) { OnError("Time: m_total_hours < 0, m_total_hours = " + String(m_total_hours)); }

    if (m_secs > 59) { OnError("Time: m_secs > 59, delta_secs = " + String(delta_secs)); }
    if (m_secs <  0) { OnError("Time: m_secs <  0, delta_secs = " + String(delta_secs)); }
    if (m_secs > 59) { OnError("Time: m_secs > 59, delta_secs = " + String(delta_secs)); }
    if (m_mins <  0) { OnError("Time: m_mins <  0, delta_mins = " + String(delta_mins)); }
    if (m_mins > 59) { OnError("Time: m_mins > 59, delta_mins = " + String(delta_mins)); }
    if (m_hours <  0) { OnError("Time: m_hours <  0, delta_hours = " + String(delta_hours)); }
    if (m_hours > 23) { OnError("Time: m_hours > 23, delta_hours = " + String(delta_hours)); }
    #endif // NDEBUG
  }
  int GetHours() const { return m_hours; }
  int GetMins() const { return m_mins; }
  int GetSecs() const { return m_secs; }
  private:
  int m_hours;
  int m_mins;
  int m_secs; 
  const long m_total_msecs; //Must be long
  long m_total_secs; //Must be long, a day has more than 65536 secs
  int m_total_mins;
  int m_total_hours;
};

Time GetTime();

int delta_hours = 0;
int delta_mins = 0;
int delta_secs = 0;

Time GetTime()
{
  return Time(delta_secs,delta_mins,delta_hours);
}

void TestTime()
{
  Time(0,0,0);
  Time(0,0,0,0);
  if (Time(0,0,0,1000).GetSecs() != 1) { OnError("TestTime #1"); }
  if (Time(0,0,0, 60000).GetMins() != 1) { OnError("TestTime #2"); }
  if (Time(0,0,0,3600000).GetHours() != 1) { OnError("TestTime #3"); }
  if (Time(59,0,0,1000).GetSecs() != 0) { OnError("TestTime #4"); }
  if (Time(59,0,0,1000).GetMins() != 1) { OnError("TestTime #5"); }
  if (Time(0,59,0,60000).GetMins() != 0) { OnError("TestTime #6"); }
  if (Time(0,59,0,60000).GetHours() != 1) { OnError("TestTime #7"); }
  if (Time(0,0,22,3600000).GetHours() != 23) { OnError("TestTime #8"); }
  if (Time(0,0,23,3600000).GetHours() !=  0) { OnError("TestTime #9"); }
}

void setup()
{
  digitalWrite(resetpin,HIGH);  //This line is first, so when the Arduino is reset it doesn't reset again untill it's told to
  pinMode(resetpin,OUTPUT);     //Very important, without this line resetting won't work

  pinMode(red_pin,OUTPUT);
  pinMode(green_pin,OUTPUT);
  pinMode(blue_pin,OUTPUT);
  pinMode(error_pin,OUTPUT);
  Serial.begin(9600); //Cannot be used: chip is used stand-alone
  #ifndef NDEBUG
  Serial.println("ApproxyClock v. 1.0 (debug version)");
  #else //NDEBUG
  Serial.println("ApproxyClock v. 1.0 (release version)");
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
  const int pre_delta_hour = GetTime().GetHours() - delta_hours;
  delta_hours = ((h - pre_delta_hour) + 24) % 24;

  const int pre_delta_mins = GetTime().GetMins() - delta_mins;
  delta_mins = ((m - pre_delta_mins) + 60) % 60;

  const int pre_delta_secs = GetTime().GetSecs() - delta_secs;
  delta_secs = ((s - pre_delta_secs) + 60) % 60;
  /*
  #ifndef NDEBUG
  const String time_indended = String(h) + String(":") + String(m) + String(":") + String(s);
  const String time_realized 
    = String(GetTime().GetHours()) 
    + String(":") 
    + String(GetTime().GetMins()) 
    + String(":") 
    + String(GetTime().GetSecs()
  );
  const String debug_msg 
    = String("time_indended (") 
    + time_indended 
    + String("), time_realized (") 
    + time_realized 
    + String(") ")
  ;
  delay(100);
  Serial.println(debug_msg);
  delay(100);
  if (time_indended != time_realized)
  {
    const String error_msg 
      = String("time_indended (") 
      + time_indended 
      + String(")!= time_realized (") 
      + time_realized 
      + String(")")
    ;
    OnError(error_msg);
  }
  #endif
  */
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
    const Time t = GetTime();
    const int s = t.GetSecs();
    const int m = t.GetMins();
    const int h = t.GetHours();


    if (last_sec == s) 
    {
      continue;
    }

    last_sec = s;
    ShowTime(s,m,h);

    //if (sensors_state == state_right_sensor_pressed) 
    { 
      //Send debug message to console window
      const Time t = GetTime();
      const String deltas = String(delta_hours) + ":" + String(delta_mins) + ":" + String(delta_secs);
      const String time_now = String(h) + ":" + String(m) + ":" + String(s);
      Serial.print(deltas);
      Serial.print(" -> ");
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
void ShowTime(const int secs, const int mins, const int hours)
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
  analogWrite(red_pin,red_value);
  analogWrite(green_pin,green_value);
  analogWrite(blue_pin,blue_value);
}
