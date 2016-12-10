#include "catlinkm.h"

long int Clocks = 0, prevClocks = 0, Clocks2 = 0, prevClocks2 = 0;

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


CatLink::CatLink(byte id) {
  id_device = id;
  startMarker = 0xFF;
  stopMarker = 0xFE;
  Reset();
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

  Serial1.write(startMarker);
  Serial1.flush();
  Serial1.write(id_device);
  Serial1.flush();
  Serial1.write(command_markerS);
  Serial1.flush();
  Serial1.write(data1_byteS);
  Serial1.flush();
  Serial1.write(data2_byteS);
  Serial1.flush();
  Serial1.write(stopMarker);
  Serial1.flush();
  check_summ = startMarker + id_device + command_marker + data1_byte + data2_byte + stopMarker;
  Serial1.write(check_summ);
  Serial1.flush();
}

void CatLink::Read(){
  byte CheckSummR = 0;
  byte CheckSummT = 0;
  int a = 0;

  if (millis() - lasttime > 1000)
  {
    online = false;
  }

  a = Serial1.available();

  if (a > 0)
  {
  	      	//digitalWrite(13, 1);

    if ((RecieveBuf[0] = Serial1.read()) == startMarker)
    {
      if ((RecieveBuf[1] = Serial1.read()) == id_device)
      {
        RecieveBuf[2] = Serial1.read();
        RecieveBuf[3] = Serial1.read();
        RecieveBuf[4] = Serial1.read();
        RecieveBuf[5] = Serial1.read();
        RecieveBuf[6] = Serial1.read();

        for (int i = 0; i < 6; i++)
        {
          CheckSummT += RecieveBuf[i];
        }

        //if (CheckSummT == RecieveBuf[6])
        //{
          for (int i = 0; i < 3; i++)
          {
            package[i] = RecieveBuf[i + 2];
          }
          lasttime = millis();
          online = true;

          handlers[package[0]].handler(package[1], package[2]);
        //}
        //else
        //{
        //  Reset();
        //}
      }
    }
  }
  else {
    Reset();
  }
}

void CatLink::on_receive(){

}
