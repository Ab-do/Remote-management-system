// le 06/02/2019 Agadir
// Etape 2  : On ajoute une foncation de commutation pour Organiser le sotackage des données et pour effectuer la fonction appropriée.
#include <SoftwareSerial.h>


void setup() {
  // Initialisation les serials ( Moniteur, Gsm , Nextion et HC12)
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  
  

}

void loop() {
  getDataSerial();
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
         //Commutation des données.
         switchData(Matrix);
         memset(Matrix,0,sizeof(Matrix));
      }
      else{
         i++; 
      }     
  }
}
// Analyse, Commutation des données et Effectuation des actions.
void switchData(byte Matrix[30]){
  switch(Matrix[0]){
    case 49: // Byte : int + 48.
            // Inclure les données de paramétrage.
            Serial.println("Inclure les données de paramétrage.");
            break;
    case 50:
             // Définir les relations entre les objects + les conditions + les secteurs.
            Serial.println("Définir les relations entre les objects + les conditions.");
            break;
    case 51:
             // Effectuer une action sur un objet.
            Serial.println("Effectuer une action sur un objet.");
            break;
    case 52:
             // Fonctions liées à l'horloge.
            Serial.println("Fonctions liées à l'horloge.");
            break;
    case 53:
             // mettre les données à propos système.
            Serial.println("mettre les données à propos système.");
            break;
    case 54:
             // Obtenir les données à propos système.
            Serial.println("mettre les données à propos système.");
            break;
    case 55:
             // les fonctions à propos les paramétres.
            Serial.println("les fonctions à propos les paramétres.");
            break;
    case 57: // 9 
             //Protocole d'essai.
            Serial.println("Protocole d'essai .");
            break;

    default:
            break;
  }
}
