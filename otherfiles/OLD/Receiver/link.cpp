#include <avr/io.h>
#include <arduino.h>
#include "link.h"

APC APC1;
byte package[3];

void apc_Reset()
{
  APC1.check_summ = 0;
  for ( int i = 0; i < 7; i++)
  {
    APC1.RecieveBuf[i] = 0;
  }

  for (int i = 0; i < 3; i++)
  {
    package[i] = 0;
  }

}

void apc_SetUp()
{
  APC1.id_device = 0x30 ;
  APC1.apc_startMarker = 0xFF;
  APC1.apc_stopMarker = 0xFE;
  apc_Reset();
}


void apc_Send(int command_marker, int data1_byte, int data2_byte )
{
  byte command_markerS = command_marker;
  byte data1_byteS = data1_byte;
  byte data2_byteS = data2_byte;

  Serial.write(APC1.apc_startMarker);
  Serial.flush();
  Serial.write(APC1.id_device);
  Serial.flush();
  Serial.write(command_markerS);
  Serial.flush();
  Serial.write(data1_byteS);
  Serial.flush();
  Serial.write(data2_byteS);
  Serial.flush();
  Serial.write(APC1.apc_stopMarker);
  Serial.flush();
  APC1.check_summ = APC1.apc_startMarker + APC1.id_device + command_marker + data1_byte + data2_byte + APC1.apc_stopMarker;
  Serial.write(APC1.check_summ);
  Serial.flush();
}


void apc_Read()
{
  byte CheckSummR = 0;
  byte CheckSummT = 0;
  int a = 0;

  if (millis() - APC1.lasttime > 1000)
  {
    APC1.online = false;
  }

  a = Serial.available();

  if (a > 0)
  {
    if ((APC1.RecieveBuf[0] = Serial.read()) == 0xFF)
    {
      if ((APC1.RecieveBuf[1] = Serial.read()) == APC1.id_device)
      {
        APC1.RecieveBuf[2] = Serial.read();
        APC1.RecieveBuf[3] = Serial.read();
        APC1.RecieveBuf[4] = Serial.read();
        APC1.RecieveBuf[5] = Serial.read();
        APC1.RecieveBuf[6] = Serial.read();

        for (int i = 0; i < 6; i++)
        {
          CheckSummT += APC1.RecieveBuf[i];
        }

        if (  CheckSummT == APC1.RecieveBuf[6] )
        {
          for (int i = 0; i < 3; i++)
          {
            package[i] = APC1.RecieveBuf[i + 2];
          }
          APC1.lasttime = millis();
          APC1.online = true;
        }

        else
        {
          apc_Reset();
        }
      }
    }
  }
  else {
    apc_Reset();
  }
}

bool online() {
  return APC1.online;
}

