// le 07/02/2019 Agadir
// Etape 4  : les fonction pricipaux des commutations
#include <SoftwareSerial.h>
#include <EEPROM.h>
const int MTR=30;  // Nombre de Rangé de la matrice

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
// Obtenir des données d Module radio HC12
void getDataHc(){
  if(Serial3.available()>0){
    String str=Serial3.readString();
    Serial.println(str); // afficher str
    strToMatrix(str);
  }
}


// Convertir de string à matrice
void strToMatrix(String str){
  byte Matrix[MTR];
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
void switchData(byte Matrix[MTR]){
  switch(Matrix[0]){
    case 49: // Byte : int + 48.
            // Inclure les données de paramétrage.
            Serial.println("Inclure les données de paramétrage.");
            if(Matrix[1]==49){
              // les fonction de paramétrage.
              if(Matrix[2]==49){
                setObj(Matrix);
              }else if(Matrix[2]==50){
                setRelation(Matrix);
              }else if(Matrix[2]==50){
                setSec(Matrix);
              }else {
                // Erreur.
              Error();
              }
            }else if(Matrix[1]==50){
              // Mettre les informations.
              if(Matrix[2]==49){
                setNumPhone(Matrix);
              }else if(Matrix[2]==50){
                setPIN(Matrix);
              }else {
                Error();
              }
            }else {
              // Erreur.
              Error();
            }
            break;
    case 50:
             //  // Effectuer une action sur un objet.
            Serial.println("// Effectuer une action sur un objet.");
            if(Matrix[1]==49){
              actionObj(Matrix); 
            }else if(Matrix[1]==50){
              progObj(Matrix);
            }else {
              Error();
            }
            break;
    case 51:
             // Fonctions liées à l'horloge.
            Serial.println("// Fonctions liées à l'horloge.");
            if(Matrix[1]==49){ // Réglage la date et l'heure
              setTime(Matrix);
            }else if(Matrix[1]==50) { // Demmande la date et l'heure
              getTime();
            }else {
              Error();
            }
            break;
    case 52:
             // mettre les données à propos système.
            Serial.println("// mettre les données à propos système.");
            switch(Matrix[1]){
              case 49:
                showHist(Matrix[2]);
              break;
              case 50:
                showState();
              break;
              case 51:
                showProg(Matrix[2]);
              break;
              case 52:
                getState(Matrix);
              break;
              case 53:
                getAccess(Matrix);
              break;
              default:
                Error();
              break;
            }
            break;
    case 53:
             // Paramétre
            Serial.println("// Paramétre");
            if (Matrix[1]==49){
              smsSetting(Matrix);
            }else if (Matrix[1]==50){
              pinSetting(Matrix);
            }else if (Matrix[1]==51){
              modeSys(Matrix);
            }else {
              Error();
            }
            break;
    case 54:
             // Réinitialisation du système 128
            Serial.println("Réinitialisation du système.");
            if(Matrix[1]==49 && Matrix[2]==50 && Matrix[3]==56){
              restSys();
            }
            break;
    case 55:
             // Protection du système.
            Serial.println("Protection du système.");
            if(Matrix[1]==55){
              if(Matrix[2]==49){
                setDelay(Matrix);
              }else if(Matrix[2]==50){
                tryProto(Matrix);
              }else if(Matrix[2]==51){
                sysLock(Matrix);
              }else {
                Error();
              }
            }
            break;

    default:
            break;
  }
}
/////// PARTIE 1 : Mettre les données en EEPROM.
/// CONFIGURATION.
//Mettre le numbres , numéro et la puissance de chaque objets.
void setObj(byte Matrix[MTR]){}
void setRelation(byte Matrix[MTR]){}
void setSec(byte Matrix[MTR]){}
//Mettre les données de client
void setNumPhone(byte Matrix[MTR]){}
void setPIN(byte Matrix[MTR]){}
/////// PARTIE 2 : Démarrage , Arrer des objets ou mettre un programme de démarrage.
/// Démarrage / Arret 
void actionObj(byte Matrix[MTR]){}
// Mettre un programme de démarrage.
void progObj(byte Matrix[MTR]){}
// PS : à cette fonction Il sera des prototypes pour traiter et afficher les erreurs.
/////// PARTIE 3 : les fonction d'horloge
/// Réglage la date et l'heure.
void setTime(byte Matrix[MTR]){}
//  Obtenir la date et l'heure
void getTime(){}
////// PARTIE 4 : les données de NEXTION
//// Historique
void showHist(byte Page){}
//// Etats des objets
void showState(){}
///  Programme de démarrage
void showProg(byte Page){}
//// Obtenir l'Etat d'un objet
void getState(byte Matrix[MTR]){}
//// dommander l'acès de paramétrage
void getAccess(byte Matrix[MTR]){}
//////// PARTIE 5 : paramétre
//// les paramétre des SMS
void smsSetting(byte Matrix[MTR]){}
//// les paramétre de PIN
void pinSetting(byte Matrix[MTR]){}
//// les paramétre Mode de démarrage
void modeSys(byte Matrix[MTR]){}
/////// PARTIE 6 : Réinitialisation du système
void restSys(){}
/////// PARTIE 7 : Protection du système
void setDelay(byte Matrix[MTR]){}
void tryProto(byte Matrix[MTR]){}
void sysLock(byte Matrix[MTR]){}
///////////////////////////////

void Error(){
  Serial.println("Il y a une Erreur ou ce choix n'existe pas");
}
