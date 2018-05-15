// WC 8 : wirless controller.
// le 04/03/2018 Agadir.
#include <SoftwareSerial.h>
#include<EEPROM.h>
#include "SIM800L.h"
const int LED_START=3;
const int LED_WIRLESS_COM=2;
const int LED_CHECK_GSM=4;
const int AD_PHONE=10;
const int AD_SETTING_SMS=30;
const int AD_CONT_SMS=60;
String Phone="";
uint8_t index=1;
String textSms;
String numberSms;
uint8_t index1;
uint8_t LED2=13; // use what you need
bool error;
int numberPhone[9]={0};        // nombre des objets de systeme
int settingSMS[6]={0};
int GsmSgnal=0;
int ContSMS=0;
const int MTR=15;
unsigned long Last=millis();
unsigned long CheckSMS=millis();
bool SMS_Responce=false;
SIM800L GSMmodule;
void setup() {
  Serial.begin(9600);  // Moniteur série
  Serial3.begin(9600); // Module GSM  SIM800L
  GSMmodule.begin();
  Serial2.begin(9600); // Module radio HC12
  Serial.println("Start-up");
  pinMode(LED_START,OUTPUT);
  pinMode(LED_WIRLESS_COM,OUTPUT);
  pinMode(LED_CHECK_GSM,OUTPUT);
  loadingData();
  Last=millis();
  CheckSMS=millis();
  //checkWirless();
  delay(2500);
  checkNet(true);
  digitalWrite(LED_START,HIGH);
  Serial3.println("AT+CMGF=1");
  Serial3.println("AT+CNMI=3,3,3,0,0");
  //Serial.println(GSMmodule.dateNet());
  //GSMmodule.setPhoneFunctionality();
  sendSMS("GB-9467",1);
  //switchSMS("2"); // tm
}

void loop() {
  checkNet(false);
  getDataGsm();
  getDataHc();
  getDataSerial();
}

//  obtenir les donnée
void getDataSerial(){
  if(Serial.available()>0){
    String data=Serial.readString();
    Serial.println(data); // afficher str
  }
}

void getDataGsm(){
  if(Serial3.available()>0){
      textSms=GSMmodule.readSms(index);
      if(textSms.indexOf("OK")!=-1){
      numberSms=GSMmodule.getNumberSms(index);
      Serial.println("Num de tel : " + numberSms);
      int _idx = textSms.indexOf("\"\r");
      Serial.println(_idx);
      int _idx2 = textSms.indexOf("\r",_idx+3);
      Serial.println(_idx2);
      textSms=textSms.substring(_idx+3,_idx2);
      Serial.println("Msg : "+textSms);
      if(numberSms.indexOf("669600729")!=-1 || numberSms.indexOf(Phone)!=-1){
          if(numberSms.indexOf("FXP")!=-1){
             if(textSms[3]=='R'){
                ContSMS=0;
                EEPROM.put(AD_CONT_SMS,ContSMS);
                checkConx();
             }else if (textSms[3]=='E'){
                checkConx();
             }else if(textSms[3]=='D'){
              int op=textSms[4];
               textSms.remove(0,4);
               recharge(op,textSms);
             }
          }else {
            Serial2.println(numberSms);
          }
      }else {
        sendSMS(numberSms+"\n"+textSms,1);
      }
      Serial3.println("AT+CMGD=1,4");
   }
  }

}

void getDataHc(){
  if(Serial2.available()>0){
    String data=Serial2.readString();
    data.trim();
    Serial.print(data+" nChar : ");
    Serial.println(data.length());
    switch(data[0]){
      case 'W':
         if(data[1]=='c'){
          checkWirless();
          checkNet(true);
         }else if(data[1]=='d'){
          sendSMS("GB-9478",1);
          checkNet(true);
         }else if(data[1]=='e'){
          checkConx();
         }else if(data[1]=='r'){
          ContSMS=0;
          EEPROM.put(AD_CONT_SMS,ContSMS);
          checkConx();
         }
      break;
      case 'N':
        data.remove(0,1);
        setNumPhone(data);
      break;
      case 'S':
        data.remove(0,1);
        Serial.println(data);
        setSettingSMS(data);
      break;
      case '8':
          data.remove(0,1);
          Serial.println(data);
          Serial2.println("<810"+data+">");
          //switchSMS("1"+data);
          if(SMS_Responce==true && millis()-CheckSMS<20000 && settingSMS[1]!=1){
                Serial.println("SMS send");
                sendSMS("Gx21"+data,1);
                SMS_Responce==false;
           }else {
                sendSMS("Gx21"+data,settingSMS[1]);
                SMS_Responce==false;
           }
      break;
      case 'P':
        data.remove(0,1);
        //switchSMS("3"+data);
      break;
      case 'R':
        data.remove(0,1);
        Serial2.println(data);
        delay(500);
        //Serial2.println("<810"+data+">");
        //Serial.println("<810"+data+">");
      break;
      case 'G':
        sendSMS(data,1);
      break;
      default:
        if(data=="T475"){
          restSys();
        }
      break;
    }
  }
}

