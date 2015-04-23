
#include <MIDI.h>

 


#define Klaxon1  2                        
#define Klaxon2  3                        
#define Klaxon3  4                        
#define Klaxon4  5
#define Klaxon5  6                        
#define Klaxon6  7                        
#define Klaxon7  8                        
#define Klaxon8  9


void setup()
{
  
  //klaxons Relay pins
  pinMode(Klaxon1,OUTPUT);//RELAY1
  pinMode(Klaxon2,OUTPUT);//RELAY2
  pinMode(Klaxon3,OUTPUT);//RELAY3
  pinMode(Klaxon4,OUTPUT);//RELAY4
  pinMode(Klaxon5,OUTPUT);//RELAY5
  pinMode(Klaxon6,OUTPUT);//RELAY6
  pinMode(Klaxon7,OUTPUT);//RELAY7
  pinMode(Klaxon8,OUTPUT);//RELAY8
  
  //reset Relays
  digitalWrite(Klaxon1,LOW); //turn off Klaxon1
  digitalWrite(Klaxon2,LOW); //turn off Klaxon2
  digitalWrite(Klaxon3,LOW); //turn off Klaxon3
  digitalWrite(Klaxon4,LOW); //turn off Klaxon4
  digitalWrite(Klaxon5,LOW); //turn off Klaxon5
  digitalWrite(Klaxon6,LOW); //turn off Klaxon6
  digitalWrite(Klaxon7,LOW); //turn off Klaxon7
  digitalWrite(Klaxon8,LOW); //turn off Klaxon8
 
  // MIDI 
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI.setHandleNoteOn(HandleNoteOn);
  MIDI.setHandleNoteOff(HandleNoteOff);
 Serial.begin(31250);//MIDI31250
}
 
void loop()
{
	
  MIDI.read();	
  
}
 
void HandleNoteOff(byte channel, byte pitch, byte velocity) {
 
  switch (pitch) {
 
  case 48:
    digitalWrite(Klaxon1,LOW); //turn off Klaxon1
   
    break;
 
  case 49:
     digitalWrite(Klaxon2,LOW); //turn off Klaxon2
    break;
 
  case 50:
     digitalWrite(Klaxon3,LOW); //turn off Klaxon3
    break;
 
  case 51:
     digitalWrite(Klaxon4,LOW); //turn off Klaxon4
    break;	
    
    case 52:
     digitalWrite(Klaxon5,LOW); //turn off Klaxon5
   
    break;
 
  case 53:
    digitalWrite(Klaxon6,LOW); //turn off Klaxon6
    break;
 
  case 54:
    digitalWrite(Klaxon7,LOW); //turn off Klaxon7
    break;
 
  case 55:
     digitalWrite(Klaxon8,LOW); //turn off Klaxon8
    break;	
    
  }
}
 
void HandleNoteOn(byte channel, byte pitch, byte velocity) {
	
	
 switch (pitch) {
 
  case 48:
    digitalWrite(Klaxon1,HIGH); //turn on Klaxon1
   
    break;
 
  case 49:
     digitalWrite(Klaxon2,HIGH); //turn on Klaxon2
    break;
 
  case 50:
     digitalWrite(Klaxon3,HIGH); //turn on Klaxon3
    break;
 
  case 51:
     digitalWrite(Klaxon4,HIGH); //turn on Klaxon4
    break;	
    
    case 52:
     digitalWrite(Klaxon5,HIGH); //turn on Klaxon5
   
    break;
 
  case 53:
    digitalWrite(Klaxon6,HIGH); //turn on Klaxon6
    break;
 
  case 54:
    digitalWrite(Klaxon7,HIGH); //turn on Klaxon7
    break;
 
  case 55:
     digitalWrite(Klaxon8,HIGH); //turn on Klaxon8
    break;	
   
  
  }
 
}
 
 
 
 
 
 
 
 