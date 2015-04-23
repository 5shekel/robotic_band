// GRslider for arduino mega 2560

#define DEBUG true
#define IR_enable false 

///// MIDI ///////
#include "MIDI.h"
#include "midi_Defs.h"
#include "midi_Message.h"
#include "midi_Namespace.h"
#include "midi_Settings.h"
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, midiA);
////////////////////////

#include <AccelStepper.h>
#include "Streaming.h"

////////// IR remote /////////
#ifdef IR_enable
  #include <IRremote.h>
  int RECV_PIN = 13;
  int prevRes;
  long previousMillisIR = 0; 
  long IRinterval = 400;
  IRrecv irrecv(RECV_PIN);
  decode_results results;
#endif
//////////////////////////

//////////////////////////
//Define Relay pins
#define RELAY0  38                        
#define RELAY1  40                        
#define RELAY2  42                       
#define RELAY3  44                        
#define RELAY4  46
#define RELAY5  48
//////////////////////////


//STEPPER PROPERTIES
//////// #1 //////////////
int stepper1_Speed = 400;

#define limitPin1a 5
#define limitPin1b 4
#define step1_STEP 7
#define step1_DIR 8
#define step1_ENABLE 6
int limit_A = 0; //which limit pin are we tochin, 1 is home 2 is other
boolean stepperMove = false;
AccelStepper stepper1(1, step1_STEP, step1_DIR); //step / dir
///////////////////////////////

//////// #2 //////////////
int stepper2_Speed = 5000;
int stepper2_accl = 4500;

#define limitPin2a 3
#define step2_STEP 10
#define step2_DIR 11
#define step2_ENABLE 12
AccelStepper stepper2(1, step2_STEP, step2_DIR); //step / dir

#define arrLen 19
int scalePos[arrLen] = {
  0, 44, 88, 132, 176, 220, 264, 308, 352, 396, 
  440, 484, 528, 572, 616, 660, 704, 748, 792};
///////////////////////////////

///////////////////////////////
// how often you want the serial input to be checked.
int measurementInterval = 50;
// when the serial input was last checked.
long lastMeasurementTime = 0L;
int sensorPin = A5;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
int knobGo = 0;

// smoothing stage, via analog/smoothing example
const int numReadings = 5;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

void homeAll(){
  knobGo=0; //stop knob if its on
  stepper1.enableOutputs();
  stepper2.enableOutputs();
  //run steppers until they hits the limit switch   
  homeStepper1();  
  homeStepper2();  
}

void sleepAll(){
  knobGo=0;
  stepper1.disableOutputs();
  stepper2.disableOutputs();
  Serial.println("steppers sleep");
}


void knobControl(){
    if(knobGo) {
      stepper2.enableOutputs();
      stepper2.setMaxSpeed(1000);
      stepper2.setAcceleration(10000);

      if (millis() > (lastMeasurementTime + measurementInterval)) {
        sensorValue = analogRead(sensorPin);
        sensorValue = map(sensorValue, 0 ,1024, 0, 800);
        
        total= total - readings[readIndex];         
        readings[readIndex] = sensorValue;
        total= total + readings[readIndex];       
        readIndex = readIndex + 1;                    
        if (readIndex >= numReadings)              
          readIndex = 0;                           

        average = total / numReadings; 

        stepper2.moveTo(average);
        Serial.println(average);

        lastMeasurementTime = millis();
      }
    }
}

void stepper1_action(){
  if(stepperMove)
  {
    if(!digitalRead(limitPin1a) && limit_A==1)
    {
      limit_A = 2;
      stepperMove = false;
      stepper1.setSpeed(stepper1_Speed);
      stepper1.disableOutputs();
    }
    else if(!digitalRead(limitPin1b) && limit_A==2)
    {
      limit_A = 1;
      stepperMove = false;
      stepper1.setSpeed(-stepper1_Speed);
      stepper1.disableOutputs();
    }
    stepper1.runSpeed();
  }
}

void goStepper2(int i_dest){
  stepper2.setMaxSpeed(stepper2_Speed);
  stepper2.setAcceleration(stepper2_accl);
  Serial<<"scalePos[i_dest] "<<scalePos[i_dest]<<endl;
  stepper2.moveTo(scalePos[i_dest]);
}


