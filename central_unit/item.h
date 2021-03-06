//extern const int NUMBER_OBJ;
//extern const int NUMBER_MAX;
extern int numberObj[6];        // nombre des objets de systeme
extern int settingSMS[4];        // nombre des objets de systeme
extern int objState[5][15];        // les etates des objets
extern int sector[6][6];        // les secteurs 
extern int relationObj[6][6]; // les relation entre les pompes de refoulements et les vannes
extern int relationPae[5][2];
extern void sendCmd(int cmd);
extern void popupMessage(String msg,int Id);
extern void addHist(String hist);
extern void successMessage(int retCode);
extern void successMessage();
extern void funValve(int van,int action);
extern void sendStateApp(int key,int value);
int const AD_PROG_PIM=500;
int const AD_PROG_PR=1100;
int const AD_PROG_VAN=1450;
int const AD_PROG_MLG=2300;
int const AD_PROG_ENG=2650; // to 3000
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
      void setProg(int sHr,int sMin,int eHr,int eMin,int Type,int Id);
      void getProg();
      void updateProg();
      int MatrixTime[5][5];
      //void static setTime(int Hr,int Min,int Sec);
    private :
      int IdObj,NumberObj,Cmd;
      bool ckeckPmp();
      bool checkVan();
      bool ckeckPrVan();
      bool checkPrPae();
      bool checkPae();
      bool ckeckVanPr(int pr);
  };
#endif
