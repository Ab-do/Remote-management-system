// le 04/03/2019 Agadir
// Etape 40  :  change la vitesse serial
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <DS3231.h>
#include <Wire.h>
#include "Item.h"
#include <SdFat.h>
#include <CSVFile.h>
#define PIN_SD_CS 53
#define SD_CARD_SPEED SPI_FULL_SPEED 
//Définition RTC
DS3231 Clock;
SdFat sd;
CSVFile csv;
bool Century=false;
bool h12;
bool PM;
bool Config=false;
bool ComWc=false;
bool ComGsm=false;
bool dataFromNex=true;
// Déclaration des constantes.
const int MTR=20;  // Nombre de Rangé de la matrice
const int AD_VIRGINITY=1;
const int AD_MODE_SYS=2; // mode de démarrage
const int AD_VALIDITY=0;
const int AD_PIN=18;
const int AD_PHONE=20;
const int AD_NUMBER_OBJ=38;
const int AD_SETTING_SMS=50;
const int AD_SECTOR=62;
const int AD_RELATION_OBJ=134; 
const int AD_RELATION_PAE=210; //280
const int NUMBER_OBJ=5;
const int NUMBER_MAX=15;
const double SPEED_SERIAL = 9600;
const unsigned long PERIOD=110000;
// Declaration les pins des LED
const int LED_START=34;
const int LED_WIRLESS_COM=33;
const int LED_CHECK_GSM=32;
// Déclaration des variables.
unsigned long Time = millis();
unsigned long Last = millis();
String Phone="669600729"; // numéro de Tel.
String StePhone="669600729";
int sysTime=9999;
int PINcode=11234;
int Hour,Minute,Second,Date,Month,Year;
int LevelNet=0;
//boolean ModeSys=false;
// Déclaration des matrices.
int numberPhone[9]={0};        // nombre des objets de systeme
int numberObj[6]={0};        // nombre des objets de systeme
int settingSMS[6]={0};        // nombre des objets de systeme
int objState[NUMBER_OBJ][NUMBER_MAX]={0};        // les etates des objets
int sector[6][6]={0};        // les secteurs 
int relationObj[6][6]={0}; // les relation entre les pompes de refoulements et les vannes
int relationPae[5][2]={0};
int ModeSys[3]={0};
Item pim[10]={{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7},{1,8},{1,9},{1,10}};
Item pr[5]={{2,1},{2,2},{2,3},{2,4},{2,5}};
Item van[15]={{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7},{3,8},{3,9},{3,10},{3,11},{3,12},{3,13},{3,14},{3,15}};
Item mlg[5]={{4,1},{4,2},{4,3},{4,4},{4,5}};
Item eng[5]={{5,1},{5,2},{5,3},{5,4},{5,5}};
void setup() {
  // Initialisation les serials ( Moniteur, Gsm , Nextion et HC12)
  Serial.begin(SPEED_SERIAL);
  Serial1.begin(SPEED_SERIAL);
  Serial2.begin(SPEED_SERIAL);
  Serial3.begin(SPEED_SERIAL);
  Serial3.setTimeout(50);
  Wire.begin();
  while(!checkValidity()){
    setDataNextion("page Info");
    setDataNextion("t13_0.txt=\"The system has been blocked, contact your supplier !\"");
    Serial2.end();
    while(1){
      getDataHc();
    }
    }
   setDataNextion("page 0");
   delay(200);setDataNextion("j0.val=10");
   while(!checkVirginity()){
    if(Config==false){
      setDataNextion("page Configuration");
      Config=true;
    }else {
       //getDataNextion();
       //getDataSerial();
       loop();
      //Virginity(1);
    }
  }
  delay(200);setDataNextion("j0.val=20");
  while(!loadingData()){
    popupMessage("Il y a un problème de téléchargement.");
    delay(5000);
  }
  setDataNextion("j0.val=100");
  if (!sd.begin(PIN_SD_CS, SD_CARD_SPEED))
  {
  popupMessage("SD card begin error");
  return;
  }
  intValve();
  intLed();
  //showRAM();
  getTime();
  Last = millis();// tm
  //setDataNextion("page 1");
  addHist("start-up");
  digitalWrite(LED_START,HIGH);
  Serial3.println("Wd1");
}

void loop() {
  getTime(); // Mettre le temps à jour. 
  getDataHc();
  getDataNextion();
  if(ModeSys[2]==1){
    autoRunObj(); 
  }
  ckeckHCState();
}
//********* OBTEBNIR LES DONNEES


