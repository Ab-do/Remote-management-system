
// WC 8 : wirless controller.
#include <SoftwareSerial.h>
#include<EEPROM.h>
#include "SIM800L.h"
#define TIME_WAIT 18000
#define CAS_NAM 3
struct waiting {
  bool Rep=false;
  bool waiting=false;
  String PIN;
  unsigned long _time=millis(); 
  bool app=false;
};
waiting val[CAS_NAM];
const int LED_START=3;
const int LED_WIRLESS_COM=2;
const int LED_CHECK_GSM=4;
const int AD_PHONE=10;
const int AD_SETTING_SMS=70;
const int AD_CONT_SMS=100;
const int NBR_CHAR_PHONE=10;
const int AD_L_S=200;
//String Phone="";
String PHONE_ADM="669600729";
uint8_t index=1;
String textSms;
String numberSms;
String User[3];
uint8_t index1;
uint8_t LED2=13; // use what you need 
bool error;
bool GSMcon=false;
bool Ls=false;
int numberPhone[NBR_CHAR_PHONE][3]={0};        // nombre des objets de systeme
int settingSMS[4]={0};
int GsmSgnal=-1;
int ContSMS=0;
int ContErr=0;
const int MTR=15;
unsigned long Last=millis();
unsigned long CheckSMS=millis();
unsigned long LastHC=millis();
unsigned long replayCmd=millis();
bool SMS_Responce=false;
bool waitingPIN=false;
bool starRep=false;
int AdminLes=0;
SIM800L GSMmodule;
void setup() {
  Serial.begin(9600);  // Moniteur série
  Serial3.begin(9600); // Module GSM  SIM800L
  GSMmodule.begin();
  Serial2.begin(9600); // Module radio HC12
  Serial2.setTimeout(15);
  //Serial.println("Start-up");
  pinMode(LED_START,OUTPUT);
  pinMode(LED_WIRLESS_COM,OUTPUT);
  pinMode(LED_CHECK_GSM,OUTPUT);
  loadingData();
  Last=millis();
  CheckSMS=millis();
  LastHC=millis();
  //checkWirless();
  delay(2500);
  //Serial.println("1");
  checkNet(true);
  //Serial.println("2");
  digitalWrite(LED_START,HIGH);
  Serial3.println("AT+CMGF=1");
  Serial3.println("AT+CNMI=3,3,0,0,0");
  delay(300);
  sendSMS("GB-9467",1,3);
  Serial3.println(F("AT+CMGDA=\"DEL ALL\""));
  delay(300);
  Serial2.println("<83>");
}

void loop() {
  checkNet(false);
  getDataGsm();
  getDataHc();
  waitReplay();
}

//  obtenir les donnée 


void getDataGsm(){
  if(Serial3.available()>0){
      textSms=GSMmodule.readSms(index); 
      if(textSms.indexOf("OK")!=-1){
      numberSms=GSMmodule.getNumberSms(index); 
      //Serial.println("Num de tel : " + numberSms);
      int _idx = textSms.indexOf("\"\r");
      //Serial.println(_idx);
      int _idx2 = textSms.indexOf("\r",_idx+3);
      //Serial.println(_idx2);
      textSms=textSms.substring(_idx+3,_idx2);
      //Serial.println("Msg : "+textSms);
      int idUser=-1;
      if(textSms.indexOf("Hes0x")!=-1){
        idUser=5;
      }
      for(int m=0;m<3;m++){
        if(numberSms.indexOf(User[m])!=-1){
          idUser=m;
          break;
        }
      }
      
      if(idUser!=-1 || numberSms.indexOf(PHONE_ADM)!=-1){
          if(textSms.indexOf("FXP")!=-1){
            textSms.trim();
             if(textSms[3]=='R'){
                ContSMS=0;
                EEPROM.put(AD_CONT_SMS,ContSMS);
                checkConx();
             }else if (textSms[3]=='E'){
                checkConx();
             }else if(textSms[3]=='D'){
              int op=textSms[4]-'0';
               textSms.remove(0,4);
               recharge(op,textSms);
             }else if(textSms[3]=='P'){
               int id=textSms[4]-'0';
               numberPhone[0][id]=textSms[5]-'0';
               textSms.remove(0,6);
                 if(id==0 || id==1 || id==2){
                  //Serial2.println(textSms);
                  setNumPhone(textSms,id);
                 }
             } else if(textSms[3]=='Y'){
              
               AdminLes=textSms[4]-'0';
              }
          }else {
            Serial2.println(textSms);
          }
      }else {
        sendSMS(numberSms+"\n"+textSms,1,1);
      }
      delay(30);
      Serial3.println("AT+CMGD=1");  
      GSMmodule.delAllSms();
   }
   Last=millis();
   textSms="";
   numberSms="";
  }
   
}

