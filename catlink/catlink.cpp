#include "catlink.h"
#include <SoftwareSerial.h>
 
long int Clocks = 0, prevClocks = 0, Clocks2 = 0, prevClocks2 = 0;
static HardwareSerial * hser = NULL;
static SoftwareSerial * sser = NULL;
bool sertype = 0;


CatLink::CatLink(byte id, SoftwareSerial &theSerial) {
  sser = &theSerial;
  sser->begin(9600);
  sertype = true;
  id_device = id;
  startMarker = 0xFF;
  stopMarker = 0xFE;
  Reset();
}

CatLink::CatLink(byte id, HardwareSerial &theSerial) {
  hser = &theSerial;
  hser->begin(9600);
  sertype = false;
  id_device = id;
  startMarker = 0xFF;
  stopMarker = 0xFE;
  Reset(); 
}

void CatLink::setOnDisconnect(FuncHandler handler) {
	DisconnectHandler = handler;
}

void CatLink::clearBuffer(){
	if(sertype) while (sser->available()) sser->read();
	else        while (hser->available()) hser->read();
}

bool CatLink::st0(int stime)
{
  bool result;
  Clocks = millis();
  if (Clocks - prevClocks >= stime )
  {
    result = 1;
    prevClocks = Clocks;
  }
  else
    result = 0;

  return result;
}

bool CatLink::st1(int stime)
{
  bool result;
  Clocks2 = millis();
  if (Clocks2 - prevClocks2 >= stime )
  {
    result = 1;
    prevClocks2 = Clocks2;
  }
  else
    result = 0;

  return result;
}

void CatLink::bind(byte opcode, OpcodeHandler handle)
{
  if (cur_opcode_count < opcode_handlers_max)
  {
    handlers[opcode].handler = handle;
    handlers[opcode].opcode = opcode;
    cur_opcode_count++;
  }
}

void CatLink::Reset() {
  check_summ = 0;
  for ( int i = 0; i < 7; i++)
  {
    RecieveBuf[i] = 0;
  }
  for (int i = 0; i < 3; i++)
  {
    package[i] = 0;
  }
}

void CatLink::Send(int command_marker, int data1_byte, int data2_byte ){
  byte command_markerS = command_marker;
  byte data1_byteS = data1_byte;
  byte data2_byteS = data2_byte;

  if(!sertype){
  hser->write(startMarker);
  //hser->flush();
  hser->write(id_device);
  //hser->flush();
  hser->write(command_markerS);
  //hser->flush();
  hser->write(data1_byteS);
  //hser->flush();
  hser->write(data2_byteS);
  //hser->flush();
  check_summ = (int)((startMarker + id_device + command_marker + data1_byte + data2_byte + stopMarker) / 6);
  hser->write(check_summ);
  //hser->flush();
  hser->write(stopMarker);
  //hser->flush();
  } 
  else{
  sser->write(startMarker);
  //sser->flush();
  sser->write(id_device);
  //sser->flush();
  sser->write(command_markerS);
  //sser->flush();
  sser->write(data1_byteS);
  //sser->flush();
  sser->write(data2_byteS);
  //sser->flush();
  check_summ = (int)((startMarker + id_device + command_marker + data1_byte + data2_byte + stopMarker) / 6);
  sser->write(check_summ);
  //sser->flush();
  sser->write(stopMarker);
  //sser->flush();
  }
}

void CatLink::Read(){
  byte CheckSummR = 0;
  byte CheckSummT = 0;
  int a = 0;

  if (millis() - lasttime > 1000)
  {
    online = false;
    DisconnectHandler();
  }

  if(sertype){

  a = sser->available();

  if (a > 0)
  { 
    if ((RecieveBuf[0] = sser->read()) == startMarker)
    {
      if ((RecieveBuf[1] = sser->read()) != id_device)
      {
        RecieveBuf[2] = sser->read();
        RecieveBuf[3] = sser->read();
        RecieveBuf[4] = sser->read();
        RecieveBuf[5] = sser->read();
        RecieveBuf[6] = sser->read();

        for (int i = 0; i < 6; i++)
        {
          CheckSummT += RecieveBuf[i];
        }

        if (CheckSummT == RecieveBuf[6])
        {
          for (int i = 0; i < 3; i++)
          {
            package[i] = RecieveBuf[i + 2];
          }
          lasttime = millis();
          online = true;

          handlers[package[0]].handler(package[1], package[2]);
        }
        else
        {
          Reset();
        }
      }
    }
  }
  else {
    Reset();
  }
}  else{

  a = hser->available();

  if (a > 0)
  {
    if ((RecieveBuf[0] = hser->read()) == startMarker)
    {
      if ((RecieveBuf[1] = hser->read()) == id_device)
      {
        RecieveBuf[2] = hser->read();
        RecieveBuf[3] = hser->read();
        RecieveBuf[4] = hser->read();
        RecieveBuf[5] = hser->read();
        RecieveBuf[6] = hser->read();

        for (int i = 0; i < 6; i++)
        {
          CheckSummT += RecieveBuf[i];
        }

        if (CheckSummT == RecieveBuf[6])
        {
          for (int i = 0; i < 3; i++)
          {
            package[i] = RecieveBuf[i + 2];
          }
          lasttime = millis();
          online = true;

          handlers[package[0]].handler(package[1], package[2]);
        }
        else
        {
          Reset();
        }
      }
    }
  }
  else {
    Reset();
  }
}
clearBuffer();
}

void CatLink::parseinput(){
  if(!sertype){ //hard
    while (hser->available()) {
      byte inChar = (byte)hser->read();
      datain[rnum] = inChar;

      if (inChar == stopMarker && rnum > 5) {
        inComplete = true;
        rnum = 0;
      }
      else
        rnum++;
    }
  }
}

void CatLink::parseinput_s(){
  if(sertype){ //soft
    while (sser->available()) {
      byte inChar = (byte)sser->read();
      datain[rnum] = inChar;

      if (inChar == stopMarker && rnum > 5) {
        inComplete = true;
        rnum = 0;
      }
      else
        rnum++;
    }
  }
}

void CatLink::Run(){
  if (millis() - lasttime > 1000)
  {
    online = false;
    DisconnectHandler();
  }

  if (inComplete){
    if (datain[0] == startMarker && datain[6] == stopMarker && datain[1] == id_device)
    {
      for (int i = 0; i < 3; i++)
      {
        package[i] = datain[i + 2];
      }

      lasttime = millis();
      online = true;
      inComplete = false;
      
      handlers[package[0]].handler(package[1], package[2]);
    }
  }

}
