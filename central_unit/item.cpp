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
  int k = this->NumberObj * sizeof(this->MatrixTime);
  switch((this->IdObj))
  {
       case 1:
              //pompe immerger
              if(EEPROM.put(AD_PROG_PIM+k,this->MatrixTime)){
                successMessage();
                sendStateApp(22,100+this->NumberObj);
              }
              break;
       case 2:
              //pompe refolemnt
              if(EEPROM.put(AD_PROG_PR+k,this->MatrixTime)){
                successMessage();
                sendStateApp(22,200+this->NumberObj);
              }
              break;
       case 3:
              //les vannes 
              if(EEPROM.put(AD_PROG_VAN+k,this->MatrixTime)){
                successMessage();
                sendStateApp(22,300+this->NumberObj);
              }
              break;
       case 4:
              if(EEPROM.put(AD_PROG_MLG+k,this->MatrixTime)){
                successMessage();
                sendStateApp(22,400+this->NumberObj);
              }
              break;
       case 5:
              if(EEPROM.put(AD_PROG_ENG+k,this->MatrixTime)){
                successMessage();
                sendStateApp(22,500+this->NumberObj);
              }
              break;
       default:
              break;
  }
}

void Item::setProg(int sHr,int sMin,int eHr,int eMin,int Type,int Id)
{
  //int Matrix[5]= {sHr,sMin,eHr,eMin,Type};
 this->MatrixTime[0][Id] =sHr+1;
 this->MatrixTime[1][Id] =sMin+1;
 this->MatrixTime[2][Id] =eHr+1;
 this->MatrixTime[3][Id] =eMin+1;
 this->MatrixTime[4][Id] =Type;
 this->updateProg();  
}


void Item::getProg(){
  int k = this->NumberObj * sizeof(this->MatrixTime);
  int AD = 0; // Tempor.
  switch((this->IdObj))
  {
       case 1:
              //pompe immerger
              EEPROM.get(AD_PROG_PIM+k,this->MatrixTime);
              AD=AD_PROG_PIM+k;
              break;
       case 2:
              //pompe refolemnt
              EEPROM.get(AD_PROG_PR+k,this->MatrixTime);
              AD=AD_PROG_PR+k;
              break;
       case 3:
              //les vannes 
              EEPROM.get(AD_PROG_VAN+k,this->MatrixTime);
              AD=AD_PROG_VAN+k;
              break;
       case 4:
              //Melengeur 
              EEPROM.get(AD_PROG_MLG+k,this->MatrixTime);
              AD=AD_PROG_MLG+k;
              break;
       case 5:
              //les pompes ?? engrais
              EEPROM.get(AD_PROG_ENG+k,this->MatrixTime);
              AD=AD_PROG_ENG+k;
              break;
       default:
              break;
  }

}

void Item::autoRun()
{
  for(int ID=0;ID<5;ID++){
  if(this->MatrixTime[4][ID]==3 || this->MatrixTime[4][ID]==4){
   if(Hour==this->MatrixTime[0][ID]-1 && Minute==this->MatrixTime[1][ID]-1 && Second==0) {
        this->runObj(1);
        delay(800);
      }  else if(Hour==this->MatrixTime[2][ID]-1 && Minute==this->MatrixTime[3][ID]-1 && Second==0)
      {
         this->runObj(2);
         delay(800);
         if(this->MatrixTime[4][ID]==3){
          this->MatrixTime[4][ID]=5;
          this->updateProg();
        }
      }
    }
  }
}


bool Item::runObj(int Action)
{
  switch(this->IdObj)
  {
    case 1:
            if(Action==1 && ckeckPmp()){
             sendCmd(this->Cmd+Action);
             //sendStateApp(this->Cmd+Action);
             return true;
            } else if(Action==1){
             popupMessage("Vous avez atteint le nombre max des pompes d??marr??es",64);
             sendStateApp(21,this->Cmd+5);
             return false;
            } else if(Action==2){
             //offObj(1);
             sendCmd(this->Cmd+Action);
             //sendStateApp(this->Cmd+Action);
             return true;
            }else if(Action==8){
              sendCmd(this->Cmd);
            }
           break;
    case 2:
          if(Action==1 && ckeckPmp()){
             if(checkVan()) {
                //runS();
                sendCmd(this->Cmd+Action);
                //sendStateApp(this->Cmd+Action);
                return true;
              } else {
                //Erreur(2);
                popupMessage("Les vannes sont ferm??es",65);
                sendStateApp(21,this->Cmd+5);
                return false;
              }
          } else if(Action==1) {
            //Erreur(1);
             popupMessage("Vous avez atteint le nombre max des pompes d??marr??es.",64);
             sendStateApp(21,this->Cmd+6);
             return false;
          } else if(Action==2 && checkPae()) {
            //offObj(1);
             sendCmd(this->Cmd+Action);
             //sendStateApp(this->Cmd+Action);
             return true;
          } else if(Action==2) {
            //Erreur(5);//la pompe a angre n'est pas etainde
             popupMessage("Vous pouvez pas ??teindre la pompe de refoulement N : "+String(this->NumberObj),66);
             sendStateApp(21,this->Cmd+7);
             return false;
          }else if(Action==8){
              sendCmd(this->Cmd);
            }
           break;
    case 3://****************************************************
          if(Action==1) {
             funValve(this->NumberObj,Action);
             sendStateApp(21,this->Cmd+3);
             return true;
          } else if(Action==2 && ckeckPrVan()) {
             funValve(this->NumberObj,Action);
             sendStateApp(21,this->Cmd+4);
             return true;
          } else if(Action==2)  {
            //Erreur(3);//
            popupMessage("Vous pouvez pas ??teindre la vanne N : "+String(this->NumberObj),67);
            sendStateApp(21,this->Cmd+5);
             return false;
          }else if(Action==8){
              sendCmd(this->Cmd);
            }
           break;
    case 4://****************************************************
          if(Action==1 || Action==2){
            sendCmd(this->Cmd+Action);
            //sendStateApp(,this->Cmd+Action);
          } else if(Action==8){
              sendCmd(this->Cmd);
          }
          break;
     case 5:
          //pompe a angre 
          if(Action==1 && checkPrPae()) {
             sendCmd(this->Cmd+Action);
             //sendStateApp(this->Cmd+Action);
             return true;
          } else if(Action==1)
          {
             //sendCmd(Cmd+Action);
             popupMessage("Vous pouvez pas d??marrer la pompe ?? engrais N : "+String(this->NumberObj),68);
             sendStateApp(21,this->Cmd+5);
             return false; 
          } else if(Action==2)
          {
            sendCmd(this->Cmd+Action);
            //sendStateApp(this->Cmd+Action);
            return true;
          }else if(Action==8){
              sendCmd(this->Cmd);
            }
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
        if(count>=numberObj[5]) return false;
      }
    }
  }
  //if(count<numberObj[6]) 
  return true;
}

bool Item::checkVan() {
  int count=0;
  if(relationObj[this->NumberObj-1][5]!=0){
  for(int i=0;i<5;i++)
  {
    if(objState[2][(relationObj[this->NumberObj-1][i])-1]==1)
    {
       count++;
       if(count>=relationObj[this->NumberObj-1][5]) return true;
    }
  } 
    return false;
  }else {
    return true;
  }
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
  for(int i=0;i<5;i++){
    if(objState[1][relationPae[i][0]-1]==1){ return true; }
  }
  return false;
}
//**************************condition 5***************************
bool Item::checkPae()
{ 
  if(objState[4][relationPae[(this->NumberObj)-1][0]-1]==1){ return false; }
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
