#include "Arduino.h"
#include "SIM800L.h"
#include <SoftwareSerial.h>

void SIM800L::begin(){
  Serial3.begin(9600);
  #if (LED) 
    pinMode(OUTPUT,LED_PIN);
  #endif
  _buffer.reserve(255); //reserve memory to prevent intern fragmention
}


//
//PRIVATE METHODS
//
String SIM800L::_readSerial(){
  _timeout=0;
  while  (!Serial3.available() && _timeout < 12000  ) 
  {
    delay(13);
    _timeout++;


  }
  if (Serial3.available()) {
  return Serial3.readString();
  }
  

}


//
//PUBLIC METHODS
//





int SIM800L::signalQuality(){
  String str="";
  int Signal=0;
  Serial3.print (F("AT+CSQ\r\n"));
  str=_readSerial();
  str=str.substring(16,18);
  str.trim();
  Signal=str.toInt();
  return Signal;
}










void  SIM800L::callNumber(char* number){
  Serial3.print (F("ATD"));
  Serial3.print (number);
  Serial3.print(F(";\r\n"));
}



uint8_t SIM800L::getCallStatus(){
/*
  values of return:
 
 0 Ready (MT allows commands from TA/TE)
 2 Unknown (MT is not guaranteed to respond to tructions)
 3 Ringing (MT is ready for commands from TA/TE, but the ringer is active)
 4 Call in progress

*/
  Serial3.print (F("AT+CPAS\r\n"));
  _buffer=_readSerial();  
  return _buffer.substring(_buffer.indexOf("+CPAS: ")+7,_buffer.indexOf("+CPAS: ")+9).toInt();

}









bool SIM800L::sendSms(char* number,char* text){

    Serial3.print (F("AT+CMGF=1\r")); //set sms to text mode  
    _buffer=_readSerial();
    Serial3.print (F("AT+CMGS=\""));  // command to send sms
    Serial3.print (number);           
    Serial3.print(F("\"\r"));       
    _buffer=_readSerial(); 
    Serial3.print (text);
    Serial3.print ("\r"); 
  //change delay 100 to readserial  
    _buffer=_readSerial();
    Serial3.print((char)26);
    _buffer=_readSerial();
    //expect CMGS:xxx   , where xxx is a number,for the sending sms.
    if (((_buffer.indexOf("CMGS") ) != -1 ) ){
      return true;
    }
    else {
      return false;
    }
}


String SIM800L::getNumberSms(uint8_t index){
  _buffer=readSms(index);
  if (_buffer.length() > 10) //avoid empty sms
  {
    uint8_t _idx1=_buffer.indexOf("+CMGR:");
    _idx1=_buffer.indexOf("\",\"",_idx1+1);
    return _buffer.substring(_idx1+3,_buffer.indexOf("\",\"",_idx1+4));
  }else{
    return "";
  }
}



String SIM800L::readSms(uint8_t index){
  String msg="";
  Serial3.print (F("AT+CMGF=1\r")); 
  if (( _readSerial().indexOf("ER")) ==-1) {
    Serial3.print (F("AT+CMGR="));
    Serial3.print (index);
    Serial3.print("\r");
    _buffer=_readSerial();
    if (_buffer.indexOf("CMGR:")!=-1){
      return _buffer;
    }
    else return "";    
    }
  else
    return "ERR";
}


bool SIM800L::delAllSms(){ 
  Serial3.print(F("AT+CMGD=1,4\n\r"));
  _buffer=_readSerial();
  if (_buffer.indexOf("OK")!=-1) {return true;}else {return false;}
  
}