//fonction set nemero de telephone du client
void setNumPhone(String str){
    Serial.println("Mettre Numéro de Tel.");
    //showMatrix(Matrix,11);
    for(int i=0;i<9;i++){
      numberPhone[i]=str[i]-48;
    }
    Phone=toString(numberPhone);
    EEPROM.put(AD_PHONE,numberPhone);
    Serial.print(Phone);
    //showMatrix(numberPhone,3);
}
//load data from EEPROM
void loadingData(){
  EEPROM.get(AD_PHONE,numberPhone);
  Phone=toString(numberPhone);
  EEPROM.get(AD_SETTING_SMS,settingSMS);
  EEPROM.get(AD_CONT_SMS,ContSMS);
  Phone="669600729";
  Serial.println(Phone);
  for(int i=0;i<4;i++) {
    Serial.print(settingSMS[i]);
    Serial.print("-");
  }
  Serial.println();
}

///// Envoie des notifications et des informations par SMS.
void sendSMS(String outMessage,int validity){
  if(validity==1){
  if(GSMmodule.sendSms("+212"+Phone,outMessage.c_str())){
    if(ContSMS<9999){
        ContSMS++;
    }else {
        ContSMS=1;
    }
      EEPROM.put(AD_CONT_SMS,ContSMS);
    }else {
      Serial.println("Er d'env SMS");
    }
  }
}

// Convertir une matrice en texte.
String toString(int Matrix[9]){
  String str="";
  for(int i=0;i<9;i++)
    str+=Matrix[i];
  return str;
}



//fonction set setting SMS
void setSettingSMS(String str){
   for(int i=0;i<4;i++){
     settingSMS[i]=str[i]-48;
   }
   Serial.println(str);
   EEPROM.put(AD_SETTING_SMS,settingSMS);
}
/// Convertir deux nombres en un nombre décimal
int toDec(int o,int p){
  return o*10+p;
}

void checkWirless(){
    digitalWrite(LED_WIRLESS_COM,HIGH);
    delay(70);
    digitalWrite(LED_WIRLESS_COM,LOW);
    delay(70);
    digitalWrite(LED_WIRLESS_COM,HIGH);
    //Serial2.println("<8200>");
}

void checkNet(bool mode){
   if(millis()-Last>60000 || mode==true){
      GsmSgnal=GSMmodule.signalQuality();
      if(GsmSgnal>20){
        Serial2.println("<82"+toString(GsmSgnal)+">");
        digitalWrite(LED_CHECK_GSM,HIGH);
      }else {
        Serial2.println("<82"+toString(GsmSgnal)+">");
        digitalWrite(LED_CHECK_GSM,LOW);
      }
     Serial.println("Niveau signal : "+toString(GsmSgnal));
     Last=millis();
  }
}

void checkConx(){
  String msg="GB-91";
  msg+=toString(GsmSgnal);
  msg+=toStringPin(ContSMS);
  sendSMS(msg,1);
  Serial2.println(msg);
}
void restSys(){
    for (int i = 1 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  setup();
  }

String toString(int value){
  if(value<10){
    return "0"+String(value);
  }else {
    return String(value);
  }
}
String toStringPin(int value){
  if(value<10){
    return   "000"+String(value);
  } if(value <100) {
     return "00"+String(value);
  } if(value <1000) {
     return  "0"+String(value);
  }else {
    return String(value);
  }
}

void recharge(int Operator,String recCode){
  if(Operator==1){
    recCode+="*1";
    GSMmodule.sendSms("555",recCode.c_str());
  }else if(Operator==2){
    GSMmodule.sendSms("555",recCode.c_str());
  }

}
