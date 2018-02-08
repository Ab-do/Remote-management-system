// le 08/02/2019 Agadir
// Etape 6  : 
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
  int Matrix[MTR]={0};
  int i=0;
  while(i<str.length()){
      if(str[i]==60){
        i++;
        int j=0;
          while(str[i]!=62){
            Matrix[j]=str[i]-48;
            j++; 
            i++;
         }
         //Commutation des données.
         for(int j=0;j<MTR;j++){
         Serial.print(" ");
         Serial.print(Matrix[j]);
         }
         switchData(Matrix);
         memset(Matrix,0,sizeof(Matrix));
      }
      else{
         i++; 
      }     
  }
}
// Analyse, Commutation des données et Effectuation des actions.
void switchData(int Matrix[MTR]){
  Serial.println("SWITCH");
  switch(Matrix[0]){
    case 1: // int : int + 48.
            // Inclure les données de paramétrage.
            Serial.println("Inclure les données de paramétrage.");
            if(Matrix[1]==1){
              // les fonction de paramétrage.
              if(Matrix[2]==1){
                setObj(Matrix);
              }else if(Matrix[2]==2){
                setRelation(Matrix);
              }else if(Matrix[2]==3){
                setSec(Matrix);
              }else {
                // Erreur.
              Error();
              }
            }else if(Matrix[1]==2){
              // Mettre les informations.
              if(Matrix[2]==1){
                setNumPhone(Matrix);
              }else if(Matrix[2]==2){
                setPIN(Matrix);
              }else {
                Error();
              }
            }else {
              // Erreur.
              Error();
            }
            break;
    case 2:
             //  // Effectuer une action sur un objet.
            Serial.println("// Effectuer une action sur un objet.");
            if(Matrix[1]==1){
              actionObj(Matrix); 
            }else if(Matrix[1]==2){
              progObj(Matrix);
            }else {
              Error();
            }
            break;
    case 3:
             // Fonctions liées à l'horloge.
            Serial.println("// Fonctions liées à l'horloge.");
            if(Matrix[1]==1){ // Réglage la date et l'heure
              setTime(Matrix);
            }else if(Matrix[1]==2) { // Demmande la date et l'heure
              getTime();
            }else {
              Error();
            }
            break;
    case 4:
             // mettre les données à propos système.
            Serial.println("// mettre les données à propos système.");
            switch(Matrix[1]){
              case 1:
                showHist(Matrix[2]);
              break;
              case 2:
                showState();
              break;
              case 3:
                showProg(Matrix[2]);
              break;
              case 4:
                getState(Matrix);
              break;
              case 5:
                getAccess(Matrix);
              break;
              default:
                Error();
              break;
            }
            break;
    case 5:
             // Paramétre
            Serial.println("// Paramétre");
            if (Matrix[1]==1){
              smsSetting(Matrix);
            }else if (Matrix[1]==2){
              pinSetting(Matrix);
            }else if (Matrix[1]==3){
              modeSys(Matrix);
            }else {
              Error();
            }
            break;
    case 6:
             // Réinitialisation du système 128
            Serial.println("Réinitialisation du système.");
            if(Matrix[1]==1 && Matrix[2]==2 && Matrix[3]==8){
              restSys();
            }
            break;
    case 7:
             // Protection du système.
            Serial.println("Protection du système.");
            if(Matrix[1]==4){
              if(Matrix[2]==1){
                setDelay(Matrix);
              }else if(Matrix[2]==2){
                tryProto(Matrix);
              }else if(Matrix[2]==3){
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
void setObj(int Matrix[MTR]){}
void setRelation(int Matrix[MTR]){}
void setSec(int Matrix[MTR]){}
//Mettre les données de client
void setNumPhone(int Matrix[MTR])
  {
    
  }
void setPIN(int Matrix[MTR])
  {
    int value = Matrix[3]*1000+Matrix[4]*100+Matrix[5]*10+Matrix[6];
    EEPROM.put(2, value);
  }
/////// PARTIE 2 : Démarrage , Arrer des objets ou mettre un programme de démarrage.
/// Démarrage / Arret 
void actionObj(int Matrix[MTR]){}
// Mettre un programme de démarrage.
void progObj(int Matrix[MTR]){}
// PS : à cette fonction Il sera des prototypes pour traiter et afficher les erreurs.
/////// PARTIE 3 : les fonction d'horloge
/// Réglage la date et l'heure.
void setTime(int Matrix[MTR]){}
//  Obtenir la date et l'heure
void getTime(){}
////// PARTIE 4 : les données de NEXTION
//// Historique
void showHist(int Page){}
//// Etats des objets
void showState(){}
///  Programme de démarrage
void showProg(int Page){}
//// Obtenir l'Etat d'un objet
void getState(int Matrix[MTR]){}
//// dommander l'acès de paramétrage
void getAccess(int Matrix[MTR]){}
//////// PARTIE 5 : paramétre
//// les paramétre des SMS
void smsSetting(int Matrix[MTR]){}
//// les paramétre de PIN
void pinSetting(int Matrix[MTR]){}
//// les paramétre Mode de démarrage
void modeSys(int Matrix[MTR]){}
/////// PARTIE 6 : Réinitialisation du système
void restSys(){}
/////// PARTIE 7 : Protection du système
void setDelay(int Matrix[MTR]){}
void tryProto(int Matrix[MTR]){}
void sysLock(int Matrix[MTR]){}
///////////////////////////////
////////////////// les fonctions EEPROM
////// Mettre des valeurs 
void Virginity(int value ){
  EEPROM[0]=value;
}



////////////////// les fonctions du vérification
///// fonction pour vérifier 
bool checkVirginity(){
  int value=EEPROM[0];
  if(value==1){
    return true;
  }else if(value==2){
    return false;
  }
}
void Error(){
  Serial.println("Il y a une Erreur ou ce choix n'existe pas");
}


void showMatrix(int Matrix[30][30],int a,int b){
  for(int i=0;i<a;i++){
    for(int j=0;j<b;j++){
    Serial.print(" ");
    Serial.print(Matrix[i][j]);
  }
  Serial.println();
  }
}
