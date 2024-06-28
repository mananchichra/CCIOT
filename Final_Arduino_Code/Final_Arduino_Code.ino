#include<SoftwareSerial.h>
SoftwareSerial s(5,6);

void setup()
{
  analogReference(INTERNAL);
  Serial.begin(9600);
  s.begin(9600);
}

int pin1 = A2,pin2 = A5;
int numSamples = 4000;
float in;
int timecount=0;

int currzerocount1=181;
int currzerocount2 = 181;

int numnonzero1 = 0,numnonzero2 = 0;
int timefor_nonzero = 0;

String State1 = "OFF";
String State2 = "OFF";

int whichcycle=1;

void SendUART(int whichcycle,String State,float maxcurrent,char* stage)
{
  if(s.available()>0)
  {
    s.print(whichcycle);
    if(State == "ON")
      s.print(1);
    else
      s.print(0);

    if(stage == "OFF")
      s.print(0);
    else if(stage == "soak")
      s.print(1);
    else if(stage == "wash")
      s.print(2);
    else if(stage == "wash_end")
      s.print(3);
    else if(stage == "rinse")
      s.print(4);
    else if(stage == "rinse_end")
      s.print(5);
    else
      s.print(6);

    s.print(maxcurrent,6);
  
    Serial.println("sent");
  }
  else
  {
    Serial.println("Device Not available");
  }
}

char* currstage1 = "OFF";
int K=30,index1=0;
float* stats1 = (float*)calloc(K,sizeof(float));
float currmean1 = 0,currex21=0;

float currstddev1 = 0;

char* currstage2 = "OFF";
int index2=0;
float* stats2 = (float*)calloc(K,sizeof(float));
float currmean2 = 0,currex22=0;

float currstddev2 = 0;

void changestage(int which)
{
  if(which==1)
  {
    if(currstage1 == "soak")
      currstage1 = "wash";
    else if(currstage1 == "wash")
      currstage1 = "wash_end";
    else if(currstage1=="wash_end")
      currstage1 = "rinse";
    else if(currstage1 == "rinse")
      currstage1 = "rinse_end";
    else if(currstage1 == "rinse_end")
      currstage1 = "spin";
    else currstage1 = "soak";
  }
  else
  {
    if(currstage2 == "soak")
      currstage2 = "wash";
    else if(currstage2 == "wash")
      currstage2 = "wash_end";
    else if(currstage2=="wash_end")
      currstage2 = "rinse";
    else if(currstage2 == "rinse")
      currstage2 = "rinse_end";
    else if(currstage2 == "rinse_end")
      currstage2 = "spin";
    else currstage2 = "soak";
  }
}

float updatestats(float point1,float point2)
{
  float prev = stats1[index1];
  stats1[index1] = point1;

  currmean1 = (currmean1*K + point1 - prev)/K;
  currex21 = (currex21*K + point1*point1 - prev*prev)/K;

  index1= (index1+1)%K;

  prev = stats2[index2];
  stats2[index2] = point2;

  currmean2 = (currmean2*K + point2 - prev)/K;
  currex22 = (currex22*K + point2*point2 - prev*prev)/K;

  index2= (index2+1)%K;
}

int OFFcount1 = 0,OFFcount2=0;

void detectstage(float point1,float point2)
{
  updatestats(point1,point2);
  currstddev1 = sqrt(currex21 - currmean1*currmean1);

  if(State1 == "ON")OFFcount1=0;
  if(State1 == "OFF")
  {
    if(OFFcount1 <=10)OFFcount1++;
    else currstage1 = "OFF";
  }
  else if(currstage1 == "OFF" && currmean1>0)changestage(1);
  else if(currstage1 == "soak" && currmean1>3)changestage(1);
  else if(currstage1 == "wash" && currmean1>0.75 && currmean1<2.5 && currstddev1 < 1)changestage(1);
  else if(currstage1 == "wash_end" && currmean1<0.5)changestage(1);
  else if(currstage1 == "rinse" && currmean1>0.75 && currmean1<2.5 && currstddev1 < 1)changestage(1);
  else if(currstage1 == "rinse_end" && currmean1<0.5)changestage(1);

  // Serial.println(currmean1);
  // Serial.println(currex21);

  currstddev2 = sqrt(currex22 - currmean2*currmean2);

  if(State2 == "ON")OFFcount2=0;
  if(State2 == "OFF")
  {
    if(OFFcount2 <=10)OFFcount2++;
    else currstage2 = "OFF";
  }
  else if(currstage2 == "OFF" && currmean2>0)changestage(2);
  else if(currstage2 == "soak" && currmean2>3.3)changestage(2);
  else if(currstage2 == "wash" && currmean2>0.75 && currmean2<2.5 && currstddev2 < 1)changestage(2);
  else if(currstage2 == "wash_end" && currmean2<0.5)changestage(2);
  else if(currstage2 == "rinse" && currmean2>0.75 && currmean2<2.5 && currstddev2 < 1)changestage(2);
  else if(currstage2 == "rinse_end" && currmean2<0.5)changestage(2);
}

void loop()
{
  float maxcurrent1 = 0;
  float maxcurrent2 = 0;

  for(int i=0;i<numSamples;i++)
  {
      in = analogRead(pin1);
      float inst_curr = ((in * 1.1 * 15)/1023);

      if(inst_curr>maxcurrent1)maxcurrent1 = inst_curr;

      in = analogRead(pin2);
      inst_curr = ((in * 1.1 * 15)/1023);
      if(inst_curr>maxcurrent2)maxcurrent2 = inst_curr;
  }

  if(maxcurrent1==0 && currzerocount1<=180)currzerocount1++;
  else if(maxcurrent1>0)
  {
    if(numnonzero1 < 2)
    {
      numnonzero1++;
    }
    else
    {
      numnonzero1=0;
      currzerocount1=0;
    }
  }

  if(maxcurrent2==0 && currzerocount2<=180)currzerocount2++;
  else if(maxcurrent2>0)
  {
    if(numnonzero2 < 2)
    {
      numnonzero2++;
    }
    else
    {
      numnonzero2=0;
      currzerocount2=0;
    }
  }

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

  // Serial.println(numnonzero1);
  // Serial.println(currzerocount1);
  // Serial.println(numnonzero2);
  // Serial.println(currzerocount2);

  timecount++;
  timefor_nonzero++;

  if(timefor_nonzero >= 300)
  {
    timefor_nonzero = 0;
    numnonzero1 = 0;
    numnonzero2 = 0;
  }

  if(timecount >=10)
  {
    timecount = 0;

    if(whichcycle == 1)
    {
      SendUART(whichcycle,State1,maxcurrent1,currstage1);
      whichcycle = 2;
    }
    else
    {
      SendUART(whichcycle,State2,maxcurrent2,currstage2);
      whichcycle = 1;
    }
  }

  Serial.println(State1);

  Serial.print("Max Current1: ");
  Serial.println(maxcurrent1,6);

  Serial.println(State2);

  Serial.print("Max Current2: ");
  Serial.println(maxcurrent2,6);

  detectstage(maxcurrent1,maxcurrent2);

  Serial.println(currstage1);
  Serial.println(currstage2);
}
