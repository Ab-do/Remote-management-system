// le 09/02/2019 Agadir
// Etape 11  : 
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <DS3231.h>
#include <Wire.h>
//Définition RTC
DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
// Déclaration des constantes.
const int MTR=20;  // Nombre de Rangé de la matrice
const int AD_VIRGINITY=0;
const int AD_VALIDITY=5;
const int AD_PIN=18;
const int AD_PHONE=20;
const int AD_NUMBER_OBJ=38;
const int AD_SETTING_SMS=50;
const int AD_SECTOR=62;
const int AD_RELATION_OBJ=134; //206
const int NUMBER_OBJ=5;
const int NUMBER_MAX=15;
// Déclaration des variables.
String Phone="669600729"; // numéro de Tel.
int sysTime=9999;
int PINcode=1234;
byte Hour,Minute,Second,Date,Month,Year;
// Déclaration des matrices.
int numberPhone[9]={0};        // nombre des objets de systeme
int numberObj[6]={0};        // nombre des objets de systeme
int settingSMS[6]={0};        // nombre des objets de systeme
int objState[NUMBER_OBJ][NUMBER_MAX]={0};        // les etates des objets
int sector[6][6]={0};        // les secteurs 
int relationObj[6][6]={0}; // les relation entre les pompes de refoulements et les vannes

void setup() {
  // Initialisation les serials ( Moniteur, Gsm , Nextion et HC12)
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  Wire.begin();
  while(!checkValidity()){}
  while(!checkVirginity()){
    Serial.println("NO");
    delay(5000);
    Virginity(1);
  }
  while(!loadingData()){
    Serial.println("Il y a un problème de téléchargement.");
    delay(5000);
  }
  Serial.println("start-up");
  showRAM();
}

void loop() {
  getTime(); // Mettre le temps à jour. 
  getDataSerial(); // ausculter les données qui obtient de moniteur série.
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
         showMatrix(Matrix,20);
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
            if(Matrix[2]==2 && Matrix[2]==2 && Matrix[2]==2){
               Serial.println("en cours: ");
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
                showMemory();
                //tryProto(Matrix);
              }else if(Matrix[2]==3){
                sysLock(Matrix);
              }else {
                Error();
              }
            }
            break;
     case 8:
         if(Matrix[1]==1){
          setState(Matrix);
         }
     break;
    default:
            break;
  }
}
/////// PARTIE 1 : Mettre les données en EEPROM.
/// CONFIGURATION.
//Mettre le numbres , numéro et la puissance de chaque objets.
void setObj(int Matrix[MTR])
  {
    Serial.println("Mettre le nombre des objets");
    showMatrix(Matrix,15);
    int j=0;
    for(int i=0;i<6;i++){
      numberObj[i]=toDec(Matrix[j+3],Matrix[j+4]);
      j+=2;
    }
    EEPROM.put(AD_NUMBER_OBJ,numberObj);
    showMatrix(numberObj,6);
  }
void setRelation(int Matrix[MTR])
  {
    Serial.println("Mettre les relations");
    showMatrix(Matrix,15);
    int j=0;
    for(int i=0;i<6;i++){
      relationObj[Matrix[3]-1][i]=toDec(Matrix[j+4],Matrix[j+5]);
      j+=2;
    }
    EEPROM.put(AD_RELATION_OBJ,relationObj);
    showMatrix(relationObj);
    }
void setSec(int Matrix[MTR])
  {
    Serial.println("Mettre les secteurs.");
    showMatrix(Matrix,15);
    int j=0;
    for(int i=0;i<6;i++){
      sector[Matrix[3]-1][i]=toDec(Matrix[j+4],Matrix[j+5]);
      j+=2;
    }
    EEPROM.put(AD_SECTOR,sector);
    showMatrix(sector);
  }
//Mettre les données de client
void setNumPhone(int Matrix[MTR])
  {
    Serial.println("Mettre Numéro de Tel.");
    showMatrix(Matrix,11);
    for(int i=0;i<9;i++){
      numberPhone[i]=Matrix[i+3];
    }
    Phone=toString(numberPhone);
    EEPROM.put(AD_PHONE,numberPhone);
    showMatrix(numberPhone,3);
  }