void getDataHc(){
  if(Serial2.available()>1){
    String data=Serial2.readString();
    data.trim();
    if(data.length()<30){
    switch(data[0]){
      case 'W':
         digitalWrite(LED_WIRLESS_COM,HIGH);
         if(data[1]=='c'){
          checkWirless();
          checkNet(true);
         }else if(data[1]=='d'){
          sendSMS("GB-9478",1,3);
          checkNet(true);
          }
      break;
      case 'M':
        data.remove(0,1);
        numberPhone[0][0]=1;
        setNumPhone(data,0);
      break;
      case 'S':
        data.remove(0,1);
        setSettingSMS(data);
      break;
      case 'J':
          data.remove(0,1);
          delay(400);
          if(Ls==true){
            delay(5000);
            if(data[3]=='3'){
              data[3]='2';
              data="C"+data;
              Serial2.println(data);
            }
          }else {
            Serial2.println("<818"+data+">");
            if(SMS_Responce==true && millis()-CheckSMS<20000 && settingSMS[0]!=1){
                  //Serial.println("SMS send");
                  sendSMS("Gx21"+data,1,3);
                  SMS_Responce==false;
             }else {
                  if(data[3]=='3' || data[3]=='4'){
                    sendSMS("Gx21"+data,settingSMS[0],3);
                  }else {
                    sendSMS("Gx21"+data,settingSMS[2],3);
                  }
                  SMS_Responce==false;
             }
             checkRep(data);
          }
      break;
      case 'P':
        data.remove(0,1);
        sendSMS("PIN-"+data,1,1);
      break;
      case 'R':
        data=data.substring(1,5);
        delay(200);
        Serial2.println("C"+data);
        saveToWaiting(data);
      break;
      case 'G':
        sendSMS(data,1,3);
      break;
      case 'F':
        data.remove(0,1);
        sendSMS("GC"+data,9,1);
      break;
      default:
        if(data=="T475"){
          restSys();
        }else if(data=="T795") {
          Ls=true;
          EEPROM.put(AD_L_S,Ls);
        }else if(data=="T735") {
          Ls=false;
          EEPROM.put(AD_L_S,Ls);
        }
      break;
    }
    }
    Serial2.flush();
  }
}

//fonction set nemero de telephone du client
void setNumPhone(String str,int id){
  str.trim();
  String msg="";
  if(str.length()==NBR_CHAR_PHONE-1){
    for(int i=1;i<NBR_CHAR_PHONE;i++){
      numberPhone[i][id]=str[i-1]-48;
    }
    User[id]=getNumPhone(numberPhone,id);
    //Serial2.print("id : "+String(id)+" Phone:"+User[id]);
    if(EEPROM.put(AD_PHONE,numberPhone)){
      msg="PH"+String(id)+"Y";
    }
       
  }else if(str.length()==0) {
    if(numberPhone[0][id]==3){
         for(int i=1;i<NBR_CHAR_PHONE;i++){
         numberPhone[i][id]=-48;
         }
        User[id]="";
        if(EEPROM.put(AD_PHONE,numberPhone)){
          msg="PH"+String(id)+"D";
        }
    }else if(numberPhone[0][id]==2) {
        if(EEPROM.put(AD_PHONE,numberPhone)){
          msg="PH"+String(id)+"N";
        }
    }else if(numberPhone[0][id]==1) {
        if(EEPROM.put(AD_PHONE,numberPhone)){
          delay(200);
          msg="PH"+String(id)+"A";
        }
    }
    
  }
  if(id==0 && msg.length()>0){
      sendSMS(msg,1,1);
    }else if(msg.length()>0) {
          delay(200);                                                                                                                                                                                
          GSMmodule.sendSms("+212"+User[id],msg);
          delay(200);
          sendSMS(msg,1,1);
    }
}
//load data from EEPROM
void loadingData(){
  EEPROM.get(AD_PHONE,numberPhone);
  User[0]=getNumPhone(numberPhone,0);
  User[1]=getNumPhone(numberPhone,1);
  User[2]=getNumPhone(numberPhone,2);
  EEPROM.get(AD_SETTING_SMS,settingSMS);
  EEPROM.get(AD_CONT_SMS,ContSMS);
  EEPROM.get(AD_L_S,Ls);
}

