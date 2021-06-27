#include <IRremote.h>
#include<Servo.h>

int IRSensorPin = 11;
IRrecv irrecv(IRSensorPin);
decode_results irrecv_result;

Servo motor;
int isGateOpen = false;

int currentLedMode = 0;
int ledModes[][8][8] = {
  {{1,0,0,0,0,0,0,1},{1,1,0,1,0,1,0,1},{1,1,1,1,1,1,1,1},{0,0,0,0,0,0,0,0}, // mode 1
   {0,0,1,0,1,0,1,0},{1,0,1,0,1,0,1,1},{1,1,1,1,1,1,1,1},{0,0,0,0,0,0,0,0}},
  
  {{0,1,1,0,0,0,0,0},{0,1,1,1,1,0,0,0},{0,1,1,1,1,1,1,0},{1,1,1,1,1,1,1,1}, // mode 2
   {0,1,1,1,1,1,1,0},{0,1,1,1,1,0,0,0},{0,1,1,0,0,0,0,0},{1,0,0,0,0,0,0,1}},

    {{1,0,0,0,0,0,0,1},{1,1,0,0,0,0,1,1},{1,1,1,0,0,1,1,1},{1,1,1,0,1,1,1,1}, // mode 3
     {1,1,1,1,0,1,1,1},{1,1,1,1,1,1,1,1},{1,1,1,0,0,1,1,1},{0,1,1,0,0,1,1,0}},
   
  {{1,0,0,0,0,0,0,0},{1,1,0,0,0,0,0,0},{1,1,1,0,0,0,0,0},{1,1,1,1,0,0,0,0}, // mode 4
   {1,1,1,1,1,0,0,0},{1,1,1,1,1,1,0,0},{1,1,1,1,1,1,1,0},{1,1,1,1,1,1,1,1}},

  {{0,0,0,0,0,0,0,1},{0,0,0,0,0,0,1,1},{0,0,0,0,0,1,1,1},{0,0,0,0,1,1,1,1}, // mode 5
   {0,0,0,1,1,1,1,1},{0,0,1,1,1,1,1,1},{0,1,1,1,1,1,1,1},{1,1,1,1,1,1,1,1}},

  {{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1},{1,0,0,0,0,0,0,1},{1,1,1,1,1,1,1,1}, // mode 6
   {1,1,0,0,0,0,1,1},{1,1,1,1,1,1,1,1},{1,1,0,0,0,0,1,1},{1,1,1,1,1,1,1,1}}
};

int ledSpeedMode = 1;
int ledSpeed[3] = {100,400,1000};

int buzzer = 12;

void playSingleTone(){
    digitalWrite(buzzer,1);
    delay(100);
    digitalWrite(buzzer,0);
}

void playDoubleTone(){
    digitalWrite(buzzer,1);
    delay(20);
    digitalWrite(buzzer,0);
    delay(100);
    digitalWrite(buzzer,1);
    delay(20);
    digitalWrite(buzzer,0);
}

void playTripleTone(){
    digitalWrite(buzzer,1);
    delay(100);
    digitalWrite(buzzer,0);
    delay(100);
    digitalWrite(buzzer,1);
    delay(200);
    digitalWrite(buzzer,0);
    delay(100);
    digitalWrite(buzzer,1);
    delay(300);
    digitalWrite(buzzer,0);
}

void handleGate(){
  lightfAllLed();
  delay(200);
  turnOffAllLed();
  if(isGateOpen){ motor.write(80); }
  else{ motor.write(180); }
  isGateOpen = !isGateOpen;
}

void handleIRSingle(){
  if (irrecv.decode(&irrecv_result)) {
    int IRCode = irrecv_result.value;
    Serial.println("Button Code");
    Serial.println(IRCode);
    if(IRCode == 20655){ //click power
      playDoubleTone();
      if(currentLedMode != 0){ currentLedMode=0; }
      else{ currentLedMode=1; }
    }
    else if(IRCode == 28815){playSingleTone();currentLedMode=1;} //click 1
    else if(IRCode == 24735){playSingleTone();currentLedMode=2;} //click 2
    else if(IRCode == -4081){playSingleTone();currentLedMode=3;} //click 3
    else if(IRCode == 18615){playSingleTone();currentLedMode=4;} //click 4
    else if(IRCode == -8161){playSingleTone();currentLedMode=5;} //click 5
    else if(IRCode == -14281){playSingleTone();currentLedMode=6;} //click 6
    else if(IRCode == -9691){playSingleTone();if(ledSpeedMode != 2){ledSpeedMode++;}} //click leftSide 
    else if(IRCode == 6885){playSingleTone();if(ledSpeedMode != 0){ledSpeedMode--;}} //click rightSide
    else if(IRCode == 23205){playTripleTone(); handleGate();} // click ok
    irrecv.resume();
  }
}

void turnOffAllLed(){
   for(int i=2;i<=9;i++){digitalWrite(i,0);}
}

void lightfAllLed(){
   for(int i=2;i<=9;i++){digitalWrite(i,1);}
}


void setLedState(int state[8],int delayTime,int ledMode){
    handleIRSingle();
    if(currentLedMode != ledMode){
        turnOffAllLed();
        return;
    }
    for(int i=0;i<8;i++){
      if(state[i] == 1){ digitalWrite(i+2,1); }
      else{ digitalWrite(i+2,0); }
    }
    delay(delayTime);
}

void setLedStates(int states[8][8],int ledMode){
  for(int i=0;i<8;i++){
      setLedState(states[i],ledSpeed[ledSpeedMode],ledMode);
  }
}

void startLedMode(int mode){
  if(mode != 0){setLedStates(ledModes[mode-1],mode);}
  turnOffAllLed();
}

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
  motor.attach(10);
  motor.write(80);
  for(int i=2;i<=9;i++){pinMode(i,OUTPUT);}
  pinMode(buzzer, OUTPUT);
}


void loop(){
  handleIRSingle();
  startLedMode(currentLedMode);
}





