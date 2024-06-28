void setup()
{
  Serial.begin(9600);
}

int pin = A0;
int numSamples = 8000,currzerocount=181;
float in;

String State = "OFF";

void loop()
{
  float maxcurrent = 0;

  for(int i=0;i<numSamples;i++)
  {
      in = analogRead(pin);
      float inst_curr = ((in * 5 * 15)/1024);

      if(inst_curr>maxcurrent)maxcurrent = inst_curr;
  }

  if(maxcurrent==0)currzerocount++;
  else currzerocount=0;

  if(currzerocount>180)
  {
    State = "OFF";
  }
  else
  {
    State = "ON";
  }

  Serial.println(State);

  Serial.print("Max Current: ");
  Serial.println(maxcurrent,6);
}
