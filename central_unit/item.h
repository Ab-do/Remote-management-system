//extern const int NUMBER_OBJ;
//extern const int NUMBER_MAX;
extern int numberObj[6];        // nombre des objets de systeme
extern int settingSMS[6];        // nombre des objets de systeme
extern int objState[5][15];        // les etates des objets
extern int sector[6][6];        // les secteurs 
extern int relationObj[6][6]; // les relation entre les pompes de refoulements et les vannes
extern void sendCmd(int cmd);
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
      int IdObj,NumberObj,Cmd,sH=99,eH=99,sM=99,eM=99,ProgType;
      //int MatrixTime={99};
      static int  Hour,Minute,Second;
  };
#endif