// Obtenir des données de Nextion
void getDataNextion(){
  if(Serial2.available()>0){
    String str=Serial2.readString();
    strToMatrix(str);
    dataFromNex=true;
  }
}
// Obtenir des données d Module radio HC12
void getDataHc(){
  if(Serial3.available()>0){
    if(Serial3.read()=='<'){
    String str=Serial3.readString();
    str.trim();
    int n=str.indexOf('>');
    if( str.length()<30 && n!=-1){
      strToMatrix("<"+str.substring(0,n+1));
      dataFromNex=false;
    }
  }else {
    Serial3.flush();
  }
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
            if(i>str.length()) return 0;
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
 
  switch(Matrix[0]){
    case 1: // int : int + 48.
            // Inclure les données de paramétrage.
            if(Matrix[1]==1){
              // les fonction de paramétrage.
              if(Matrix[2]==1){
                setObj(Matrix);
              }else if(Matrix[2]==2){
                setRelation(Matrix);
              }else if(Matrix[2]==3){
                setSec(Matrix);
              }else if(Matrix[2]==4){
                setRelationPae(Matrix);
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
          
            if(Matrix[1]==1){
              if(ModeSys[0]==1){
                actionObj(Matrix); 
              }else {
                Error();
              }
            }else if(Matrix[1]==2){
              progObj(Matrix);
            }else {
              Error();
            }
            break;
    case 3:
             // Fonctions liées à l'horloge.
           
            if(Matrix[1]==1){ // Réglage la date et l'heure
              setTime(Matrix);
            }else if(Matrix[1]==2) { // Demmande la date et l'heure
              getTimeNextion();
            }else {
              Error();
            }
            break;
    case 4:
             // mettre les données à propos système.
            switch(Matrix[2]){
              case 1:
                showHist(Matrix);
              break;
              case 2:
                showState();
              break;
              case 3:
                showProg(Matrix[1],Matrix[3]);
              break;
              case 4:
                getState(Matrix);
              break;
              case 5:
                getAccess();
              break;
              case 6:
                getProg(Matrix);
              break;
              case 7:
                getSettingSMS();
              break;
              case 8:
                sendPinSMS();
              break;
              case 9:
                 switch(Matrix[1]){
                  case 1:
                      getModeSys();
                  break;
                  case 2: 
                      getStateWc();
                  break;
                  case 3:
                      getNumClient();
                  break;
                  default:
                      Error();
                  break;
                 }
              break;
              default:
                Error();
              break;
            }
            break;
    case 5:
            if (Matrix[1]==1){
              smsSetting(Matrix);
            }else if (Matrix[1]==2){
              pinSetting(Matrix);
            }else if (Matrix[1]==3){
              modeSys(Matrix);
            }else if(Matrix[1]==4){
                 switch(Matrix[2]){
                    case 1:
                      getNumObj();
                    break;
                    case 2:
                      getNumPae();
                    break;
                    case 3:
                      getPae(Matrix[3]);
                    break;
                    case 4:
                      getNumSector();
                    break;
                    case 5:
                      getSector(Matrix[3]);
                    break;
                    case 6:
                      getNumRelation();
                    break;
                    case 7:
                      getRelation(Matrix[3]);
                    break;
                    default:
                    break;
             
                  }
            }else {
              Error();
            }
            break;
    case 6:
           
            switch(Matrix[3]){
              case 1:
                restSys();
              break;
              case 2:
                //Virginity(1);
              break;
              case 3:
                getValueRelation();
              break;
              case 4:
                getValuePae();
              break;
              case 5:
                Virginity(1);
              break;
              default:
                Error();
              break;
            };
          break;
    case 7:
        if(Matrix[1]==1){
           switch(Matrix[2]){
            case 1:
              getAppPin(Matrix);
            break;
            case 2:
              getAppNumObj();
            break;
            case 3:
              getAppState();
            break;
            default:
              Error();
            break;
           }
        }else if(Matrix[1]==4){
              if(Matrix[2]==1){
                setDelay(Matrix);
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
         }else if (Matrix[1]==2){
            ckeckGsmState(toDec(Matrix[2],Matrix[3]));
         }else if(Matrix[1]==3){
            ckeckWirless();
         }else if(Matrix[1]==6){
            popupMessage("echec d'envoi SMS");
         }else {
          
         }
     break;
     case 9:
        checkState(Matrix);
     break;
    default:
       Error();
            break;
  }
}

void setObj(int Matrix[MTR]){
    int j=0;
    for(int i=0;i<6;i++){
      numberObj[i]=toDec(Matrix[j+3],Matrix[j+4]);
      j+=2;
    }
    if(EEPROM.put(AD_NUMBER_OBJ,numberObj)){
      successMessage(111);
    }
  }
void setRelation(int Matrix[MTR]){
    int j=0;
    for(int i=0;i<6;i++){
      relationObj[Matrix[3]-1][i]=toDec(Matrix[j+4],Matrix[j+5]);
      j+=2;
    }
    if(EEPROM.put(AD_RELATION_OBJ,relationObj)){
      successMessage(112);
    }
    }
void setSec(int Matrix[MTR]){
    int j=0;
    for(int i=0;i<6;i++){
      sector[Matrix[3]-1][i]=toDec(Matrix[j+4],Matrix[j+5]);  // peut etre y'a un problème pour les vannes qu'elle a un numéro plus que 10.
      j+=2;
    }
    if(EEPROM.put(AD_SECTOR,sector)){
      successMessage(113);
    }
  }
 // Mettre la relation entre les pompes de refoulement et les pompes à engrais.
void setRelationPae(int Matrix[MTR]){
    relationPae[Matrix[3]-1][0]=toDec(Matrix[4],Matrix[5]);
    relationPae[Matrix[3]-1][1]=toDec(Matrix[6],Matrix[7]);
    if(EEPROM.put(AD_RELATION_PAE,relationPae)){
      successMessage(114);
    }
}
//Mettre les données de client
void setNumPhone(int Matrix[MTR]){
    for(int i=0;i<9;i++){
      numberPhone[i]=Matrix[i+3];
    }
    Phone=toString(numberPhone);
    if(EEPROM.put(AD_PHONE,numberPhone)){
      successMessage(121);
    }
    Serial3.println("N"+Phone);
  }
void setPIN(int Matrix[MTR]){
    PINcode= 10000+Matrix[3]*1000+Matrix[4]*100+Matrix[5]*10+Matrix[6]; 
    if(EEPROM.put(AD_PIN,PINcode)){
      successMessage(122);
    }
  }


void actionObj(int Matrix[MTR]){
  switch(Matrix[2]){
    case 1:
        pim[toDec(Matrix[3],Matrix[4])-1].runObj(Matrix[5]);
    break;
    case 2:
        pr[toDec(Matrix[3],Matrix[4])-1].runObj(Matrix[5]);
    break;
    case 3:
        van[toDec(Matrix[3],Matrix[4])-1].runObj(Matrix[5]);
    break;
    case 4:
        mlg[toDec(Matrix[3],Matrix[4])-1].runObj(Matrix[5]);
    break;
    case 5:
        eng[toDec(Matrix[3],Matrix[4])-1].runObj(Matrix[5]);
    break;
    case 6:
        for(int i=0;i<6;i++){
           if(sector[Matrix[4]-1][i]>0){
              van[sector[Matrix[4]-1][i]-1].runObj(Matrix[5]);
              delay(500);
           }
        }  
    break;
    default:
      Error();
    break;
  }
  }
// Mettre un programme de démarrage.
void progObj(int Matrix[MTR]){

  switch(Matrix[2]){
    case 1:
        pim[toDec(Matrix[3],Matrix[4])-1].setProg(toDec(Matrix[6],Matrix[7]),toDec(Matrix[8],Matrix[9]),toDec(Matrix[10],Matrix[11]),toDec(Matrix[12],Matrix[13]),Matrix[5]);
    break;
    case 2:
        pr[toDec(Matrix[3],Matrix[4])-1].setProg(toDec(Matrix[6],Matrix[7]),toDec(Matrix[8],Matrix[9]),toDec(Matrix[10],Matrix[11]),toDec(Matrix[12],Matrix[13]),Matrix[5]);
    break;
    case 3:
        van[toDec(Matrix[3],Matrix[4])-1].setProg(toDec(Matrix[6],Matrix[7]),toDec(Matrix[8],Matrix[9]),toDec(Matrix[10],Matrix[11]),toDec(Matrix[12],Matrix[13]),Matrix[5]);
    break;
    case 4:
        mlg[toDec(Matrix[3],Matrix[4])-1].setProg(toDec(Matrix[6],Matrix[7]),toDec(Matrix[8],Matrix[9]),toDec(Matrix[10],Matrix[11]),toDec(Matrix[12],Matrix[13]),Matrix[5]);
    break;
    case 5:
        eng[toDec(Matrix[3],Matrix[4])-1].setProg(toDec(Matrix[6],Matrix[7]),toDec(Matrix[8],Matrix[9]),toDec(Matrix[10],Matrix[11]),toDec(Matrix[12],Matrix[13]),Matrix[5]);
    break;
    case 6:
        for(int i=0;i<6;i++){
         if(sector[Matrix[4]-1][i]>0){  
          int N= sector[Matrix[4]-1][i];
           van[N-1].setProg(toDec(Matrix[6],Matrix[7]),toDec(Matrix[8],Matrix[9]),toDec(Matrix[10],Matrix[11]),toDec(Matrix[12],Matrix[13]),Matrix[5]);
         }
         }
         
    break;
    default:
      Error();
            break;
  }
  
  }
void setTime(int Matrix[MTR]){
    Clock.setDate(toDec(Matrix[2],Matrix[3]));
    Clock.setMonth(toDec(Matrix[4],Matrix[5]));
    Clock.setYear(toDec(Matrix[6],Matrix[7]));
    Clock.setHour(toDec(Matrix[8],Matrix[9]));
    Clock.setMinute(toDec(Matrix[10],Matrix[11]));
    Clock.setSecond(0);
    successMessage(312);
  }

void showTime(){}

void showHist(int Matrix[MTR]){
  String HistFile;
  Matrix[3]==9? HistFile = "Hist_"+String(Date)+"_"+String(Month)+"_"+String(Year)+".csv"         
  :   HistFile = "Hist_"+String(toDec(Matrix[3],Matrix[4]))+"_"+String(toDec(Matrix[5],Matrix[6]))+"_"+String(toDec(Matrix[7],Matrix[8]))+".csv";
  if(sd.exists(HistFile.c_str())){
      csv.open(HistFile.c_str(), O_RDWR); 
    } else {
      popupMessage("File not existe");  
      return 0; 
      }
  csv.gotoBeginOfFile();
  while(!csv.isEndOfLine()) csv.nextLine();
  int countLine=csv.getNumberOfLine()-1,i=5,Line=0;
  csv.gotoBeginOfFile();
  Line=(countLine-(5*Matrix[1]))+5;
  csv.gotoLine(Line);
  int j=1;
  while(i!=0)
  {
    char buffer[30];
    csv.readField(buffer,30);
    setDataNextion("O"+String(j)+".txt=\""+String(buffer)+"\"");
    csv.nextField(); 
    memset(buffer,0,30);
    csv.readField(buffer,7);
    setDataNextion("M"+String(j)+".txt=\""+String(buffer)+"\"");
    csv.nextField(); memset(buffer,0,30);
    csv.readField(buffer,5);
    setDataNextion("S"+String(j)+".txt=\""+String(buffer)+"\"");
    j++;   i--;  Line--; memset(buffer,0,30);
    csv.gotoLine(Line);
  }
    csv.close();
}
//// Etats des objets
void showState(){
    ckeckWirless();
    getStateWc(); 
     int S;
  for(int i=0;i<numberObj[0];i++){
     int k=i+0;
    S=objState[0][i];
    if(S==1){
    setDataNextion("b"+String(k)+".picc=30");
    }else if(S==2 || S==0) {
    setDataNextion("b"+String(k)+".picc=31");
    }else {
    setDataNextion("b"+String(k)+".picc=32");
    }
  }
  for(int i=0;i<numberObj[1];i++){
    int k=i+10;
    S=objState[1][i];
     if(S==1){
    setDataNextion("b"+String(k)+".picc=30");
    }else if(S==2 || S==0) {
    setDataNextion("b"+String(k)+".picc=31");
    }else {
    setDataNextion("b"+String(k)+".picc=32");
    }
  }
  for(int i=0;i<numberObj[2];i++){
     int k=i+15;
    S=objState[2][i];
     if(S==1){
    setDataNextion("b"+String(k)+".picc=30");
    }else if(S==2 || S==0) {
    setDataNextion("b"+String(k)+".picc=31");
    }else {
    setDataNextion("b"+String(k)+".picc=32");
    }
  }
  for(int i=0;i<numberObj[3];i++){
     int k=i+45;
    S=objState[3][i];
     if(S==1){
    setDataNextion("b"+String(k)+".picc=30");
    }else if(S==2 || S==0) {
    setDataNextion("b"+String(k)+".picc=31");
    }else {
    setDataNextion("b"+String(k)+".picc=32");
    }
  }
    for(int i=0;i<numberObj[4];i++){
     int k=i+50;
    S=objState[4][i];
    if(S==1){
     setDataNextion("b"+String(k)+".picc=30");
    }else if(S==2 || S==0) {
    setDataNextion("b"+String(k)+".picc=31");
    }else {
    setDataNextion("b"+String(k)+".picc=32");
    }
  }
  }
///  Programme de démarrage
void showProg(int type,int Page){
    int id=1;
    int cont=1;
    switch(type){
      case 1:
          for(int j=0;j<numberObj[0];j++){
              if(pim[j].MatrixTime[4]>1){  // P1 , T1 , A1 , E1
                if((Page-1)*5<cont && Page*5>=cont){
                setDataNextion("P"+String(id)+".txt=\"PIM "+String(j+1)+"\"");
                setDataNextion("T"+String(id)+".txt=\""+toString(pim[j].MatrixTime[0]-1)+":"+toString(pim[j].MatrixTime[1]-1)+"\"");
                setDataNextion("A"+String(id)+".txt=\""+toString(pim[j].MatrixTime[2]-1)+":"+toString(pim[j].MatrixTime[3]-1)+"\"");
                if(pim[j].MatrixTime[4]==3 || pim[j].MatrixTime[4]==4 ){
                     setDataNextion("E"+String(id)+".txt=\"Actif\"");
               }else if(pim[j].MatrixTime[4]==5 || pim[j].MatrixTime[4]==6){
                     setDataNextion("E"+String(id)+".txt=\"Inactif\"");
               }
               id++;  }
               cont++;
          } 
          }
      break;
      case 2:
         for(int j=0;j<numberObj[1];j++){
           if(id<=Page*5){
              if(pr[j].MatrixTime[4]>1){  // P1 , T1 , A1 , E1
                setDataNextion("P"+String(id)+".txt=\"PMP REF "+String(j+1)+"\"");
                setDataNextion("T"+String(id)+".txt=\""+toString(pr[j].MatrixTime[0]-1)+":"+toString(pr[j].MatrixTime[1]-1)+"\"");
                setDataNextion("A"+String(id)+".txt=\""+toString(pr[j].MatrixTime[2]-1)+":"+toString(pr[j].MatrixTime[3]-1)+"\"");
                if(pr[j].MatrixTime[4]==3 || pr[j].MatrixTime[4]==4 ){
                     setDataNextion("E"+String(id)+".txt=\"Actif\"");
               }else if(pr[j].MatrixTime[4]==5 || pr[j].MatrixTime[4]==6){
                     setDataNextion("E"+String(id)+".txt=\"Inactif\"");
               }
               id++;
          } }
          }
      break;
      case 3:
         for(int j=0;j<numberObj[2];j++){
              if(van[j].MatrixTime[4]>1){  // P1 , T1 , A1 , E1
                if((Page-1)*5<cont && Page*5>=cont){
                setDataNextion("P"+String(id)+".txt=\"VAN "+String(j+1)+"\"");
                setDataNextion("T"+String(id)+".txt=\""+toString(van[j].MatrixTime[0]-1)+":"+toString(van[j].MatrixTime[1]-1)+"\"");
                setDataNextion("A"+String(id)+".txt=\""+toString(van[j].MatrixTime[2]-1)+":"+toString(van[j].MatrixTime[3]-1)+"\"");
                if(van[j].MatrixTime[4]==3 || van[j].MatrixTime[4]==4 ){
                     setDataNextion("E"+String(id)+".txt=\"Actif\"");
               }else if(van[j].MatrixTime[4]==5 || van[j].MatrixTime[4]==6){
                     setDataNextion("E"+String(id)+".txt=\"Inactif\"");
               }
               id++;  }
               cont++;
          } 
          }
      break;
      case 4:
          for(int j=0;j<numberObj[3];j++){
            if(id<=Page*5){
              if(pr[j].MatrixTime[4]>1){  // P1 , T1 , A1 , E1
                setDataNextion("P"+String(id)+".txt=\"MLG "+String(j+1)+"\"");
                setDataNextion("T"+String(id)+".txt=\""+toString(pr[j].MatrixTime[0]-1)+":"+toString(pr[j].MatrixTime[1]-1)+"\"");
                setDataNextion("A"+String(id)+".txt=\""+toString(pr[j].MatrixTime[2]-1)+":"+toString(pr[j].MatrixTime[3]-1)+"\"");
                if(pr[j].MatrixTime[4]==3 || pr[j].MatrixTime[4]==4 ){
                     setDataNextion("E"+String(id)+".txt=\"Actif\"");
               }else if(pr[j].MatrixTime[4]==5 || pr[j].MatrixTime[4]==6){
                     setDataNextion("E"+String(id)+".txt=\"Inactif\"");
               }
               id++;
               
          } }
          }
      break;
      case 5:
          for(int j=0;j<numberObj[4];j++){
           if(id<=Page*5){
              if(eng[j].MatrixTime[4]>1){  // P1 , T1 , A1 , E1
                setDataNextion("P"+String(id)+".txt=\"PMP ANG "+String(j+1)+"\"");
                setDataNextion("T"+String(id)+".txt=\""+toString(eng[j].MatrixTime[0]-1)+":"+toString(eng[j].MatrixTime[1]-1)+"\"");
                setDataNextion("A"+String(id)+".txt=\""+toString(eng[j].MatrixTime[2]-1)+":"+toString(eng[j].MatrixTime[3]-1)+"\"");
                if(eng[j].MatrixTime[4]==3 || eng[j].MatrixTime[4]==4 ){
                     setDataNextion("E"+String(id)+".txt=\"Actif\"");
               }else if(eng[j].MatrixTime[4]==5 || eng[j].MatrixTime[4]==6){
                     setDataNextion("E"+String(id)+".txt=\"Inactif\"");
               }
               id++;
          } }
          }
      break;
    }
  
  }


  void showProg(int Page){
    int id=1;
      for(int h=0;h<24;h++){
        for(int i=0;i<15;i++){
          if(van[i].MatrixTime[0]-1==h){
                setDataNextion("P"+String(id)+".txt=\"VAN "+String(i+1)+"\"");
                setDataNextion("T"+String(id)+".txt=\""+toString(van[i].MatrixTime[0]-1)+":"+toString(van[i].MatrixTime[1]-1)+"\"");
                setDataNextion("A"+String(id)+".txt=\""+toString(van[i].MatrixTime[2]-1)+":"+toString(van[i].MatrixTime[3]-1)+"\"");
                id++;
          }
          if(pim[i].MatrixTime[0]-1==h){
                setDataNextion("P"+String(id)+".txt=\"PIM "+String(i+1)+"\"");
                setDataNextion("T"+String(id)+".txt=\""+toString(pim[i].MatrixTime[0]-1)+":"+toString(pim[i].MatrixTime[1]-1)+"\"");
                setDataNextion("A"+String(id)+".txt=\""+toString(pim[i].MatrixTime[2]-1)+":"+toString(pim[i].MatrixTime[3]-1)+"\"");
                id++;
          }
        }
      }
  }
//// Obtenir l'Etat d'un objet
void getState(int Matrix[MTR]){
   int St =objState[Matrix[3]-1][toDec(Matrix[4],Matrix[5])-1];  
   switch(Matrix[3]){
      case 1:
          if(St==1){
          setDataNextion("p0.pic=57");
          }else if (St==2 || St==0){
          setDataNextion("p0.pic=52");
          }else {
          setDataNextion("p0.pic=49");
          }
      break;
      case 2:
          if(St==1){
          setDataNextion("p0.pic=54");
          }else if (St==2 || St==0){
          setDataNextion("p0.pic=50");
          }else {
          setDataNextion("p0.pic=51");
          }
      break;
      case 3:
          if(St==1){
          setDataNextion("p0.pic=43");
          }else if (St==2 || St==0){
          setDataNextion("p0.pic=56");
          }else {
          setDataNextion("p0.pic=53");
          }
      break;
      case 4:
          if(St==1){
          setDataNextion("p0.pic=55");
          }else if (St==2 || St==0){
          setDataNextion("p0.pic=47");
          }else {
          setDataNextion("p0.pic=48");
          }
      break;
      case 5:
          if(St==1){
          setDataNextion("p0.pic=44");
          }else if (St==2 || St==0){
          setDataNextion("p0.pic=46");
          }else {
          setDataNextion("p0.pic=45");
          }
      break;
      default:
          Error();
      break;
   }
  }
//// dommander l'acès de paramétrage
void getAccess(){
    setDataNextion("Pw.txt=\""+toStringPin(PINcode-10000)+"\"");
  }

void getProg(int Matrix[MTR]){
   switch(Matrix[3]){
      case 1: 
            setDataNextion("va0.val="+String(pim[toDec(Matrix[4],Matrix[5])-1].MatrixTime[0]-1));
            setDataNextion("va1.val="+String(pim[toDec(Matrix[4],Matrix[5])-1].MatrixTime[1]-1));
            setDataNextion("va2.val="+String(pim[toDec(Matrix[4],Matrix[5])-1].MatrixTime[2]-1));
            setDataNextion("va3.val="+String(pim[toDec(Matrix[4],Matrix[5])-1].MatrixTime[3]-1));
            setDataNextion("St.val="+String(pim[toDec(Matrix[4],Matrix[5])-1].MatrixTime[4]));
      break;
      case 2:
            setDataNextion("va0.val="+String(pr[toDec(Matrix[4],Matrix[5])-1].MatrixTime[0]-1));
            setDataNextion("va1.val="+String(pr[toDec(Matrix[4],Matrix[5])-1].MatrixTime[1]-1));
            setDataNextion("va2.val="+String(pr[toDec(Matrix[4],Matrix[5])-1].MatrixTime[2]-1));
            setDataNextion("va3.val="+String(pr[toDec(Matrix[4],Matrix[5])-1].MatrixTime[3]-1));
            setDataNextion("St.val="+String(pr[toDec(Matrix[4],Matrix[5])-1].MatrixTime[4]));
      break;
      case 3:
            setDataNextion("va0.val="+String(van[toDec(Matrix[4],Matrix[5])-1].MatrixTime[0]-1));
            setDataNextion("va1.val="+String(van[toDec(Matrix[4],Matrix[5])-1].MatrixTime[1]-1));
            setDataNextion("va2.val="+String(van[toDec(Matrix[4],Matrix[5])-1].MatrixTime[2]-1));
            setDataNextion("va3.val="+String(van[toDec(Matrix[4],Matrix[5])-1].MatrixTime[3]-1));
            setDataNextion("St.val="+String(van[toDec(Matrix[4],Matrix[5])-1].MatrixTime[4]));
      break;
      case 4:
            setDataNextion("va0.val="+String(mlg[toDec(Matrix[4],Matrix[5])-1].MatrixTime[0]-1));
            setDataNextion("va1.val="+String(mlg[toDec(Matrix[4],Matrix[5])-1].MatrixTime[1]-1));
            setDataNextion("va2.val="+String(mlg[toDec(Matrix[4],Matrix[5])-1].MatrixTime[2]-1));
            setDataNextion("va3.val="+String(mlg[toDec(Matrix[4],Matrix[5])-1].MatrixTime[3]-1));
            setDataNextion("St.val="+String(mlg[toDec(Matrix[4],Matrix[5])-1].MatrixTime[4]));
      break;
      case 5:
            setDataNextion("va0.val="+String(eng[toDec(Matrix[4],Matrix[5])-1].MatrixTime[0]-1));
            setDataNextion("va1.val="+String(eng[toDec(Matrix[4],Matrix[5])-1].MatrixTime[1]-1));
            setDataNextion("va2.val="+String(eng[toDec(Matrix[4],Matrix[5])-1].MatrixTime[2]-1));
            setDataNextion("va3.val="+String(eng[toDec(Matrix[4],Matrix[5])-1].MatrixTime[3]-1));
            setDataNextion("St.val="+String(eng[toDec(Matrix[4],Matrix[5])-1].MatrixTime[4]));
      break;
      default:
           Error();
      break;
   }
}

void getSettingSMS(){
  for(int i=0;i<6;i++){
    if(settingSMS[i]==1){
      setDataNextion("bt17_"+String(i+1)+".val=1");
    }
  }
}

void sendPinSMS(){
  if(PINcode>10000){
    Serial3.println("P"+toStringPin(PINcode-10000));
  }else {
    Serial3.println("P1234");
  }
  
}
void smsSetting(int Matrix[MTR]){
    String str="";
    for(int i=0;i<4;i++){
      settingSMS[i]=Matrix[i+2];
      str+=Matrix[i+2];
    }
    if(EEPROM.put(AD_SETTING_SMS,settingSMS)){
      successMessage(511);
    }
    Serial3.println("S"+str);
  }

void getNumObj(){
  for(int i=0;i<6;i++){
      setDataNextion("n"+String(i)+".val="+String(numberObj[i]));
    }
  }
void getNumPae(){
  for(int i=0;i<numberObj[1];i++){
    if(relationPae[i][0]>0 && relationPae[i][0]<6){
      setDataNextion("b32_"+String(i+2)+".picc=62");
    }else {
      setDataNextion("b32_"+String(i+2)+".picc=60");
    }
    setDataNextion("vis b32_"+String(i+2)+",1");
  }
  }
void getPae(int pr){
    for(int i=0;i<2;i++){
      setDataNextion("va"+String(i+1)+".val="+String(relationPae[pr-1][i]));
    }
  } 
void getNumSector(){
  int count=0;
  for(int i=0;i<6;i++){
    if(sector[i][0]>0 && sector[i][0]<numberObj[2]){
      setDataNextion("vis b29_"+String(i+2)+",1");
      count++;
    }
  }
  setDataNextion("NbSec.val="+String(count));
  }
void getSector(int sec){
    for(int i=0;i<6;i++){
      setDataNextion("n"+String(i+1)+".val="+String(sector[sec-1][i]));
    }
    setDataNextion("vanMax.val="+String(numberObj[2]));
  }
void getNumRelation(){
  int count=0;
  for(int i=0;i<numberObj[1];i++){
    if(relationObj[i][0]>0 && relationObj[i][0]<numberObj[2]){
      setDataNextion("b31_"+String(i+2)+".picc=61");
      count++;
    }else {
      setDataNextion("b31_"+String(i+2)+".picc=60");
    }
    setDataNextion("vis b31_"+String(i+2)+",1");
  }
}

void getRelation(int pr){
     setDataNextion("n0.val="+String(pr));
    for(int i=0;i<6;i++){
      setDataNextion("n"+String(i+1)+".val="+String(relationObj[pr-1][i]));
    }
    
  }

void getModeSys(){
   for(int i=0;i<3;i++){
    if(ModeSys[i]==1){
      setDataNextion("bt15_"+String(i)+".val=1");
    }
  }
}
void getStateWc(){
    //getTimeNextion();
    setDataNextion("t5_0.txt=\""+toString(Hour)+"\"");
    setDataNextion("t5_2.txt=\""+toString(Minute)+"\"");
    setDataNextion("t5_3.txt=\""+toString(Date)+"\"");
    setDataNextion("t5_5.txt=\""+toString(Month)+"\"");
    setDataNextion("t5_8.txt=\""+toString(Year)+"\"");
    if(ComWc){
      setDataNextion("t5_10.txt=\"R\"");
      digitalWrite(LED_WIRLESS_COM,HIGH);
    }else {
      setDataNextion("t5_10.txt=\"N.R\"");
      digitalWrite(LED_WIRLESS_COM,LOW);
    }
    float PerLev=LevelNet/31.0;
    if(ComGsm){
      setDataNextion("t5_6.txt=\""+String(PerLev*100)+"%\"");
    }else {
      setDataNextion("t5_6.txt=\""+String(PerLev*100)+"%\"");
    }
    
    
     
}
void getNumClient(){
   setDataNextion("t27_0.txt=\""+Phone+"\"");
}
/////////// PARTIE 6 les etats
void setState(int Matrix[MTR]){
  if(Matrix[3]<=NUMBER_OBJ && toDec(Matrix[4],Matrix[5])<=NUMBER_MAX){
    objState[Matrix[3]-1][toDec(Matrix[4],Matrix[5])-1]=Matrix[6]-2;
    if(Matrix[6]==3){
      addHist(getName(Matrix[3],toDec(Matrix[4],Matrix[5]))+"ON");
    }else if(Matrix[6]==4){
      addHist(getName(Matrix[3],toDec(Matrix[4],Matrix[5]))+"OFF");
    }else if(Matrix[6]==9) {
      addHist(getName(Matrix[3],toDec(Matrix[4],Matrix[5]))+"DEF");
    }else {
      addHist(getName(Matrix[3],toDec(Matrix[4],Matrix[5]))+"ERR");
    } 
    getState(Matrix);  //4  1 4 101 < 8 1 0 102 2
  }
}
//// les paramétre de PIN
void pinSetting(int Matrix[MTR]){}
//// les paramétre Mode de démarrage
void modeSys(int Matrix[MTR]){
    for(int i=0;i<3;i++){
      ModeSys[i]=Matrix[i+2];
    }
    if(EEPROM.put(AD_MODE_SYS,ModeSys)){
      successMessage(531);
    }
  }
/////// PARTIE 6 : Réinitialisation du système
void restSys(){
    int j=-10;
    setDataNextion("page Reset");
    Serial3.println("T475");
    for (int i = 1 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
    if(i%400==0){
      setDataNextion("j30_0.val="+String(j+10));
    }
  }
  setup();
  }

void getValueRelation(){
    setDataNextion("prMax.val="+String(numberObj[1]));
    setDataNextion("vanMax.val="+String(numberObj[2]));
}
void getValuePae(){
    setDataNextion("mlgMax.val="+String(numberObj[3]));
    setDataNextion("engMax.val="+String(numberObj[3]));
}
/////// PARTIE 7 : Protection du système
void setDelay(int Matrix[MTR]){}
void tryProto(int Matrix[MTR]){}
void sysLock(int Matrix[MTR]){
    EEPROM[AD_VALIDITY]=toDec(Matrix[3],Matrix[5]);
    Serial3.println("T7"+toString(toDec(Matrix[3],Matrix[5])));
    setDataNextion("page Info"); //t0.txt=\"Certificat de securite non valide.\"
    setDataNextion("t13_0.txt=\"The system has been blocked, contact your supplier !\"");
    Serial2.end();
  }
  
///////////////////////////////
void checkState(int Matrix[]){
    if(Matrix[2]<=5 && toDec(Matrix[3],Matrix[4])<=numberObj[2] &&  toDec(Matrix[3],Matrix[4])>0 && (Matrix[5]==3 || Matrix[5]==4 || Matrix[5]==5 || Matrix[5]==6)){
         objState[Matrix[2]-1][toDec(Matrix[3],Matrix[4])-1]=Matrix[5]-2;
    }
}
////////////////// les fonctions EEPROM
////// Mettre des valeurs 
void Virginity(int value ){
  EEPROM[AD_VIRGINITY]=value;
  defaultPar();
  setup();
}
// les données
bool loadingData(){
  delay(200);setDataNextion("j0.val=30");
//  byte Mode=EEPROM[AD_MODE_SYS];
//  if(Mode==1){
//    ModeSys=true;
//    EEPROM[AD_MODE_SYS]=1;
//  } else if(Mode==2){
//    ModeSys=false;
//    EEPROM[AD_MODE_SYS]=2;
//  }
  EEPROM.get(AD_MODE_SYS,ModeSys);
  EEPROM.get(AD_PIN,PINcode);
  delay(100);setDataNextion("j0.val=40");
  EEPROM.get(AD_PHONE,numberPhone);
  delay(100);setDataNextion("j0.val=45");
  Phone=toString(numberPhone);
  delay(100);setDataNextion("j0.val=50");
  EEPROM.get(AD_NUMBER_OBJ,numberObj);
  delay(100);setDataNextion("j0.val=55");
  EEPROM.get(AD_SETTING_SMS,settingSMS);
  delay(100);setDataNextion("j0.val=60");
  EEPROM.get(AD_SECTOR,sector);
  delay(100);setDataNextion("j0.val=65");
  EEPROM.get(AD_RELATION_OBJ,relationObj);
  delay(100);setDataNextion("j0.val=70");
  EEPROM.get(AD_RELATION_PAE,relationPae);
  for(int i=0;i<10;i++)
      pim[i].getProg();
  delay(100);setDataNextion("j0.val=75");
  for(int i=0;i<5;i++)
      pr[i].getProg();
  delay(100);setDataNextion("j0.val=80");
  for(int i=0;i<15;i++)
      van[i].getProg();
  
  delay(100); setDataNextion("j0.val=85");
  for(int i=0;i<5;i++)
      mlg[i].getProg();
  delay(100);
  setDataNextion("j0.val=90");
  for(int i=0;i<5;i++)
      eng[i].getProg();    
  putDataNextion();
  delay(100);
  setDataNextion("j0.val=95");
  intState();
  return true;
}

void putDataNextion(){
   setDataNextion("pim.val="+String(numberObj[0]));
   setDataNextion("pr.val="+String(numberObj[1]));
   setDataNextion("van.val="+String(numberObj[2]));
   setDataNextion("mlg.val="+String(numberObj[3]));
   setDataNextion("ang.val="+String(numberObj[4]));
}


////////////////// les fonctions du vérification
///// fonction pour vérifier 
  //  Vérification la virginité.
bool checkVirginity(){
  int value=EEPROM[AD_VIRGINITY];
  if(value==1){
    return true;
  }else if(value==2){
    return false;
  }
}
  //  Vérification la validité
bool checkValidity(){
  bool k;
  EEPROM[AD_VALIDITY]==95? k=false : k=true;
  return k;
}
/////////////// NEXTION
// Mettre les données à Nextion 
void setDataNextion(String data) {
  Serial2.print(data);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff); 
  Serial2.write(0xff); 
  Serial2.write(0xff);

}
// Affiche les informations et les erreurs.
void popupMessage(String msg){
   setDataNextion("page errors");
   setDataNextion("msg.txt=\""+msg+"\"");
   addHist(msg);
}
// Affiche les informations 
void successMessage(){
   if(dataFromNex){
      setDataNextion("page succes");
   }
}
void successMessage(int retCode){
    String msg="Gx";
    if(dataFromNex==false){
      msg+=String(retCode);
      Serial3.println(msg);
    }else {
      setDataNextion("page succes");
    }
}

int toDec(int o,int p){
  return o*10+p;
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

String toString(int Matrix[9]){
  String str="";
  for(int i=0;i<9;i++)
    str+=Matrix[i];
  return str; 
}
//  Obtenir l'heure d'horloge 
void getTimeNextion(){
  setDataNextion("t28_3.txt=\""+toString(Hour)+"\"");
  setDataNextion("t28_4.txt=\""+toString(Minute)+"\"");
  setDataNextion("t28_0.txt=\""+toString(Date)+"\"");
  setDataNextion("t28_1.txt=\""+toString(Month)+"\"");
  setDataNextion("t28_2.txt=\""+toString(Year)+"\"");
}

void getTime(){
  Year=Clock.getYear();
  Month=Clock.getMonth(Century);
  Date=Clock.getDate();
  Hour = Clock.getHour(h12, PM);
  Minute= Clock.getMinute();
  Second= Clock.getSecond();
  Item::Hour=Hour;
  Item::Minute=Minute;
  Item::Second=Second;
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
// fonction class
void autoRunObj(){
  for(int i=0;i<10;i++)
      pim[i].autoRun();
  for(int i=0;i<5;i++)
      pr[i].autoRun();
  for(int i=0;i<15;i++)
      van[i].autoRun();
  for(int i=0;i<5;i++)
      mlg[i].autoRun();
  for(int i=0;i<5;i++)
      eng[i].autoRun();  
}


void sendCmd(int cmd){
      Serial3.println("R"+String(cmd));
}
// fonction pour ajouter l'historique.
void addHist(String hist)
{
  String Time,Day;
  if(Hour<10)   { Time  ="0"+String(Hour); }     else { Time=String(Hour); }
  if(Minute<10) { Time+=" : 0"+String(Minute); }  else { Time+=" : "+String(Minute); }
  if(Date<10)   { Day   ="0"+String(Date); }    else { Day=String(Date); }
  if(Month<10)  { Day+="/0"+String(Month); }   else { Day+="/"+String(Month); }
  if(!sdInit()){
    popupMessage("Erreur de creation de fichier");
    return 0;
  }
  csv.addField(hist.c_str());
  csv.addField(Time.c_str());
  csv.addField(Day.c_str());
  csv.addLine();
  csv.close();
}
////// Initialization de la module carte SD !! 
bool sdInit(){
  String HistFile="Hist_"+String(Date)+"_"+String(Month)+"_"+String(Year)+".csv";
  if(!sd.exists(HistFile.c_str())){
      csv.open(HistFile.c_str(), O_RDWR | O_CREAT);
      csv.gotoBeginOfFile();
      while(!csv.isEndOfLine()) csv.nextLine();
      return true;
  }else if(sd.exists(HistFile.c_str())){
      csv.open(HistFile.c_str(), O_RDWR);
      csv.gotoBeginOfFile();
      while(!csv.isEndOfLine()) csv.nextLine();
      return true;
  }else{
    return false;
  }
}

void Error(){
  
}


void RdCmd(int Bpin){
  int Mpin[4];
  int i=0;
  String Histo="";
  while (Bpin > 0)
  {
    Mpin[i] = Bpin%10;
    Bpin /= 10;
    i++;
  }
if((Mpin[1]+(Mpin[2]*10))<=15 && (Mpin[1]+(Mpin[2]*10))>0 && Mpin[3]<=5){
   objState[Mpin[3]-1][Mpin[1]+(Mpin[2]*10)-1]=Mpin[0];
}
 if(Mpin[0]==1)
    addHist(getName(Mpin[3],Mpin[1]+(Mpin[2]*10))+"ON");
 else
    addHist(getName(Mpin[3],Mpin[1]+(Mpin[2]*10))+"OFF");
}
// Initialisation  les pins des vannes 
void intValve(){
  for(int i=0;i<=numberObj[2];i++){
    pinMode(i+2,OUTPUT);
    digitalWrite(i+2,HIGH);
  }
}
// 
void funValve(int van,int action){
    if(action==1){
      digitalWrite(van+1,LOW);
      RdCmd(3001+van*10);
    }else if(action==2){
      digitalWrite(van+1,HIGH);
      RdCmd(3002+van*10);
    }
}
void intLed(){
  pinMode(LED_START,OUTPUT);
  pinMode(LED_WIRLESS_COM,OUTPUT);
  pinMode(LED_CHECK_GSM,OUTPUT);
  digitalWrite(LED_START,LOW);
  digitalWrite(LED_WIRLESS_COM,LOW);
  digitalWrite(LED_CHECK_GSM,LOW);
}
void ckeckWirless(){
   Serial3.println("Wc1");
}
void ckeckWirlessState(){
   if(ComWc==false){
   digitalWrite(LED_WIRLESS_COM,HIGH);
   delay(70);
   digitalWrite(LED_WIRLESS_COM,LOW);
   delay(70);
   digitalWrite(LED_WIRLESS_COM,HIGH);
   ComWc=true;
   }
   Last=millis();
}
void ckeckGsmState(int level){
  LevelNet=level;
  if(level>19){
    digitalWrite(LED_CHECK_GSM,HIGH);
    delay(70);
    digitalWrite(LED_CHECK_GSM,LOW);
    delay(70);
    digitalWrite(LED_CHECK_GSM,HIGH);
    ComGsm=true;
  }else {
    ComGsm=false;
    digitalWrite(LED_CHECK_GSM,LOW);
  }
  ckeckWirlessState();
}
void ckeckHCState(){
  if(millis()-Last>PERIOD && ComWc==true){
    ComWc=false;
    ComGsm=false;
    digitalWrite(LED_WIRLESS_COM,LOW);
    digitalWrite(LED_CHECK_GSM,LOW);
  }
}
void defaultPar(){
  ModeSys[0]=ModeSys[1]=ModeSys[2]=1;
  EEPROM.put(AD_MODE_SYS,ModeSys);
  EEPROM.put(AD_PIN,PINcode);
}
// Application
void getAppPin(int Matrix[]){
    long appPin=Matrix[3]*1000+Matrix[4]*100+Matrix[5]*10+Matrix[6];
    appPin=appPin*3;
    Serial3.println("GP-"+String(appPin));
}

void getAppNumObj(){
    String msg="GD-";
    msg+=toString(numberObj[0]);
    msg+=toString(numberObj[1]);
    msg+=toString(numberObj[2]);
    msg+=toString(numberObj[3]);
    msg+=toString(numberObj[4]);
    msg+=toString(numberObj[5]);
    msg+=toString(numberObj[5]);
    Serial3.println(msg);
}

void getAppState(){
    String msg="GS-";
    delay(60);
    for(int i=0;i<numberObj[0];i++){
      msg+=String(objState[0][i]);
    }
    for(int i=0;i<numberObj[1];i++){
      msg+=String(objState[1][i]);
    }
    for(int i=0;i<numberObj[2];i++){
      msg+=String(objState[2][i]);
    }
    for(int i=0;i<numberObj[3];i++){
      msg+=String(objState[3][i]);
    }
    for(int i=0;i<numberObj[4];i++){
      msg+=String(objState[4][i]);
    }
    Serial3.println(msg);
}
void sendStateApp(int key,int value){
    String msg="Gx";
    msg+=String(key);
    msg+=String(value);
    if(!dataFromNex || settingSMS[1]==1){
      Serial3.println(msg);
    }
}
void intState(){
    for(int i=0;i<numberObj[0];i++){
      objState[0][i]=2;
    }
    for(int i=0;i<numberObj[1];i++){
      objState[1][i]=2;
    }
    for(int i=0;i<numberObj[2];i++){
      objState[2][i]=2;
    }
    for(int i=0;i<numberObj[3];i++){
      objState[3][i]=2;
    }
    for(int i=0;i<numberObj[4];i++){
      objState[4][i]=2;
    }
    for(int i=0;i<numberObj[5];i++){
      objState[5][i]=2;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
