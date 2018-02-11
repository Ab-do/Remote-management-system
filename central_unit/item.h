#ifndef Item_h
#define Item_h
#include "Arduino.h"
#include<EEPROM.h>
class Item{
   public :
      Item(int id,int number);
      void autoRun();
      bool runObj(int Actin);
      void setProg(int sHr,int sMin,int eHr,int eMin,int Type);
      void static setTime(int Hr,int Min,int Sec);
    private :
      int IdObj,NumberObj,cmd,sH=99,eH=99,sM=99,eM=99,ProgType;
      //int MatrixTime={99};
      static int  Hour,Minute,Second;
  };
#endif
