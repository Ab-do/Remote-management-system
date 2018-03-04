// WC 4 : wirless controller.
// le 2/03/2019 Agadir.
#include <SoftwareSerial.h>
#include<EEPROM.h>
const int AD_PHONE=10;
const int AD_SETTING_SMS=30;
String Phone="";
int numberPhone[9]={0};        // nombre des objets de systeme
int settingSMS[6]={0};
const int MTR=15;
void setup() {
  Serial.begin(9600);  // Moniteur série
  Serial1.begin(9600); // Module GSM  SIM800L
  Serial2.begin(9600); // Module radio HC12
  Serial.println("Start-up");
  Serial2.println("Start-up");
  loadingData();
}

void loop() {
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
  if(Serial1.available()>0){
    String data=Serial1.readString();
    Serial.println(data); // afficher str
    filtering(data);
  }
    
}

void getDataHc(){
  if(Serial2.available()>0){
    String data=Serial2.readString();
    data.trim();
    Serial.println(data);
    Serial.println(data.length());
    Serial.println(data.charAt(1));
    if(data[0]==49){
      data.remove(0,1);
      Serial2.println(data);
    }else if( data[0]==57){
      Serial2.println("<"+data+">");
      switchSMS("1"+data);
    }else if (data[0]==51){
      Serial.println("switch SMS");
      switchSMS(data);
    }else if(data[0]==52){
      data.remove(0,1);
      setNumPhone(data);
    } else {
      Serial.println("autre choix...");
    }
  }
  
}

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
         }
         //Commutation des données.
         //showMatrix(Matrix,20);
         //switchData(Matrix);
         Serial.println(dataSend);
         Serial1.println(dataSend);
         //memset(Matrix,0,sizeof(Matrix));
      }
      else{
         i++; 
      }     
  }
}

void switchSMS(String str){
  Serial.print("les données qui la fonction va analyser et l'envoyer par SMS au client.");
  Serial.println(str);
  String msg;
  switch(str[1]-48){
    case 9:
         msg=getName(str[2]-48,toDec(str[3]-48,str[4]-48));
         if(str[5]-48==3){
            msg+=" ON";
         }else if(str[5]-48==4) {
            msg+=" OFF";
         }else if(str[5]-48==5) {
            msg+=" Erreur!";
         }
         sendSMS(msg,settingSMS[0]);
    break;
    case 2:
       msg="démmarage de systeme..";
       sendSMS(msg,1);
    break;
    case 3:
       msg="votre PIN code : ";
       msg+=str.substring(2);
       sendSMS(msg,1);
    break;
    case 4:
      
    break;
  };
  
}


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

void loadingData(){
  EEPROM.get(AD_PHONE,numberPhone);
  Phone=toString(numberPhone);
  EEPROM.get(AD_SETTING_SMS,settingSMS);
  Serial.println(Phone);
  for(int i=0;i<6;i++) Serial.print(settingSMS[i]);
  Serial.println();
}

///// Envoie des notifications et des informations par SMS.
void sendSMS(String outMessage,int validity){
  Serial.print("SMS: ");
  Serial.println(outMessage);
  if(validity==1){
  Serial1.print("AT+CMGF=1\r");
  delay(500);
  if(Phone=="+212000000000"){
    Phone="+212770509044";
  }
  Serial1.println("AT + CMGS= \"" + Phone +"\"" );
  delay(500);
  Serial1.println(outMessage);
  delay(500);
  Serial1.write((char)26); //ctrl+z
  delay(500);
  Serial1.println("AT+CLTS=1");
  Serial1.println("AT+CCLK?");
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

void setSettingSMS(String str){
   for(int i=0;i<6;i++){
     settingSMS[i]=str[i+2]-48;
   }
   EEPROM.put(AD_SETTING_SMS,settingSMS);
}
/// Convertir deux nombres en un nombre décimal
int toDec(int o,int p){
  return o*10+p;
}
