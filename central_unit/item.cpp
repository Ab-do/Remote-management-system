#include "Arduino.h"
#include "Item.h"
int  Item::Hour,Item::Minute,Item::Second;
//Constructeur
Item::Item(int id,int number)
{
  this->IdObj=id;
  this->NumberObj=number;
  this->cmd=id*1000+number*10;
}

//Function Set Programme *********************************************
void Item::setProg(int sHr,int sMin,int eHr,int eMin,int Type)
{
  int add;
  int Matrix[5]={sHr,sMin,eHr,eMin,Type};
  for(int i=0;i<5;i++){
    Serial.println(Matrix[i]);
  }
  Serial.println("------------");
  //sizeof(mT)
  int k = this->NumberObj * sizeof(Matrix);
  Serial.println(k);
  this->sH=sHr;
  this->sM=sMin;
  this->eH=eHr;
  this->eM=eMin;
  switch((this->IdObj))
  {
       case 1:
              //pompe immerger
              EEPROM.put(500+k,Matrix);
              break;
       case 2:
              //pompe refolemnt
              EEPROM.put(610+k,Matrix);
              break;
       case 3:
              //les vannes 
              EEPROM.put(790+k,Matrix);
              break;
       case 4:
              //les pompes angre
              EEPROM.put(670+k,Matrix);
              break;
       case 5:
              //Melengeur Angre
              EEPROM.put(730+k,Matrix);
              break;
       default:
              break;
  }
}
//********************************************

//function Auto Runobj
void Item::autoRun()
{
  
  if(Hour==this->sH && Minute==sM && Second==0)
      {
         this->runObj(1);
      }
      else if(Hour==this->eH && Minute==eM && Second==0)
      {
         this->runObj(2);
      }
}


//function  Run Objet
bool Item::runObj(int Action)
{
  
  Serial.println(cmd+Action);
  Serial3.write(cmd+Action);
}

void static Item::setTime(int Hr,int Min,int Sec)
{
  Hour=Hr;
  Minute=Min;
  Second=Sec;
}