void setup(){
  //// RELAYS //////
    //Relays (solenoids)
  pinMode(RELAY0,OUTPUT);//relay0
  pinMode(RELAY1,OUTPUT);//relay1
  pinMode(RELAY2,OUTPUT);//relay2
  pinMode(RELAY3,OUTPUT);//relay3
  pinMode(RELAY4,OUTPUT);//relay4
  pinMode(RELAY5,OUTPUT);//relay5
  digitalWrite(RELAY0,LOW);//reset relay0
  digitalWrite(RELAY1,LOW);//reset relay1
  digitalWrite(RELAY2,LOW);//reset relay2
  digitalWrite(RELAY3,LOW);//reset relay3
  digitalWrite(RELAY4,LOW);//reset relay4
  digitalWrite(RELAY5,LOW);//reset relay5
  //////////////////////////////


  /////// stepper 1 ////////
  stepper1.setPinsInverted(14,0,1); //dir, step, enable
  stepper1.setMaxSpeed(stepper1_Speed);
  stepper1.setEnablePin(step1_ENABLE);
  stepper1.disableOutputs();
  
  pinMode(limitPin1a, INPUT_PULLUP); //arm the pullup for limit pin
  pinMode(limitPin1b, INPUT_PULLUP); //arm the pullup for limit pin

  /////// stepper 2 ////////
  stepper2.setPinsInverted(1,0,1); //dir, step, enable
  stepper2.setMaxSpeed(stepper2_Speed);
  stepper2.setEnablePin(step2_ENABLE);
  stepper2.disableOutputs();
  
  pinMode(limitPin2a, INPUT_PULLUP); //arm the pullup for limit pin

  ////////////////////////////
  if(DEBUG)    Serial.begin(115200);

    // setup for IR debounce 
    // initialize all the readings to 0: 
    if(IR_enable){ 
      irrecv.enableIRIn(); // Start the receiver
      for (int thisReading = 0; thisReading < numReadings; thisReading++)
        readings[thisReading] = 0;  
    }

  //run steppers until they hits the limit switch   
  homeStepper1();
  homeStepper2();  

  /// MIDI ///
  midiA.setHandleNoteOn(HandleNoteOn);
  midiA.setHandleNoteOff(HandleNoteOff);
  midiA.begin(MIDI_CHANNEL_OMNI);   
}

void loop(){

  if(IR_enable){
    unsigned long currentMillisIR = millis();
    if (irrecv.decode(&results)) {
      if(currentMillisIR - previousMillisIR > IRinterval) {
        previousMillisIR = currentMillisIR;   
        //do something here
        if(DEBUG) Serial.println(results.value);
        switchesOn(results.value, 0);

      }
      irrecv.resume(); // Receive the next value
    }
  }

  midiA.read();
  //if mode 6, translate analog pin(A5) to motion
  knobControl();
  stepper1_action();
  stepper2.run(); // slider stepper 02
}

////////// STEPPER SEQ ///////////

void homeStepper1(){
  //run stepper until it hits the limit switch
  //the while is BLOCKING so we cant use it in main loop
  if(DEBUG)Serial.println("home_01");

  stepper1.enableOutputs();
  stepper1.setSpeed(400); //override stepper1 speed, it might be slow
  while(digitalRead(limitPin1b))
    stepper1.runSpeed();

  // prime stepper for next action
  limit_A = 1;
  stepper1.setSpeed(-400);
  stepper1.disableOutputs();
}

void homeStepper2(){
  //run stepper until it hits the limit switch
  //the while is BLOCKING so we cant use it in main loop
  if(DEBUG)Serial.println("home_02");

  stepper2.enableOutputs();
  stepper2.setCurrentPosition(0);
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(10000);
  stepper2.moveTo(-4000);
  while(digitalRead(limitPin2a)){
    stepper2.run();
  }

  if(DEBUG)Serial.println("steps:"); //how many steps did we take to reach home
  if(DEBUG)Serial.println(stepper2.currentPosition());
  stepper2.setCurrentPosition(0);
  stepper2.disableOutputs();
}
//////////////////////////////////////


void HandleNoteOff(byte channel, byte pitch, byte velocity) {
  if(DEBUG) Serial << "midikey off>" << pitch << endl;

  //mapping solenoids knocks
  if(pitch==36) pitch = 14;
  if(pitch==37) pitch = 15;
  if(pitch==38) pitch = 16;
  if(pitch==39) pitch = 17;
  if(pitch==40) pitch = 18;
  if(pitch==41) pitch = 19;

  if(pitch==45) pitch = 1 ; //strummm

  //mapping frets , maping starts at 20
  if(pitch >= 47 && pitch <= 71)  pitch -= 27;  

  if(pitch == 72) pitch = 3;
  if(pitch == 74) pitch = 4;
  if(pitch == 76) pitch = 5;
  if(pitch == 77) pitch = 6;
  if(pitch == 79) pitch = 7;
  if(pitch == 81) pitch = 8;

  if(pitch == 83) pitch = 0; //strummm

  switchsOff(pitch, velocity);
}


