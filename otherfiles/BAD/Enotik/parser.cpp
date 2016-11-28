#include "parser.h"
#include "crc8.h"
#include "Enotik_master.h"
extern EnotikMaster Enotik;
extern EnotikMaster Enotik1;

void Parser:: parse(byte data)
{
  if(count_data==ENOTIK_ID_OFFSET)
  {
    if(data==id)
    {
      message[ENOTIK_ID_OFFSET]=id;
      count_data++;
    }
  }
  else if(count_data>ENOTIK_ID_OFFSET && count_data<ENOTIK_MESSAGE_LENGTH-1)
  {
    message[count_data]=data;
    count_data++;
  }
  else 
  { 
    message[count_data]=data;

    if(crc8(message,ENOTIK_MESSAGE_LENGTH-1)==message[ENOTIK_CRC8_OFFSET])
    {
      Enotik.on_message_received (message[ENOTIK_OPPCODE_OFFSET],message[ENOTIK_DATALOW_OFFSET],message[ENOTIK_DATAHIGH_OFFSET]);
      count_data=0;
    }
    else
    {
      for(byte i=1;i<ENOTIK_MESSAGE_LENGTH;i++)            
      {
        if(message[i]==id)
        {
          count_data=ENOTIK_MESSAGE_LENGTH-i;
          shiftLeft(i);
          return;
        }
      }
      count_data=0;
    }
  }
}

void Parser::shiftLeft(byte amount)
{
  for(byte i=0, j=amount; i<ENOTIK_MESSAGE_LENGTH-amount; i++, j++)
  {
    message[i] = message[j];  
  }    
}

void Parser::init_master(byte id_master)
{
  id=id_master | 1<<7;
  count_data=0;  
}

void Parser::init_slave(byte id_slave)
{
  id=id_slave;
  count_data=0;  
}