void setPIN(int Matrix[MTR])
  {
    int value = Matrix[3]*1000+Matrix[4]*100+Matrix[5]*10+Matrix[6];
    EEPROM.put(AD_PIN, value);
  }
/////// PARTIE 2 : Démarrage , Arrer des objets ou mettre un programme de démarrage.
/// Démarrage / Arret 
void actionObj(int Matrix[MTR]){}
// Mettre un programme de démarrage.
void progObj(int Matrix[MTR]){}
// PS : à cette fonction Il sera des prototypes pour traiter et afficher les erreurs.
/////// PARTIE 3 : les fonction d'horloge
/// Réglage la date et l'heure.
void setTime(int Matrix[MTR]){
  //<312205191045>
    Clock.setDate(toDec(Matrix[2],Matrix[3]));
    Clock.setMonth(toDec(Matrix[4],Matrix[5]));
    Clock.setYear(toDec(Matrix[6],Matrix[7]));
    Clock.setHour(toDec(Matrix[8],Matrix[9]));
    Clock.setMinute(toDec(Matrix[10],Matrix[11]));
    Clock.setSecond(0);
  }
//  Obtenir la date et l'heure to Nextion
void showTime(){}
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
void smsSetting(int Matrix[MTR])
    {
    Serial.println("Paramètre SMS");
    showMatrix(Matrix,8);
    int j=0;
    for(int i=0;i<6;i++){
      settingSMS[i]=toDec(Matrix[j+2],Matrix[j+3]);
      j+=2;
    }
    EEPROM.put(AD_SETTING_SMS,settingSMS);
    showMatrix(settingSMS,6);
      }
/////////// PARTIE 6 les etats
void setState(int Matrix[MTR]){
  if(Matrix[1]<=NUMBER_OBJ && toDec(Matrix[2],Matrix[3])<=NUMBER_MAX){
    Serial.print("L'Etat : ");
    Serial.print(" L'objet de type :");
    Serial.print(Matrix[1]);
    Serial.print(" numéro : ");
    Serial.print(toDec(Matrix[2],Matrix[3]));
    Serial.print(" Etat : ");
    Serial.println(Matrix[1]);
    objState[Matrix[1]][toDec(Matrix[2],Matrix[3])]=Matrix[5];
  }
}
//// les paramétre de PIN
void pinSetting(int Matrix[MTR]){}
//// les paramétre Mode de démarrage
void modeSys(int Matrix[MTR]){}
/////// PARTIE 6 : Réinitialisation du système
void restSys()
  {
    for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
    if(i%100==0){
      Serial.print("-");
    }
  }
  Serial.println();
  Serial.println("le système a été réinitialisé");
  }
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
// les données
bool loadingData(){
  EEPROM.get(AD_PIN,PINcode);
  EEPROM.get(AD_PHONE,numberPhone);
  Phone=toString(numberPhone);
  EEPROM.get(AD_NUMBER_OBJ,numberObj);
  EEPROM.get(AD_SETTING_SMS,settingSMS);
  EEPROM.get(AD_SECTOR,sector);
  EEPROM.get(AD_RELATION_OBJ,relationObj);
  Serial.println("le chargement des données a été téléchargé");
  return true;
}



////////////////// les fonctions du vérification
///// fonction pour vérifier 
  //  Vérification la virginité.
bool checkVirginity(){
  int value=EEPROM[0];
  if(value==1){
    return true;
  }else if(value==2){
    return false;
  }
}
  //  Vérification la validité
bool checkValidity(){
  return true;
}
/////////////// NEXTION
// Mettre les données à Nextion 
void setDataNextion(String string,String val) {
  delay(10);
  Serial2.write(0xff);
  Serial2.write(0xff); 
  Serial2.write(0xff);
  string.concat(val);
  for (int i = 0; i < string.length(); i++)
  {
    Serial2.write(string[i]); 
  }
  Serial2.write(0xff); 
  Serial2.write(0xff); 
  Serial2.write(0xff);

}
// Affiche les informations et les erreurs.
void popupMessage(String msg){
  
}
///////////////// GSM 
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




