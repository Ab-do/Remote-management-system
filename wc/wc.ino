// WC : wirless controller.
// le 20/02/2019 Agadir.
#include <SoftwareSerial.h>
#include<EEPROM.h>
const int AD_PHONE=10;
String Phone="";
int numberPhone[9]={0};        // nombre des objets de systeme
const int MTR=15;
void setup() {
  Serial.begin(9600);  // Moniteur série
  Serial1.begin(9600); // Module GSM  SIM800L
  Serial2.begin(9600); // Module radio HC12
  Serial.println("Start-up");
  Serial2.println("Start-up");
}

void loop() {
  //getDataGsm();
  getDataHc();
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
    Filtrage(data);
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
      Serial2.println(data);
    }else if( data[0]==50){
      data.remove(0,1);
      Serial2.println("<"+data+">");
    }else if (data[0]==51){
      sendSMS(data);
    }else if(data[0]==52){
      data.remove(0,1);
      setNumPhone(data);
    }if(data[0]==53){
      loadingData();
      }
    else {
      Serial.println("autre choix...");
    }
  }
  
}

void Filtrage(String str){
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

void sendSMS(String str){
  Serial.print("les données qui la fonction va analyser et l'envoyer par SMS au client.");
  Serial.println(str);
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
  Serial.print(Phone);
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
  Serial.println(str);
  return str; 
}
