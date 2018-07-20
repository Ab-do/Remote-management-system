#include <EEPROM.h>
#include<SoftwareSerial.h>
SoftwareSerial HC12(3, 2);
#define CONFPIN 4
#define INFOCONTACT1 5
#define INFOCONTACT2 6
//#define LED 12
#define LEDOBJECT 12
#define RELAY1 11
#define RELAY2 10
#define ADRESS 22
int PINCODE;
int BottonState1;
int StateContact1;
int StateContact2;
unsigned long last=millis();
unsigned long lastRec=millis();
bool contact1 = true, contact2 = true;
bool State=0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  HC12.begin(9600);
  HC12.setTimeout(50);
  pinMode(CONFPIN, INPUT);
  pinMode(INFOCONTACT1, INPUT);
  pinMode(INFOCONTACT2, INPUT);
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
//  pinMode(LED, OUTPUT);
  pinMode(LEDOBJECT, OUTPUT);
  digitalWrite(RELAY1, 1);
  digitalWrite(RELAY2, 1);
  EEPROM.read(ADRESS, PINCODE);
  
  Object();
  Serial.println(PINCODE);
  Serial.println(State);
  if(State>0){
    digitalWrite(LEDOBJECT, HIGH);
  }else{
    digitalWrite(LEDOBJECT, HIGH);
    delay(1000);
    digitalWrite(LEDOBJECT, LOW);
    delay(1000);
    digitalWrite(LEDOBJECT, HIGH);
    delay(1000);
    digitalWrite(LEDOBJECT, LOW);
  }
  Serial.println("88888");
}
void Object()
{
  int i, j;
  if (PINCODE > 1000 && PINCODE < 2000)
  {
    i = 1;
    j = (PINCODE - 1000) / 10;
    State=1;
  }
  else if (PINCODE > 2000 && PINCODE < 3000)
  {
    i = 2;
    j = (PINCODE - 2000) / 10;
    State=1;
  }  else if(PINCODE > 4000 && PINCODE < 5000)
  {
    i = 4;
    j = (PINCODE - 4000) / 10;
    State=2;
  }else {
    State=0;
  }
  if(State!=0){
    delay(1000);
    for (int k = 0; k < i; k++) {
    digitalWrite(LEDOBJECT, HIGH);
    delay(1000);
    digitalWrite(LEDOBJECT, LOW);
    delay(500);
    }
    delay(1000);
    for (int k = 0; k < j; k++) {
    digitalWrite(LEDOBJECT, HIGH);
    delay(100);
    digitalWrite(LEDOBJECT, LOW);
    delay(100);
    }
  }
  
}
void loop() {
  BottonState1 = digitalRead(CONFPIN);
  if (BottonState1 == 1) configN();
  if(HC12.available() > 0) {
    if(HC12.read()=='C'){
    String str = HC12.readString();
    str.trim();
    Serial.println("commmand : "+str);
    if(str.length()<6){
      int cmd=stringToint(str);
      controle(cmd);
      cmd = 0;
    }
     HC12.flush();
    }else {
       HC12.flush();
    }
  }
  InfoContact();
}
int stringToint(String val){
  int valeur=((val[0]-'0')*1000)+((val[1]-'0')*100)+((val[2]-'0')*10)+(val[3]-'0');
  return valeur;
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
    int data = HC12.parseInt();
    if (data > 1000 && data < 3000 || data > 4000 && data < 6000) {
      EEPROM.put(ADRESS, data);
      for (int i = 0; i < 2; i++) {
        digitalWrite(LEDOBJECT, HIGH);
        delay(50);
        digitalWrite(LEDOBJECT, LOW);
        delay(50);
      }
      delay(1500);
      HC12.println("9" + String(data));
      setup();
    }
    break;
  }
}
void controle(int cmd)
{
  if (cmd > 1000 && cmd < 3000) {
    pmpFun(cmd);
  }
  else if (cmd > 4000 && cmd < 6000) {
    motorFun(cmd);
  }
}
void motorFun(int cmd)
{
  if (cmd == PINCODE + 1) {
    if(!contact1){
          delay(1500);
          HC12.println("J" + String(PINCODE + 3));
    }else {
      digitalWrite(RELAY1, LOW);
    }
  }
  else if (cmd == PINCODE + 2) {
    if(contact1){
       delay(1500);
       HC12.println("J" + String(PINCODE + 4));
    }else {
    digitalWrite(RELAY1, HIGH);
    }
  } else if (cmd == PINCODE + 1001) {
    if(!contact2){
       delay(1500);
       HC12.println("J" + String(PINCODE + 1003));
    }else {
      digitalWrite(RELAY2, LOW);
    }
  } else if (cmd == PINCODE + 1002) {
    if(contact2){
      delay(1500);
      HC12.println("J" + String(PINCODE + 1004));
    }else {
    digitalWrite(RELAY2, HIGH);
    }
  }
}

void pmpFun(int cmd)
{
  if (cmd == PINCODE + 1)
  {
    if(!contact1){
      delay(1500);
      HC12.println("J" + String(PINCODE + 3));
    }else {
      digitalWrite(RELAY1, LOW);
      delay(1500);
      digitalWrite(RELAY1, HIGH);
    }
  }
  else if (cmd == PINCODE + 2)
  {
    if(contact1){
      delay(1500);
      HC12.println("J" + String(PINCODE + 4));
    }else {
      digitalWrite(RELAY2, LOW);
      delay(1000);
      digitalWrite(RELAY2, HIGH);
    }
  }
}

void InfoContact()
{
  StateContact1 = digitalRead(INFOCONTACT1);
  StateContact2 = digitalRead(INFOCONTACT2);
  if (StateContact1 == HIGH && contact1 == false && millis() - last > 250) {
    delay(1500);
    HC12.println("J" + String(PINCODE + 4));
    //HC12.println("<810" + String(PINCODE + 4)+">"); //<8104013>

    contact1 = true;
    last = millis();
  } else if (StateContact1 == LOW && contact1 == true && millis() - last > 250) {
    delay(1500);
    HC12.println("J" + String(PINCODE + 3));
    //HC12.println("<810" + String(PINCODE + 3)+">");
    contact1 = false;
    last = millis();
  }else if (StateContact2 == HIGH && contact2 == false && PINCODE > 4000 && PINCODE < 6000 && millis() - last > 250)
  {
    delay(1500);
    HC12.println("J" + String(PINCODE + 1004));
    contact2 = true;
    last = millis();
  }
  else if (StateContact2 == HIGH && contact2 == false && PINCODE > 1000 && PINCODE < 3000 && millis() - last > 250)
  { 
    delay(1500);
    HC12.println("J" + String(PINCODE + 5));
    contact2 = true;
    last = millis();
  }else if (StateContact2 == LOW && contact2 == true && PINCODE > 4000 && PINCODE < 6000 && millis() - last > 250)
  { 
    delay(1500);
    HC12.println("J" + String(PINCODE + 1003));
    contact2 = false;
    last = millis();
  }
  else if (StateContact2 == LOW && contact2 == true && PINCODE > 1000 && PINCODE < 3000 && millis() - last > 250)
  { 
    delay(1500);
    HC12.println("J" + String(PINCODE + 6));
    contact2 = false;
    last = millis();
  }
}
