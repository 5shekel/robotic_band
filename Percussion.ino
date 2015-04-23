// Percussion_v1
// classes

#include <MIDI.h>
#include <Streaming.h>
MIDI_CREATE_DEFAULT_INSTANCE();


#define DEBUG false

String inputString = ""; 


//Define Relay pins
#define RELAY1  2                        
#define RELAY2  3                        
#define RELAY3  4                        
#define RELAY4  5
#define RELAY5  6
#define RELAY6  7
#define RELAY7  9 //klapa



void setup()
{
	
	//Relays (solenoids)
	pinMode(RELAY1,OUTPUT);//relay1
	pinMode(RELAY2,OUTPUT);//relay2
	pinMode(RELAY3,OUTPUT);//relay3
	pinMode(RELAY4,OUTPUT);//relay4
	pinMode(RELAY5,OUTPUT);//relay5
	pinMode(RELAY6,OUTPUT);//relay6
        pinMode(RELAY7,OUTPUT);//relay7
        
	digitalWrite(RELAY1,LOW);//reset relay1
	digitalWrite(RELAY2,LOW);//reset relay2
	digitalWrite(RELAY3,LOW);//reset relay3
	digitalWrite(RELAY4,LOW);//reset relay4
	digitalWrite(RELAY5,HIGH);//reset relay5
	digitalWrite(RELAY6,HIGH);//reset relay6
        digitalWrite(RELAY7,LOW);//reset relay7
        
        
	MIDI.begin(4);//MIDI_CHANNEL_OMNI
	MIDI.setHandleNoteOn(HandleNoteOn);
	MIDI.setHandleNoteOff(HandleNoteOff);
}

void loop()
{
	MIDI.read();
}

void HandleNoteOff(byte channel, byte pitch, byte velocity) {
 
  switch (pitch) {
		
		
		case 60://79
		digitalWrite(RELAY1, LOW); //turn off relay1
                
		break;
		
		case 62://81
		digitalWrite(RELAY2, LOW); //turn off relay2
		break;
		
		case 64://83
		digitalWrite(RELAY3, LOW); //turn off relay3
		break;
		
		case 65://84
		digitalWrite(RELAY4, LOW); //turn off relay4
		break;
		
		case 67://86
		digitalWrite(RELAY5, HIGH); //turn off relay5
		break;
		
		case 69://88
		digitalWrite(RELAY6, HIGH); //turn off relay6
		break;

		case 71://88
		digitalWrite(RELAY7, LOW); //turn off relay7
		break;
}
}


void HandleNoteOn(byte channel, byte pitch, byte velocity) {
//midi keys
//q60 w62 e64 r65 t67 y69
	switch (pitch) {
		
	
		case 60://79
				digitalWrite(RELAY1, HIGH); //turn on relay1
				break;
		
		case 62://81
				digitalWrite(RELAY2, HIGH); //turn on relay2
				break;
		
		case 64://83
				digitalWrite(RELAY3, HIGH); //turn on relay3
				break;
		
		case 65://84
				digitalWrite(RELAY4, HIGH); //turn on relay4
				break;
		
		case 67://86
				
				digitalWrite(RELAY5, LOW); //turn on relay5
				break;
			
		case 69://88
				digitalWrite(RELAY6, LOW); //turn on relay6
				break;
		
		case 71://88
			digitalWrite(RELAY7, HIGH); //turn off relay7
			break;
	}	
}