///////les foncations du plugin
///
int toDec(int o,int p){
  int v = o*10+p;
  return v;
}
String toString(int Matrix[9]){
  String str="";
  for(int i=0;i<9;i++)
    str+=Matrix[i];
  Serial.println(str);
  return str; 
}
void getTime(){
  Year=Clock.getYear();
  Month=Clock.getMonth(Century);
  Date=Clock.getDate();
  Hour = Clock.getHour(h12, PM);
  Minute= Clock.getMinute();
  Second= Clock.getSecond();
}
void Error(){
  Serial.println("Il y a une Erreur ou ce choix n'existe pas");
}
/////////////// fonction d'essai et d'affichage
void showRAM(){
  Serial.print("Numéro de Tel : ");
  showMatrix(numberPhone,9);
  Serial.println(Phone);
  Serial.print("durée de vie : ");
  Serial.println(sysTime);
  Serial.print("PIN : ");
  Serial.println(PINcode);
  Serial.println("Nombre des Objects..");
  showMatrix(numberObj,6);
  Serial.println("les paramètres SMS..");
  showMatrix(settingSMS,6);
  Serial.println("Les secteurs : ");
  showMatrix(sector);
  Serial.println("les Relation :");
  showMatrix(relationObj);
  delay(1000);
  getTime();
  Serial.print(Date);
  Serial.print("-");
  Serial.print(Month);
  Serial.print("-");
  Serial.print(Year);
  Serial.print(" ");
  Serial.print(Hour); //24-hr
  Serial.print(":");
  Serial.print(Minute);
  Serial.print(":");
  Serial.println(Second);
}
void showMemory(){
  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());
  int Matrix1[6];
  int Matrix2[6][6];
  int Matrix3[6][6];
  String str="";
  Serial.println("Matrice 1 : les nombres des objets");
  EEPROM.get(AD_NUMBER_OBJ,Matrix1);
  showMatrix(Matrix1,6);
  Serial.println("Matrice 2 : les relatiions ");
  EEPROM.get(AD_RELATION_OBJ,Matrix2);
  showMatrix(Matrix2);
  Serial.println("Matrice 3 : les secteurs");
  EEPROM.get(AD_SECTOR,Matrix3);
  showMatrix(Matrix3);
  Serial.println("Numéro de Tel : ");
  EEPROM.get(AD_PHONE,str);
  delay(1000);
  Serial.println(str);
  
}

void showMatrix(int Matrix[20],int a){
  
    for(int j=0;j<a;j++){
    Serial.print(" ");
    Serial.print(Matrix[j]);
     }
    Serial.println();
}

void showMatrix(int Matrix[6][6]){
  for(int i=0;i<6;i++){
    for(int j=0;j<6;j++){
    Serial.print("   ");
    Serial.print(Matrix[i][j]);
     }
    Serial.println();
  }
  Serial.println("_______6x6________");
  Serial.println();
}

void showSize(){
  int adresse=6;
  adresse += sizeof(sysTime);
  Serial.print("PIN ");
  Serial.println(adresse+10);
  adresse += sizeof(PINcode);
  Serial.print("NumbrePhone ");
  Serial.println(adresse+10);
  adresse += sizeof(numberPhone);
  Serial.print("numbreObje ");
  Serial.println(adresse+10);
  adresse += sizeof(numberObj);
  Serial.print("settingSMS ");
  Serial.println(adresse+10);
  adresse += sizeof(settingSMS);
  Serial.print("sector ");
  Serial.println(adresse+10);
  adresse += sizeof(sector);
  Serial.print("relationObj ");
  Serial.println(adresse+10);
  adresse += sizeof(relationObj);
  Serial.print("objState ");
  Serial.println(adresse+10);
  adresse += sizeof(objState);
  Serial.print("Next ");
  Serial.println(adresse+10);
}
