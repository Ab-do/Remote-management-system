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
                sendStateApp(22,100+k);
              }
              break;
       case 2:
              //pompe refolemnt
              if(EEPROM.put(AD_PROG_PR+k,this->MatrixTime)){
                successMessage();
                sendStateApp(22,200+k);
              }
              break;
       case 3:
              //les vannes 
              if(EEPROM.put(AD_PROG_VAN+k,this->MatrixTime)){
                successMessage();
                sendStateApp(22,300+k);
              }
              break;
       case 4:
              //les pompes angre
              if(EEPROM.put(AD_PROG_MLG+k,this->MatrixTime)){
                successMessage();
                sendStateApp(22,400+k);
              }
              break;
       case 5:
              //Melengeur Angre
              if(EEPROM.put(AD_PROG_PIM+k,this->MatrixTime)){
                successMessage();
                sendStateApp(22,500+k);
              }
              break;
       default:
              break;
  }
}

void Item::setProg(int sHr,int sMin,int eHr,int eMin,int Type)
{
  //int Matrix[5]= {sHr,sMin,eHr,eMin,Type};
 this->MatrixTime[0] =sHr+1;
 this->MatrixTime[1] =sMin+1;
 this->MatrixTime[2] =eHr+1;
 this->MatrixTime[3] =eMin+1;
 this->MatrixTime[4] =Type;
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
              //les pompes angre
              EEPROM.get(AD_PROG_MLG+k,this->MatrixTime);
              AD=AD_PROG_MLG+k;
              break;
       case 5:
              //Melengeur Angre
              EEPROM.get(AD_PROG_ENG+k,this->MatrixTime);
              AD=AD_PROG_ENG+k;
              break;
       default:
              break;
  }

}

void Item::autoRun()
{
  if(this->MatrixTime[4]==3 || this->MatrixTime[4]==4){
   if(Hour==this->MatrixTime[0]-1 && Minute==this->MatrixTime[1]-1 && Second==0) {
        this->runObj(1);
        delay(800);
      }  else if(Hour==this->MatrixTime[2]-1 && Minute==this->MatrixTime[3]-1 && Second==0)
      {
         this->runObj(2);
         delay(800);
         if(this->MatrixTime[4]==3){
          this->MatrixTime[4]=5;
          this->updateProg();
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
             popupMessage("Vous avez atteint le nombre max des pompes démarrées",64);
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
                popupMessage("Les vannes sont fermées",65);
                sendStateApp(21,this->Cmd+5);
                return false;
              }
          } else if(Action==1) {
            //Erreur(1);
             popupMessage("Vous avez atteint le nombre max des pompes démarrées.",64);
             sendStateApp(21,this->Cmd+6);
             return false;
          } else if(Action==2 && checkPae()) {
            //offObj(1);
             sendCmd(this->Cmd+Action);
             //sendStateApp(this->Cmd+Action);
             return true;
          } else if(Action==2) {
            //Erreur(5);//la pompe a angre n'est pas etainde
             popupMessage("Vous pouvez pas éteindre la pompe de refoulement N : "+String(this->NumberObj),66);
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
            popupMessage("Vous pouvez pas éteindre la vanne N : "+String(this->NumberObj),67);
            sendStateApp(21,this->Cmd+5);
             return false;
          }else if(Action==8){
              sendCmd(this->Cmd);
            }
           break;
    case 4://****************************************************
          //pompe a angre 
          if(Action==1 && checkPrPae()) {
             sendCmd(this->Cmd+Action);
             //sendStateApp(this->Cmd+Action);
             return true;
          } else if(Action==1)
          {
             //sendCmd(Cmd+Action);
             popupMessage("Vous pouvez pas démarrer la pompe à engrais N : "+String(this->NumberObj),68);
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
     case 5:
          if(Action==1 || Action==2){
            sendCmd(this->Cmd+Action);
            //sendStateApp(,this->Cmd+Action);
          } else if(Action==8){
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
  if(objState[3][relationPae[(this->NumberObj)-1][0]-1]==1){ return false; }
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
