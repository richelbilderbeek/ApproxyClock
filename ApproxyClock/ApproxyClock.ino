const int red_pin = 5;
const int green_pin = 6;
const int blue_pin = 3;

void setup()
{
  pinMode(red_pin,OUTPUT);
  pinMode(green_pin,OUTPUT);
  pinMode(blue_pin,OUTPUT);
}

void loop()
{
  const long total_secs = millis() / 1000;
  const long secs = total_secs % 60;
  const long total_mins = total_secs / 60;
  const long mins = total_mins / 60;
  const long total_hours = total_mins / 60;
  const long hours = total_hours / 24;
  const int max_brightness = 128;
  const int red_value   = map(secs,0,60,0,max_brightness);
  const int green_value = map(mins,0,60,0,max_brightness);
  const int blue_value  = map(hours,0,24,0,max_brightness);
  analogWrite(red_pin,red_value);
  analogWrite(green_pin,green_value);
  analogWrite(blue_pin,blue_value);
  delay(100);
}
