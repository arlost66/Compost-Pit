#include <Arduino.h>

// Fill-in information from your Blynk Template here
#include <TimeLib.h>

//
#define BLYNK_TEMPLATE_ID "TMPLrF3QZtDg"
#define BLYNK_DEVICE_NAME "NODEMCU"

//Input pins
#define STpin A0 //Soil Temperature pin V1
#define SHpin D1  //Soil Humidity pin   V2
#define ATpin D2 //Ambient Temperature pin  V3
#define Gpin D5  //Gas sensor pin V4

//Output pins
#define WPpin D3 //Water pump pin   
#define Spin D4   //servo motor pin
#define DCpin D6 // Shredder

int switchStart;
int switchEnd;

//Global Variable for sensor data
int STval;
int SHval;
int ATval;
int Gval;

//Soil Humidity mappin
int SHmap;

//Local Time Zone holder
unsigned long long phtime; // ph time holder
unsigned long long startingTime;

//Timer IDs
int soil_humidity_timer_ID;
int soil_temperature_timer_ID;
int ambient_temperature_timer_ID;
int methane_gas_timer_ID;

int v5Timer_ID;
int v6Timer_ID;

time_t date = now(); //date holds unix time from blynk server



#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG
#include "BlynkEdgent.h"

BlynkTimer timer;
void v5Timer() // function for timer V5 string for date started
{
  if (startingTime == 0)
  {
    Blynk.virtualWrite(V5, "Not yet Started");
  }
  else{
    Serial.print(String(day(startingTime)) + "/" + String(month(startingTime))+"/"+ String(year(startingTime))+"/\n"+String(hour(startingTime)) + ":" + String(minute(startingTime)) +  ":" + String(second(startingTime)));
    Blynk.virtualWrite(V5,String(day(startingTime)) + "/" + String(month(startingTime))+"/"+ String(year(startingTime))+"/ \n"+String(hour(startingTime)) + ":" + String(minute(startingTime)) +  ":" + String(second(startingTime)) );
  }  
}

void v6Timer()
{
  Blynk.virtualWrite(V6, startingTime);
}
BLYNK_WRITE(V0) //ON or OFF of system
{
  switchStart = param.asInt();
  if(switchStart == 1){
    //Blynk.sendInternal("rtc", "sync");
    digitalWrite(2, HIGH);
    Serial.println("All timer on");
    Blynk.virtualWrite(V7, 0);
    timer.enable(soil_humidity_timer_ID);
  }
  else{
   timer.disable(soil_humidity_timer_ID);
    Serial.println("All timer Off");
    digitalWrite(2, LOW);
  }
}

BLYNK_WRITE(V6) //saving STARTING TIME IN BLYNK SERVER
{
  if(param.asLong() != 0){
    startingTime = param.asLong();
  }
}

//switch to finish the system
BLYNK_WRITE(V7)
{
  switchEnd = param.asInt();
  if(switchEnd == 1)
  {
    startingTime = 0;
    Blynk.virtualWrite(V5 , "Not yet Started");
    Blynk.virtualWrite(V6, startingTime);
    Blynk.virtualWrite(V0 , 0); //turns off the start switch
    timer.disable(soil_humidity_timer_ID);
    Serial.println("All timer Off");
    digitalWrite(2, LOW);
  }
  else{

  }
  
}

BLYNK_WRITE(InternalPinRTC) {
  if (switchStart == 1){
    if(startingTime == 0)
    {
       startingTime = param.asLong(); //when resetting the system please reset startingTime to zero
      // Blynk.virtualWrite(V6, starting;ime);
       timer.enable(v6Timer_ID);
       timer.setTimeout(4320000000L,v6T)
      // timer.deleteTimer(v6Timer_ID);
       //timer.disable(v6Timer_ID);

       timer.enable(v5Timer_ID);
       //timer.deleteTimer(v5Timer_ID);
       //timer.disable(v5Timer_ID);
       //Serial.print(String(day(startingTime)) + "/" + String(month(startingTime))+"/"+ String(year(startingTime))+"/\n"+String(hour(startingTime)) + ":" + String(minute(startingTime)) +  ":" + String(second(startingTime)));
     //  Blynk.virtualWrite(V5,String(day(startingTime)) + "/" + String(month(startingTime))+"/"+ String(year(startingTime))+"/ \n"+String(hour(startingTime)) + ":" + String(minute(startingTime)) +  ":" + String(second(startingTime)) );
    }
  }
    phtime = param.asLong();
    setTime(phtime);
   // Serial.println("InternalPinRTC gets called");
}







BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V0);  
  Blynk.sendInternal("rtc", "sync");
  
}

void soil_humidity()
{
  Blynk.sendInternal("rtc", "sync");
  SHval = analogRead(STpin);
  SHmap = map(SHval,0,1023,100,0);
  Blynk.virtualWrite(V2, SHmap);
  Serial.println("Timer Soil Humidity Started");
  Serial.println(SHmap);

 Serial.println("startingTime:");
    Serial.println(startingTime);
}



void setup()
{
  pinMode(2, OUTPUT); // Initialise digital pin 2 as an output pin
  pinMode(STpin, INPUT);

 //int soil_humidity_timer_ID;
 //int soil_temperature_timer_ID;
 //int ambient_temperature_timer_ID;
 //int methane_gas_timer_ID;
  
  //breakTime(startingTimeTimeLib, startingTimeString);  

  timer.setTimeout(3600000L, [] () {} ); // dummy/sacrificial Function                                                   
  soil_humidity_timer_ID = timer.setInterval(2000L, soil_humidity);

  v5Timer_ID = timer.setInterval(500L, v5Timer, 1);
  v6Timer_ID = timer.setInterval(800L, v6Timer, 1);

  Serial.begin(115200);
  BlynkEdgent.begin();
}

void loop() {

  
  timer.run();
  BlynkEdgent.run();
}