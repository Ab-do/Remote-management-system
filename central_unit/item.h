//extern const int NUMBER_OBJ;
//extern const int NUMBER_MAX;
extern int numberObj[6];        // nombre des objets de systeme
extern int settingSMS[6];        // nombre des objets de systeme
extern int objState[5][15];        // les etates des objets
extern int sector[6][6];        // les secteurs 
extern int relationObj[6][6]; // les relation entre les pompes de refoulements et les vannes
extern int relationPae[5][2];
extern void sendCmd(int cmd);
void showMatrix(int Matrix[20],int a);
int const AD_PROG_PIM=500;
int const AD_PROG_PR=610;
int const AD_PROG_VAN=790;
int const AD_PROG_MLG=670;
int const AD_PROG_ENG=730;
#ifndef Item_h
#define Item_h
#include "Arduino.h"
#include<EEPROM.h>
class Item{
   public :
      Item(int id,int number);
      static int  Hour,Minute,Second;
      void autoRun();
      bool runObj(int Actin);
      void setProg(int sHr,int sMin,int eHr,int eMin,int Type);
      void getProg();
      void updateProg();
      //void static setTime(int Hr,int Min,int Sec);
    private :
      int IdObj,NumberObj,Cmd;
      int MatrixTime[5]={99};
      bool ckeckPmp();
      bool checkVan();
      bool ckeckPrVan();
      bool checkPrPae();
      bool checkPae();
      bool ckeckVanPr(int pr);
  };
#endif
