// robobass_v5
// classes

#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>
#include <Streaming.h>
#include <AccelStepper.h>
MIDI_CREATE_DEFAULT_INSTANCE();

#define DEBUG false

String inputString = ""; 


//Define Gear motor pins
#define Gdir 2
#define Gpwm 3
int GmotorSPD = 0;


//Define Relay pins
#define RELAY1  4                        
#define RELAY2  5                        
#define RELAY3  6                        
#define RELAY4  7


//GLOBAL STEPPER PROPERTIES
#define homePosition 1000
int stepper_speed = 3200; //up to 6400, 25600;


//stepper A
#define step1DIR 8
#define step1STEP 9
#define step1_enablePin 10
#define limitPin1a 24 
#define limitPin1b 28
int limit_A = 0; //which limit pin are we tochin, 1 is home 2 is other
boolean stepperMove1 = false;
AccelStepper stepper1(1, step1STEP, step1DIR); //step / dir


//stepper B
#define step2DIR 11
#define step2STEP 12
#define step2_enablePin 13
#define limitPin2a 32
#define limitPin2b 36 
int limit_B = 0; //which limit pin are we tochin, 1 is home 2 is other
boolean stepperMove2 = false;
AccelStepper stepper2(1, step2STEP, step2DIR); //step / dir

//stepper C
#define step3DIR A0
#define step3STEP A1
#define step3_enablePin A2
#define limitPin3a 40
#define limitPin3b 44
int limit_C = 0; //which limit pin are we tochin, 1 is home 2 is other
boolean stepperMove3 = false;
AccelStepper stepper3(1, step3STEP, step3DIR); //step / dir


//stepper D
#define step4DIR A3
#define step4STEP A4
#define step4_enablePin A5
#define limitPin4a 48
#define limitPin4b 52
int limit_D = 0; //which limit pin are we tochin, 1 is home 2 is other
boolean stepperMove4 = false;
AccelStepper stepper4(1, step4STEP, step4DIR); //step / dir

void setup()
{
	
	//Gear Motor
	pinMode(Gdir,OUTPUT);//DIR
	digitalWrite(Gdir,LOW);//FW (HIGH=BACKWARDS)
	pinMode(Gpwm,OUTPUT);//PWM
	
	//Relays (solenoids)
	pinMode(RELAY1,OUTPUT);//relay1
	pinMode(RELAY2,OUTPUT);//relay2
	pinMode(RELAY3,OUTPUT);//relay3
	pinMode(RELAY4,OUTPUT);//relay4
	digitalWrite(RELAY1,LOW);//reset relay1
	digitalWrite(RELAY2,LOW);//reset relay2
	digitalWrite(RELAY3,LOW);//reset relay3
	digitalWrite(RELAY4,LOW);//reset relay4
	
	//stepper A
	stepper1.setMaxSpeed(stepper_speed + 30); //stepper max speed
	stepper1.setPinsInverted(false, false, true); //needed as the logic is inverted on the easydriver
	stepper1.setEnablePin(step1_enablePin); // set enable pins
	stepper1.enableOutputs(); //and enable the steppers
	pinMode(limitPin1a, INPUT_PULLUP); //arm the pullup pin32 on the limit button
	pinMode(limitPin1b, INPUT_PULLUP); //arm the pullup pin36 on the limit button
	
	//stepper B
	stepper2.setMaxSpeed(stepper_speed + 30); //stepper max speed
	stepper2.setPinsInverted(false, false, true); //needed as the logic is inverted on the easydriver
	stepper2.setEnablePin(step2_enablePin); // set enable pins
	stepper2.enableOutputs(); //and enable the steppers
	pinMode(limitPin2a, INPUT_PULLUP); //arm the pullup pin32 on the limit button
	pinMode(limitPin2b, INPUT_PULLUP); //arm the pullup pin36 on the limit button
	
	//stepper C
	stepper3.setMaxSpeed(stepper_speed + 30); //stepper max speed
	stepper3.setPinsInverted(false, false, true); //needed as the logic is inverted on the easydriver
	stepper3.setEnablePin(step3_enablePin); // set enable pins
	stepper3.enableOutputs(); //and enable the steppers
	pinMode(limitPin3a, INPUT_PULLUP); //arm the pullup pin32 on the limit button
	pinMode(limitPin3b, INPUT_PULLUP); //arm the pullup pin36 on the limit button
	
	//stepper D
	stepper4.setMaxSpeed(stepper_speed + 30); //stepper max speed
	stepper4.setPinsInverted(false, false, true); //needed as the logic is inverted on the easydriver
	stepper4.setEnablePin(step4_enablePin); // set enable pins
	stepper4.enableOutputs(); //and enable the steppers
	pinMode(limitPin4a, INPUT_PULLUP); //arm the pullup pin32 on the limit button
	pinMode(limitPin4b, INPUT_PULLUP); //arm the pullup pin36 on the limit button
	
	//run steppers until they hits the limit switch, this is the homing stage
	homeStepper1();    	
    homeStepper2();
	homeStepper3();
	homeStepper4();
	
	MIDI.begin(3);
	MIDI.setHandleNoteOn(HandleNoteOn);
	MIDI.setHandleNoteOff(HandleNoteOff);
}