void HandleNoteOn(byte channel, byte pitch, byte velocity) {
  if(DEBUG) Serial << "midikey  on>" << pitch << endl;

  //mapping solenoids knocks 
  if(pitch==36) pitch = 14;
  if(pitch==37) pitch = 15;
  if(pitch==38) pitch = 16;
  if(pitch==39) pitch = 17;
  if(pitch==40) pitch = 18;
  if(pitch==41) pitch = 19;

  if(pitch==45) pitch = 1; //strummm

  //mapping frets , maping starts at 20
  if(pitch >= 47 && pitch <= 71)  pitch -= 27;  

  if(pitch == 72) pitch = 3;
  if(pitch == 74) pitch = 4;
  if(pitch == 76) pitch = 5;
  if(pitch == 77) pitch = 6;
  if(pitch == 79) pitch = 7;
  if(pitch == 81) pitch = 8;

  if(pitch == 83) pitch = 0; //strummm
  if(pitch == 84) pitch = 2; //knob

  switchesOn(pitch, velocity);
}

void switchsOff(int I_pitch, int I_velocity){
  switch (I_pitch){
    case 0:
      break;

    case 1:
      break;

    default:
       //pick all other notes as knocks
        if(I_pitch==14) digitalWrite(RELAY0, LOW); //turn off relay0  
        if(I_pitch==15) digitalWrite(RELAY1, LOW); //turn off relay1
        if(I_pitch==16) digitalWrite(RELAY2, LOW); //turn off relay2
        if(I_pitch==17) digitalWrite(RELAY3, LOW); //turn off relay3
        if(I_pitch==18) digitalWrite(RELAY4, LOW); //turn off relay4
        if(I_pitch==19) digitalWrite(RELAY5, LOW); //turn off relay5  
      break;
  }
     // digitalWrite(RELAY1, LOW); //turn off relay1

}
void switchesOn(int I_pitch, int I_velocity){
  //general switch function, this works with both MIDI and IR
    switch (I_pitch) {
          case 0: 
            homeAll();
            if(DEBUG) Serial<<"homeAll"<<endl;
            break; 
          
          case 1: 
            stepper1.enableOutputs();
            stepperMove = 1;
            if(DEBUG) Serial<<"stepper1_action"<<endl;
            break;
          
          case 2:
          //knob control, will be decrepted
            knobGo = 1;
            break; 
          
          case 3: 
            stepper1_Speed += 50;
            if(DEBUG) Serial<<"current speed 1: "<<stepper1_Speed<<endl;
            break; 
          
          case 4: 
            stepper1_Speed -= 50;
            if(DEBUG) Serial<<"current speed 1: "<<stepper1_Speed<<endl;
            break;
          
          case 5: 
            stepper2_Speed += 100;
            if(DEBUG) Serial<<"current speed 2: "<<stepper2_Speed<<endl;
            break; 
          
          case 6: 
            stepper2_Speed -= 100;
            if(DEBUG) Serial<<"current speed 2: "<<stepper2_Speed<<endl;
            break;   

          case 7: 
            stepper2_accl += 100;
            if(DEBUG) Serial<<"current accel 2: "<<stepper2_accl<<endl;
            break; 
          
          case 8: 
            stepper2_accl -= 100;
            if(DEBUG) Serial<<"current accel 2: "<<stepper2_accl<<endl;
            break; 

          default: 
          //pick all other notes as fret or knocks
                 //pick all other notes as knocks
	    if(I_pitch==14) digitalWrite(RELAY0, HIGH); //turn on relay0
            if(I_pitch==15) digitalWrite(RELAY1, HIGH); //turn on relay1
            if(I_pitch==16) digitalWrite(RELAY2, HIGH); //turn on relay2
            if(I_pitch==17) digitalWrite(RELAY3, HIGH); //turn on relay3
            if(I_pitch==18) digitalWrite(RELAY4, HIGH); //turn on relay4
            if(I_pitch==19) digitalWrite(RELAY5, HIGH); //turn on relay5 

            if(I_pitch >= 20 && I_pitch <= 44){
              if(DEBUG) Serial<< "FRET on> "<< I_pitch << endl;
              stepper2.enableOutputs();
              
              I_pitch -= 20;
              if(DEBUG) Serial<< "FRET after> "<< I_pitch << endl;
              goStepper2(I_pitch);
            }
            break;  
  } 
}

/*
sony IR codes
1 16
2 2064
3 1040
4 3088
5 528
6 2576
7 1552
8 3600
9 272
0 2320
vol+ 1168
vol- 3216
ch+ 144
ch- 2192
power 2704
mute 656
input 2640
*/