///// Envoie des notifications et des informations par SMS.
void sendSMS(String outMessage,int validity ,int Priority){
  if(outMessage.length()<160 && (AdminLes==0 || AdminLes==1)){
  if(validity==1  && settingSMS[3]==1){ //&& GsmSgnal!=-1
    for(int g=0;g<Priority;g++){
      if(numberPhone[0][g]==1){
         if(GSMmodule.sendSms("+212"+User[g],outMessage.c_str())){
            if(ContSMS<9999){
              ContSMS++;
            }else {
              ContSMS=1;
            }
            EEPROM.put(AD_CONT_SMS,ContSMS);
         }else {
            // Serial2.println("<86>");
         }
    }
    }
    
    Last=millis();
    delay(100);
    GSMmodule.delAllSms();
   
  }
  if(AdminLes==1 || AdminLes==2 || validity==9){
    delay(400);
    GSMmodule.sendSms("+212"+PHONE_ADM,outMessage.c_str());
  }
  }
}

// Convertir une matrice en texte.
String getNumPhone(int Matrix[NBR_CHAR_PHONE][3],int id){
  String str="";
  for(int i=1;i<NBR_CHAR_PHONE;i++)
    str+=Matrix[i][id];
  return str; 
}



//fonction set setting SMS
void setSettingSMS(String str){
   for(int i=0;i<4;i++){
     settingSMS[i]=str[i]-48;
   }
   //Serial.println(str);
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
    //Serial2.println("<8200>"); tm
}

void checkNet(bool mode){
   if(millis()-Last>60000 || mode==true){
      GsmSgnal=GSMmodule.signalQuality();
      if(GsmSgnal==-1){
        digitalWrite(LED_CHECK_GSM,LOW);
        GSMcon=true;
      }else if(GsmSgnal>20){
        Serial2.println("<82"+toString(GsmSgnal)+">");
        GSMcon=true;
        digitalWrite(LED_CHECK_GSM,HIGH);
      }else {
        Serial2.println("<82"+toString(GsmSgnal)+">");
        GSMcon=false;
        digitalWrite(LED_CHECK_GSM,LOW);
      }
     Last=millis();
  }
}

void checkConx(){
  String msg="GB-91";
  msg+=toString(GsmSgnal); //
  msg+=toStringPin(ContSMS);
  sendSMS(msg,1,3);
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

void waitReplay(){
      String msg="<818";
      for(int i=0;i<CAS_NAM;i++){
        if(val[i].waiting==true){
          if(millis()-val[i]._time>TIME_WAIT && val[i].Rep==false){
            msg+=val[i].PIN+"9>";
            delay(80);
            Serial2.println(msg);
            sendSMS("Gx21"+val[i].PIN+"9",settingSMS[3],3);
            val[i].waiting=false;
            val[i].Rep=false;
            val[i].PIN="";
            val[i]._time=millis();
          }
        }
      }
}

void saveToWaiting(String PIN){
  int i=0;
  while(val[i].waiting==true){
    i++;
    if(i==CAS_NAM){
      i=0;
      break;
    }
  }
  PIN.remove(3,1);
  val[i].waiting=true;
  val[i].Rep=false;
  val[i].PIN=PIN;
  val[i]._time=millis();
}

bool checkRep(String PIN){
    for(int i=0;i<CAS_NAM;i++){
      if(val[i].waiting==true && PIN.indexOf(val[i].PIN)!=-1){
        val[i].waiting=false;
        val[i].Rep=true;
        val[i].PIN="";
      }
    }
}


void lockSys(){
  
}
