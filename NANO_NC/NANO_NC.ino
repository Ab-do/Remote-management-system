#include <EEPROM.h>
#include<SoftwareSerial.h>
SoftwareSerial HC12(3, 2);
//#define CONFPIN 4
#define INFOCONTACT1 5
#define INFOCONTACT2 6
//#define LED 12
#define LEDOBJECT 12
#define RELAY1 10
#define RELAY2 11
#define ADRESS 22
#define ADRESS_STATE 44
#define ADRESS_NUMBER 55
int PINCODE;
int BottonState1;
int StateContact1;
int StateContact2;
unsigned long last=millis();
unsigned long lastRec=millis();
bool contact1 = true, contact2 = true;
int Obj=0,ObjNumber=0;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
  HC12.setTimeout(50);
  //pinMode(CONFPIN, INPUT);
  pinMode(INFOCONTACT1, INPUT);
  pinMode(INFOCONTACT2, INPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(LEDOBJECT, OUTPUT);
  digitalWrite(RELAY1, 1);
  digitalWrite(RELAY2, 1);
  contact1 = false;
  contact2 = false;
  EEPROM.get(ADRESS, PINCODE);
  EEPROM.get(ADRESS_STATE, Obj);
  EEPROM.get(ADRESS_NUMBER, ObjNumber);
  Serial.println("NANO");
  if(Obj==1 || 2 || 3 || 4 || 5){
    for(int g=0;g<Obj;g++){
      digitalWrite(LEDOBJECT, HIGH);
      delay(400);
      digitalWrite(LEDOBJECT, LOW);
     }
     delay(1000);
     for(int g=0;g<ObjNumber;g++){
      digitalWrite(LEDOBJECT, HIGH);
      delay(400);
      digitalWrite(LEDOBJECT, LOW);
     }
     delay(1000);
     digitalWrite(LEDOBJECT, HIGH);
  }else{
    digitalWrite(LEDOBJECT, HIGH);
    delay(500);
    digitalWrite(LEDOBJECT, LOW);
  }
   HC12.println("Q"+String(PINCODE));
   Serial.println("Q"+String(PINCODE));
   delay(1000);
}

void loop() {
  if(HC12.available() > 0) {
    String strg=HC12.readString();
    strg.trim();
    if(strg[0]=='C' && strg.length()<10){
    int cmdPin=(strg[1]-'0')*1000+(strg[2]-'0')*100+(strg[3]-'0')*10+(strg[4]-'0');
      if(cmdPin >1000 && cmdPin<6000){
      Switch(cmdPin);
      cmdPin = 0;
      }
    }else if(strg[0]=='X' && strg[1]=='R'){
      Obj=strg[2]-'0';
      ObjNumber=(strg[3]-'0')*10+(strg[4]-'0');
      PINCODE=Obj*1000+ObjNumber*10;
       Serial.println(Obj);
        Serial.println(ObjNumber);
         Serial.println(PINCODE);
      EEPROM.put(ADRESS, PINCODE);
      EEPROM.put(ADRESS_STATE, Obj);
      EEPROM.put(ADRESS_NUMBER, ObjNumber);
      delay(1500);
      HC12.println("9" + String(PINCODE));
    }
     HC12.flush();
  }
  InfoContact();
}




void configN()
{
  digitalWrite(LEDOBJECT, HIGH);
  delay(200);
  digitalWrite(LEDOBJECT, LOW);
  while (HC12.available() == 0) {
    digitalWrite(LEDOBJECT, HIGH);
    delay(500);
    digitalWrite(LEDOBJECT, LOW);
    delay(500);
  }
  
   while (HC12.available() > 0) {
    String str=HC12.readString();
    str.trim();
    if(str.length()<5 && str[0]=='J'){
      Obj=str[1]-48;
      ObjNumber=str[2]*10+str[3];
      PINCODE=Obj*1000+ObjNumber*10;
      EEPROM.put(ADRESS, PINCODE);
      EEPROM.put(ADRESS_STATE, Obj);
      EEPROM.put(ADRESS_NUMBER, ObjNumber);
      delay(1500);
      HC12.println("9" + String(PINCODE));
      break;
    }
  }
}

void Switch(int cmd)
{
  if (cmd > 1000 && cmd < 3000) {
    pmpFun(cmd);
  }
  else if (cmd > 4000 && cmd < 6000) {
    motorFun(cmd);
  }
}


void InfoContact()
{
  StateContact1 = digitalRead(INFOCONTACT1);
  if (StateContact1 == HIGH && contact1 == false && millis() - last > 400) {
    HC12.println("J" + String(PINCODE + 4));
    delay(50);
    contact1 = true;
    last = millis();
  } else if (StateContact1 == LOW && contact1 == true && millis() - last > 400) {
    HC12.println("J" + String(PINCODE + 3));
    delay(50);
    contact1 = false;
    last = millis();
  }
  
  if(Obj==4){
    StateContact2 = digitalRead(INFOCONTACT2);
    if (StateContact2 == HIGH && contact2 == false && millis() - last > 400)
      {
      HC12.println("J" + String(PINCODE + 1004));
      contact2 = true;
      last = millis();
      }else if(StateContact2 == LOW && contact2 == true && millis() - last > 400)
     {
      HC12.println("J" + String(PINCODE + 1003));
      contact2 = false;
      last = millis();
     }
  }
}

void pmpFun(int cmd)
{
  if (cmd == PINCODE + 1)
  {
    if(!contact1){
      HC12.println("J" + String(PINCODE + 3));
    }else {
      digitalWrite(RELAY1, LOW);
      delay(500);
      digitalWrite(RELAY1, HIGH);
    }
  }
  else if (cmd == PINCODE + 2)
  {
    if(contact1){
      delay(150);
      HC12.println("J" + String(PINCODE + 4));
    }else {
      digitalWrite(RELAY2, LOW);
      delay(500);
      digitalWrite(RELAY2, HIGH);
    }
  }
}

void motorFun(int cmd)
{
  if (cmd == PINCODE + 1) {
    if(!contact1){
        HC12.println("J" + String(PINCODE + 3));
        digitalWrite(RELAY1, LOW);
     }else {
        digitalWrite(RELAY1, LOW);
    }
  } else if (cmd == PINCODE + 2) {
    if(contact1){
       HC12.println("J" + String(PINCODE + 4));
       digitalWrite(RELAY1, HIGH);
    }else {
       digitalWrite(RELAY1, HIGH);
    }
  } else if (cmd == PINCODE + 1001) {
    if(!contact2){
       digitalWrite(RELAY2, LOW);
       HC12.println("J" + String(PINCODE + 1003));
    }else {
       digitalWrite(RELAY2, LOW);
    }
  } else if (cmd == PINCODE + 1002) {
    if(contact2){
       digitalWrite(RELAY2, HIGH);
       HC12.println("J" + String(PINCODE + 1004));
    }else {
      digitalWrite(RELAY2, HIGH);
    }
  }
}
