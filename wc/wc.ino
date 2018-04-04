// WC 8 : wirless controller.
// le 04/03/2019 Agadir.
#include <SoftwareSerial.h>
#include<EEPROM.h>
const int LED_START=2;
const int LED_WIRLESS_COM=3;
const int LED_CHECK_GSM=4;
const int AD_PHONE=10;
const int AD_SETTING_SMS=30;
String Phone="";
int numberPhone[9]={0};        // nombre des objets de systeme
int settingSMS[6]={0};
const int MTR=15;
unsigned long Last=millis();
unsigned long CheckSMS=millis();
bool SMS_Responce=false;
void setup() {
  Serial.begin(9600);  // Moniteur série
  Serial3.begin(9600); // Module GSM  SIM800L
  Serial2.begin(9600); // Module radio HC12
  Serial.println("Start-up");
  pinMode(LED_START,OUTPUT);
  pinMode(LED_WIRLESS_COM,OUTPUT);
  pinMode(LED_CHECK_GSM,OUTPUT);
  loadingData();
  Last=millis();
  CheckSMS=millis();
  //checkWirless();
  checkNet(true);
  //Serial3.println("AT+CMGF=1");
  //Serial3.println("AT+CNMI=1,2,0,0,0");
  digitalWrite(LED_START,HIGH);
  //Serial3.println("AT+CMGD=1,4");
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
    String data=Serial3.readString();
    Serial.println(data); // afficher str
    filtering(data);
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
          if(SMS_Responce==true && millis()-CheckSMS<20000 && settingSMS[0]!=1){
                Serial.println("SMS send");
                settingSMS[0]=1;
                switchSMS("9"+data);
                settingSMS[0]=2;
                SMS_Responce==false;
           }else {
                switchSMS("9"+data);
                SMS_Responce==false;
           }
      break;
      case 'P':
        data.remove(0,1);
        switchSMS("3"+data);
      break;
      case 'R':
        data.remove(0,1);
        Serial2.println(data);
        delay(500);
        //Serial2.println("<810"+data+">");
        //Serial.println("<810"+data+">");
      break;
      default:
        if(data=="T475"){
          restSys();
        }
      break;
    }
  }
}
//filtering data from GSM
void filtering(String str){
  String dataSend="";
  int i=0;
  while(i<str.length()){
      if(str[i]==60){
        i++;
        int j=0;
          while(str[i]!=62){
            dataSend +=str[i];
            j++; 
            i++;
            if(i>str.length()) return 0;
         }
         //Commutation des données.
         //showMatrix(Matrix,20);
         //switchData(Matrix);
         Serial.println(dataSend);
         Serial2.println(dataSend);
         SMS_Responce=true;
         CheckSMS=millis();
         //memset(Matrix,0,sizeof(Matrix));
      }
      else{
         i++; 
      }     
  }
}
//switchSMS 
void switchSMS(String str){
  Serial.print("les données ...");
  Serial.println(str);
  String msg;
  switch(str[0]-48){
    case 9:
         msg=getName(str[1]-48,toDec(str[2]-48,str[3]-48));
         if(str[4]-48==3){
            msg+=" ON";
         }else if(str[4]-48==4) {
            msg+=" OFF";
         }else if(str[4]-48==5) {
            msg+=" Erreur!";
         }
         Serial.println(msg);
         sendSMS(msg,settingSMS[0]);
    break;
    case 2:
       msg="démmarage de systeme..";
       sendSMS(msg,1);
    break;
    case 3:
       msg="votre PIN code : ";
       str.remove(0,1);
       msg+=str;
       Serial.println(msg);
       sendSMS(msg,1);
    break;
    case 4:
      
    break;
  };
  
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
  Serial.println(Phone);
  for(int i=0;i<4;i++) {
    Serial.print(settingSMS[i]);
    Serial.print("-");
  }
  Serial.println();
}

///// Envoie des notifications et des informations par SMS.
void sendSMS(String outMessage,int validity){
  Serial.print("SMS: ");
  Serial.println(outMessage);
  if(validity==1){
  Serial3.print("AT+CMGF=1\r");
  delay(500);
  if(Phone=="+212000000000"){
    Phone="+212770509044";
  }
  Serial3.println("AT + CMGS= \"+212" + Phone +"\"" );
  delay(500);
  Serial3.println(outMessage);
  delay(500);
  Serial3.write((char)26); //ctrl+z
  delay(500);
  Serial3.println("AT+CLTS=1");
  Serial3.println("AT+CCLK?");
  }
}

// Convertir une matrice en texte.
String toString(int Matrix[9]){
  String str="";
  for(int i=0;i<9;i++)
    str+=Matrix[i];
  return str; 
}

void smsExmple(int Matrix[]){
    String msg="";
    switch(Matrix[2]){
      case 1:
             msg=getName(Matrix[1],toDec(Matrix[2],Matrix[3]));
         if(Matrix[4]==3){
            msg+=" ON";
         }else if(Matrix[5]==4) {
            msg+=" OFF";
         }else if(Matrix[4]==5) {
            msg+=" Erreur!";
         }
      break;
    }
    sendSMS(msg,settingSMS[0]);
}

/// Obtenir le nome de l'obejct et leur numéro.
String getName(int Obj,int Number){
  switch(Obj){
    case 1 : //pim
    return "POMPE IMMERGEE "+String(Number)+" ";
    break;
    case 2 : //pr
    return "POMPE ROUF "+String(Number)+" ";
    break;
    case 3 : //Vn
    return "VANNE "+String(Number)+" ";
    break;
    case 4 : //Vn
    return "MELANGEUR "+String(Number)+" ";
    break;
    case 5 : //Vn
    return "PMP ENGRIS "+String(Number)+" ";
    break;
    case 6 : //Vn
    return "SECTEUR "+String(Number)+" ";
    break;
  }
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
void checkGsm(){
    delay(2000);
    if(Serial3.available()>0){
    String data=Serial3.readString();
    if(data.indexOf("AT+CSQ")==0){
       data.trim();
       data.remove(0,14);
       int GsmSgnal=data.toInt();
       if(GsmSgnal<10){
        Serial2.println("<820"+String(GsmSgnal)+">");
        Serial2.println("<820"+String(GsmSgnal)+">");
        digitalWrite(LED_CHECK_GSM,LOW);
       }else{
        Serial2.println("<82"+String(GsmSgnal)+">");
        Serial.println("<82"+String(GsmSgnal)+">");
        if(GsmSgnal<20){
           digitalWrite(LED_CHECK_GSM,LOW);
        }else{
           digitalWrite(LED_CHECK_GSM,HIGH);
        }
       }     
    }else {
       Serial2.println("<8200>");
       digitalWrite(LED_CHECK_GSM,LOW);
    }
  }else {
    Serial2.println("<8200>");
    digitalWrite(LED_CHECK_GSM,LOW);
  }
}
void checkNet(bool mode){
  if(millis()-Last>60000 || mode==true){
     Serial3.write("AT+CSQ\r");
     checkGsm();
     Serial.println("Check Net");
     Last=millis();
  }
}
void restSys(){
    for (int i = 1 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  setup();
  }
