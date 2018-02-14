#include "Arduino.h"
#include "Item.h"
int  Item::Hour,Item::Minute,Item::Second;
//Constructeur
Item::Item(int id,int number)
{
  this->IdObj=id;
  this->NumberObj=number;
  this->Cmd=id*1000+number*10;
}

//Function Set Programme *********************************************

void Item::updateProg(){
  showMatrix(MatrixTime,5);
  int k = this->NumberObj * sizeof(MatrixTime);
  Serial.println(k);
  switch((this->IdObj))
  {
       case 1:
              //pompe immerger
              EEPROM.put(AD_PROG_PIM+k,MatrixTime);
              break;
       case 2:
              //pompe refolemnt
              EEPROM.put(AD_PROG_PR+k,MatrixTime);
              break;
       case 3:
              //les vannes 
              EEPROM.put(AD_PROG_VAN+k,MatrixTime);
              break;
       case 4:
              //les pompes angre
              EEPROM.put(AD_PROG_MLG+k,MatrixTime);
              break;
       case 5:
              //Melengeur Angre
              EEPROM.put(AD_PROG_ENG+k,MatrixTime);
              break;
       default:
              break;
  }
}

void Item::setProg(int sHr,int sMin,int eHr,int eMin,int Type)
{
  //int Matrix[5]= {sHr,sMin,eHr,eMin,Type};
 MatrixTime[0] =sHr+1;
 MatrixTime[1] =sMin+1;
 MatrixTime[2] =eHr+1;
 MatrixTime[3] =eMin+1;
 MatrixTime[4] =Type;
 this->updateProg();  
}


void Item::getProg(){
  int k = this->NumberObj * sizeof(MatrixTime);
  int AD = 0; // Tempor.
  switch((this->IdObj))
  {
       case 1:
              //pompe immerger
              EEPROM.get(AD_PROG_PIM+k,MatrixTime);
              AD=AD_PROG_PIM+k;
              break;
       case 2:
              //pompe refolemnt
              EEPROM.get(AD_PROG_PR+k,MatrixTime);
              AD=AD_PROG_PR+k;
              break;
       case 3:
              //les vannes 
              EEPROM.get(AD_PROG_VAN+k,MatrixTime);
              AD=AD_PROG_VAN+k;
              break;
       case 4:
              //les pompes angre
              EEPROM.get(AD_PROG_MLG+k,MatrixTime);
              AD=AD_PROG_MLG+k;
              break;
       case 5:
              //Melengeur Angre
              EEPROM.get(AD_PROG_ENG+k,MatrixTime);
              AD=AD_PROG_ENG+k;
              break;
       default:
              break;
  }
  Serial.print("Objet : ");
  Serial.print(IdObj);
  Serial.print("   Numéro : ");
  Serial.print(NumberObj);
  Serial.print("   Adresse ");
  Serial.println(AD);
  showMatrix(MatrixTime,5);
}

void Item::autoRun()
{
  if(this->MatrixTime[5]==3 || 4){
   if(Hour==this->MatrixTime[0]-1 && Minute==this->MatrixTime[1]-1 && Second==0) {
        if(MatrixTime[5]==3){
          this->MatrixTime[5]=5;
          this->updateProg();
        }
         this->runObj(1);
      }  else if(Hour==this->MatrixTime[2]-1 && Minute==this->MatrixTime[3]-1 && Second==0)
      {
         this->runObj(2);
      }
     
  }
}


bool Item::runObj(int Action)
{
  switch(this->IdObj)
  {
    case 1:
            if(Action==1 && ckeckPmp()){
             sendCmd(Cmd+Action);
             return true;
            } else if(Action==1){
             Serial.println("la pompe immérgée: n'a pas démarré");
             return false;
            } else if(Action==2){
             //offObj(1);
             sendCmd(Cmd+Action);
             return true;
            }
           break;
    case 2:
          if(Action==1 && ckeckPmp()){
             if(checkVan()) {
                //runS();
                sendCmd(Cmd+Action);
                return true;
              } else {
                //Erreur(2);
                Serial.println("la pompe de refoulement : n'a pas démarré a cause de nombre des vannes.");
                return false;
              }
          } else if(Action==1) {
            //Erreur(1);
             Serial.println("la pompe de refoulement : n'a pas démarré.");
             return false;
          } else if(Action==2 && checkPae()) {
            //offObj(1);
             sendCmd(Cmd+Action);
             return true;
          } else if(Action==2) {
            //Erreur(5);//la pompe a angre n'est pas etainde
            Serial.println("la pompe a angre n'est pas etainde");
             return false;
          }
           break;
    case 3://****************************************************
           //Vannes
          if(Action==1) {
            //runS();
             sendCmd(Cmd+Action);
             return true;
          } else if(Action==2 && ckeckPrVan()) {
            //offObj(3);
             sendCmd(Cmd+Action);
             return true;
          } else if(Action==2)  {
            //Erreur(3);//
            Serial.println("pompe doit etre etainde");
             return false;
          }
           break;
    case 4://****************************************************
          //pompe a angre 
          if(Action==1 && checkPrPae()) {
             //runS();
             sendCmd(Cmd+Action);
             return true;
          } else if(Action==1)
          {
             //sendCmd(Cmd+Action);
             return false; 
          } else if(Action==2)
          {
            sendCmd(Cmd+Action);
            return true;
          }
          break;
     case 5:
          sendCmd(Cmd+Action);
          break;
    default:
           break;
  }
}


bool Item::ckeckPmp(){
  int count=0;
  for(int i=0;i<2;i++)
  {
    for(int j=0;j<10;j++)
    {
      if(objState[i][j]==1)
      {
        count++;
        if(count>numberObj[6]) return false;
      }
    }
  }
  //if(count<numberObj[6]) 
  return true;
}

bool Item::checkVan() {
  int count=0;
  for(int i=0;i<5;i++)
  {
    if(objState[2][(relationObj[this->NumberObj-1][i])-1]==1)
    {
       count++;
       if(count>=relationObj[this->NumberObj-1][5]) return true;
    }
  }
  //if(count<relationObj[this->numberObj-1][5]) 
    return false;
}


bool Item::ckeckPrVan()
{
  for(int i=0;i<5;i++) {
    for(int j=0;j<5;j++) {
     if(relationObj[i][j]==this->NumberObj) 
        if(objState[1][i]==1 && !ckeckVanPr(i)) return false;
    }
  }
  return true;
}


bool Item::checkPrPae()
{
  if(objState[1][relationPae[this->NumberObj-1][1]-1]==1) return true;
  return false;
}
//**************************condition 5***************************
bool Item::checkPae()
{
  if(objState[3][relationPae[this->NumberObj-1][1]]==1) return false;
  return true;
}

bool Item::ckeckVanPr(int pr) 
{
  int count=0;
  for(int i=0;i<5;i++){
    if(objState[2][(relationObj[pr][i])-1]==1){
      count++;
      if(count>relationObj[pr][5]) return true;
    }
  }
  return false;
}
