// GRslider for arduino mega 2560

#define DEBUG true

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


//////////////////////////
//Define Relay pins
int RELAYS[6] = {
  38,40,42,48,46,44};
//////////////////////////

//STEPPER PROPERTIES
//////// #1 //////////////
int stepper1_Speed = 400;

#define limitPin1a 3
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

#define limitPin2a 5
#define step2_DIR 29
#define step2_STEP 31
#define step2_ENABLE 33
AccelStepper stepper2(1, step2_STEP, step2_DIR); //step / dir

int scalePos[19] = {
  0, 44, 88, 132, 176, 220, 264, 308, 352, 396, 
  440, 484, 528, 572, 616, 660, 704, 748, 792};

int action; //see eventclass for more


void stepper1_action(){
  if(stepperMove)
  {
    if(!digitalRead(limitPin1a) && limit_A==1)
    {
      limit_A = 2;
      stepperMove = false;
      stepper1.disableOutputs();
    }
    else if(!digitalRead(limitPin1b) && limit_A==2)
    {
      limit_A = 1;
      stepperMove = false;
      stepper1.disableOutputs();
    }
    stepper1.runSpeed();
  }
}


void HandleNoteOn(byte channel, byte pitch, byte velocity) {

  if(pitch==45) goStepper1(velocity);//stepper1 is here
  if(pitch >= 36 && pitch <= 41) digitalWrite(RELAYS[pitch-36], HIGH); // mapping solenoids knocks 
  if(pitch >= 47 && pitch <= 64)  goStepper2(pitch, velocity);  //mapping frets 
  if(pitch == 65) homeStepper2(); //hack to use the limit as a zeroing mechanism
  if(pitch == 83) homeAll(); //home
  //if(DEBUG) Serial << "ON: pitch/vel> " << pitch << " / "<< velocity <<endl;

}

void HandleNoteOff(byte channel, byte pitch, byte velocity){
  if(pitch >= 36 && pitch <= 41)  digitalWrite(RELAYS[pitch-36], LOW);
}


void goStepper2(int i_dest, int I_velocity){
  //takes the various cntrols for the slider stepper2
  // and starts to moves the stepper
  stepper2.enableOutputs();
  //stepper2_Speed = map(I_velocity, 0 , 127, 300, 3000);
  stepper2_Speed = 2000;
  stepper2.setMaxSpeed(stepper2_Speed);
  stepper2.setAcceleration(stepper2_Speed-1);
  int tempDest = i_dest - 47;
  stepper2.moveTo(scalePos[tempDest]);
}

void goStepper1(int I_velocity){
    stepper1.enableOutputs();
    stepper1_Speed = map(I_velocity, 0, 127, 70, 450);

      if(limit_A == 1){
              stepper1.setSpeed(-stepper1_Speed);
      }else{
              stepper1.setSpeed(stepper1_Speed);
      }
      stepperMove = 1;
}

void homeAll(){
  //run steppers until they hits the limit switch   
  homeStepper1();  
  homeStepper2();  
}

void sleepAll(){
  stepper1.disableOutputs();
  stepper2.disableOutputs();
}

void homeStepper1(){
  //run stepper until it hits the limit switch
  //the while is BLOCKING so we cant use it in main loop
  stepper1.enableOutputs();
  stepper1.setSpeed(-400); //override stepper1 speed, it might be slow
  while(digitalRead(limitPin1a)){
    stepper1.runSpeed();
    
    //if(DEBUG) Serial<<"limitPin1a= "<<digitalRead(limitPin1a)<< " limitPin1b= "<<digitalRead(limitPin1b)<<endl;
  }

  // prime stepper for next action
  limit_A = 1;
  stepper1.disableOutputs();
}

void homeStepper2(){
  //run stepper until it hits the limit switch
  //the while is BLOCKING so we cant use it in main loop
  stepper2.enableOutputs();
  stepper2.setMaxSpeed(1000);
  stepper2.setAcceleration(10000);
  stepper2.moveTo(-4000); //minus for going left
  while(digitalRead(limitPin2a)){
    stepper2.run();
  }
  stepper2.setCurrentPosition(0);
  stepper2.disableOutputs();
}



void setup(){
  //// RELAYS //////
  //Relays (solenoids)
  for(int iii=0;iii<=6;iii++){
    pinMode(RELAYS[iii], OUTPUT);
    digitalWrite(RELAYS[iii], LOW);
  }
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
  if(DEBUG)    Serial<<"restart:"<<endl;

  //run steppers until they hits the limit switch   
  homeStepper1();
  homeStepper2();  

  /// MIDI ///
  midiA.setHandleNoteOn(HandleNoteOn);
  midiA.setHandleNoteOff(HandleNoteOff);
  midiA.begin(1);   
}

void loop(){
  midiA.read();
  stepper1_action(); 
  stepper2.run(); // slider stepper 02
  //if(stepper2.distanceToGo()==0) stepper2.disableOutputs(); 
}




