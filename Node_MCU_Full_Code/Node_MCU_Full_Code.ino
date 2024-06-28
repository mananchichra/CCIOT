#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include <SoftwareSerial.h>

SoftwareSerial s(D5,D6);

const char *ssid = "O.M.";
const char *password = "ommehta343";
// const char *ssid = "JioFi3_8AD7D9";
// const char *password = "66em6fze6e";

const char *thingSpeakAddress = "api.thingspeak.com";
const char *apiKey1 = "W753QY1FQ6UYVYG5";
const char *apiKey2 = "LBVN4QDI9R6E5J2S";

int channelid1 = 2490599;
int channelid2 = 2513220;

WiFiClient client;

void setup()
{
  Serial.begin(9600);
  s.begin(9600); 

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != 7) {
    delay(500);
    WiFi.begin(ssid, password);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  ThingSpeak.begin(client);
}

int timecount = 0;
int receiveddata;

String State,State1,State2;
String sensechar;

float sensorvalue,sensorvalue1,sensorvalue2;

int whichchannel=1;

void loop()
{
  if (s.available() >= sizeof(char))
  {
    int count = 0;
    while(s.available() > 0)
    {
      receiveddata = s.read();
      count++;

      if(receiveddata == 49 && count == 1)
      {
          whichchannel = 1;
      }
      else if(receiveddata == 50 && count == 1)
      {
          whichchannel = 2;
      }
      else if(receiveddata == 49 && count == 2)
      {
        State = "ON";
      }
      else if(receiveddata == 48 && count == 2)
      {
        State = "OFF";
      }
      else
      {
        sensechar+=(char)receiveddata;
      }

      Serial.print("Received data: ");
      Serial.println(State);
    }

    Serial.println(sensechar);
  }
  else
  {
    Serial.println("Device Not Available");
  }

  sensechar.clear();

  sensorvalue = atof(sensechar.c_str());

  if(whichchannel==1)
  {
    sensorvalue2 = sensorvalue;
    State2 = State;
  }
  else
  {
    sensorvalue1 = sensorvalue;
    State1 = State;
  }

  timecount++;

  if(timecount>=20)
  {
    Serial.println(WiFi.status());
    Sendreading();
    timecount = 0;
  }

  delay(1000);
}

void Sendreading()
{
  Serial.println(sensorvalue1);
  Serial.println(sensorvalue2);


  int statuscode1 = 0,statuscode2 = 0;

  if(State1 == "ON")statuscode1 = 1;
  else statuscode1 = 0;

  if(State2 == "ON")statuscode2 = 1;
  else statuscode2 = 0;

  int status = 0;
  if(whichchannel==1)
  {
    ThingSpeak.setField(1,sensorvalue1);
    ThingSpeak.setField(2,statuscode1);
    status = ThingSpeak.writeFields(2490599, apiKey1);
  }
  else
  {
    ThingSpeak.setField(1,sensorvalue2);
    ThingSpeak.setField(2,statuscode2);
    status = ThingSpeak.writeFields(2513220, apiKey2);
  }

  if (status == 200)
  {
    Serial.println("Data sent to ThingSpeak successfully");
  } else {
    Serial.print("Problem sending data to ThingSpeak. HTTP error code ");
    Serial.println(status);
  }
}
