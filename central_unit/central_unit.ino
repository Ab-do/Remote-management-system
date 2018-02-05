// le 05/02/2019 Agadir
#include <SoftwareSerial.h>


void setup() {
  // Initialisation les serials ( Moniteur, Gsm , Nextion et HC12)
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  
  

}

void loop() {
  

}
//********* OBTEBNIR LES DONNEES
// Obtenir des données de Serial
void getDataSerial(){
  if(Serial.available()>0){
    String str=Serial.readString();
    Serial.println(str); // afficher str
    strToMatrix(str);
  }
}
// Obtenir des données de GSM
void getDataGsm(){
  if(Serial1.available()>0){
    String str=Serial1.readString();
    Serial.println(str); // afficher str
    strToMatrix(str);
  }
}
// Obtenir des données de Nextion
void getDataNextion(){
  if(Serial2.available()>0){
    String str=Serial2.readString();
    Serial.println(str); // afficher str
    strToMatrix(str);
  }
}


// Convertir de string à matrice
void strToMatrix(String str){
  byte Matrix[30];
  int i=0;
  while(i<str.length()){
      if(str[i]==60){
        i++;
        int j=0;
          while(str[i]!=62){
            Matrix[j]=str[i];
            j++; 
            i++;
         }
         //Effuctier une action
         
         memset(Matrix,0,sizeof(Matrix));
      }
      else{
         i++; 
      }     
  }
}
