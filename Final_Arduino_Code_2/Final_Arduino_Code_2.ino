#include<SoftwareSerial.h>
SoftwareSerial s(5,6);

void setup()
{
  Serial.begin(9600);
  s.begin(9600);
}

int pin1 = A0,pin2 = A1;
int numSamples = 4000;
float in;
int timecount=0;

int currzerocount1=181;
int currzerocount2 = 181;

String State1 = "OFF";
String State2 = "OFF";

int whichcycle=1;

void SendUART(int whichcycle,String State,float maxcurrent)
{
  if(s.available()>0)
  {
    s.print(whichcycle);
    if(State == "ON")
      s.print(1);
    else
      s.print(0);

    s.print(maxcurrent,6);
  
    Serial.println("sent");
  }
  else
  {
    Serial.println("Device Not available");
  }
}

void loop()
{
  float maxcurrent1 = 0;
  float maxcurrent2 = 0;

  for(int i=0;i<numSamples;i++)
  {
      in = analogRead(pin1);
      float inst_curr = ((in * 5 * 15)/1024);

      if(inst_curr>maxcurrent1)maxcurrent1 = inst_curr;

      in = analogRead(pin2);
      inst_curr = ((in * 5 * 15)/1024);
      if(inst_curr>maxcurrent2)maxcurrent2 = inst_curr;
  }

  if(maxcurrent1==0 && currzerocount1<=180)currzerocount1++;
  else currzerocount1=0;

  if(maxcurrent2==0 && currzerocount2<=180)currzerocount2++;
  else currzerocount2=0;

  if(currzerocount1>180)
  {
    State1 = "OFF";
  }
  else
  {
    State1 = "ON";
  }

  if(currzerocount2>180)
  {
    State2 = "OFF";
  }
  else
  {
    State2 = "ON";
  }

  timecount++;

  if(timecount >=12)
  {
    timecount = 0;

    if(whichcycle == 1)
    {
      SendUART(whichcycle,State1,maxcurrent1);
      whichcycle = 2;
    }
    else
    {
      SendUART(whichcycle,State2,maxcurrent2);
      whichcycle = 1;
    }
  }

  Serial.println(State1);

  Serial.print("Max Current1: ");
  Serial.println(maxcurrent1,6);

  Serial.println(State2);

  Serial.print("Max Current2: ");
  Serial.println(maxcurrent2,6);
}