void loop()
{
	//we "turn on" steppers in processInput function
	stepper1turn();
	stepper2turn();
	stepper3turn();
	stepper4turn();
	
	MIDI.read();
}

void HandleNoteOff(byte channel, byte pitch, byte velocity) {
 
  switch (pitch) {
		
		case 59:
		digitalWrite(Gpwm, LOW); //turn off Gmotor
		break;
		
		case 60:
		digitalWrite(RELAY1, LOW); //turn off relay1
                
		break;
		
		case 62:
		digitalWrite(RELAY2, LOW); //turn off relay2
		break;
		
		case 64:
		digitalWrite(RELAY3, LOW); //turn off relay3
		break;
		
		case 65:
		digitalWrite(RELAY4, LOW); //turn off relay4
		break;
}
}


void HandleNoteOn(byte channel, byte pitch, byte velocity) {
//midi keys
//q60 w62 e64 r65 t67 y69 
	switch (pitch) {
		
		case 59:
				GmotorSPD = velocity*2; // 128*2 = 255
				analogWrite(Gpwm,GmotorSPD); //control GMotor speed
				break;
		
		case 60:
				digitalWrite(RELAY1, HIGH); //turn on relay1
				break;
		
		case 62:
				digitalWrite(RELAY2, HIGH); //turn on relay2
				break;
		
		case 64:
				digitalWrite(RELAY3, HIGH); //turn on relay3
				break;
		
		case 65:
				digitalWrite(RELAY4, HIGH); //turn on relay4
				break;
		//midi keys u71 i72 o74 p76
		case 71:
				//turn stepper1 on
				stepperMove1 = true;
				break;
			
		case 72:
				//turn stepper2 on
				stepperMove2 = true;
				break;
			
		case 74:
				//turn stepper3 on
				stepperMove3 = true;
				break;
			
		case 76:
				//turn stepper4 on
				stepperMove4 = true;
				break;			
	}	
}

void stepper1turn()
{
	if(stepperMove1)
	{
		if(!digitalRead(limitPin1a) && limit_A==1)
		{
			limit_A = 2;
			stepperMove1 = false;
			stepper1.setSpeed(-stepper_speed);
		}
		else if(!digitalRead(limitPin1b) && limit_A==2)
		{
			limit_A = 1;
			stepperMove1 = false;
			stepper1.setSpeed(stepper_speed);
			}
		stepper1.runSpeed();
	}
}

void stepper2turn()
{
	if(stepperMove2)
	{
		if(!digitalRead(limitPin2a) && limit_B==1)
		{
			limit_B = 2;
			stepperMove2 = false;
			stepper2.setSpeed(-stepper_speed);
		}
		else if(!digitalRead(limitPin2b) && limit_B==2)
		{
			limit_B = 1;
			stepperMove2 = false;
			stepper2.setSpeed(stepper_speed);
		}
		stepper2.runSpeed();
	}
}

void stepper3turn()
{
	if(stepperMove3)
	{
		if(!digitalRead(limitPin3a) && limit_C==1)
		{
			limit_C = 2;
			stepperMove3 = false;
			stepper3.setSpeed(-stepper_speed);
		}
		else if(!digitalRead(limitPin3b) && limit_C==2)
		{
			limit_C = 1;
			stepperMove3 = false;
			stepper3.setSpeed(stepper_speed);
		}
		stepper3.runSpeed();
	}
}


void stepper4turn()
{
	if(stepperMove4)
	{
		if(!digitalRead(limitPin4a) && limit_D==1)
		{
			limit_D = 2;
			stepperMove4 = false;
			stepper4.setSpeed(-stepper_speed);
		}
		else if(!digitalRead(limitPin4b) && limit_D==2)
		{
			limit_D = 1;
			stepperMove4 = false;
			stepper4.setSpeed(stepper_speed);
		}
		stepper4.runSpeed();
	}
}

void homeStepper1()
{
	//run stepper until it hits the limit switch
	//the while is BLOCKING so we cant use it in main loop
	stepper1.setSpeed(stepper_speed);
	while(digitalRead(limitPin1a))
	stepper1.runSpeed();
	limit_A = 1;
}

void homeStepper2()
{
	stepper2.setSpeed(stepper_speed);
	while(digitalRead(limitPin2a))
	stepper2.runSpeed();
	limit_B = 1;
}

void homeStepper3()
{
	stepper3.setSpeed(stepper_speed);
	while(digitalRead(limitPin3a))
	stepper3.runSpeed();
	limit_C = 1;
}

void homeStepper4()
{
	stepper4.setSpeed(stepper_speed);
	while(digitalRead(limitPin4a))
	stepper4.runSpeed();
	limit_D = 1;
}

