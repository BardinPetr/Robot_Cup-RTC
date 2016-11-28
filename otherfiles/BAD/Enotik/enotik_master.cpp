#include "Enotik_master.h"
#include "crc8.h"

EnotikMaster Enotik;
EnotikMaster Enotik1;

void serialEvent()
{

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  switch (Enotik.serialid) {
    case 1:
      while (Serial.available())
      {
        Enotik.parser.parse(Serial.read());
      }     break;
    case 2:
      while (Serial.available())
      {
        Enotik.parser.parse(Serial.read());
      }     break;
    case 3:
      while (Serial.available())
      {
        Enotik.parser.parse(Serial.read());
      }     break;
    case 0:
      while (Serial.available())
      {
        Enotik.parser.parse(Serial.read());
      }
      break;
  }
  #else
    while (Serial.available())
      {
        Enotik.parser.parse(Serial.read());
      }
  #endif
}
EnotikMaster::EnotikMaster()
{
}

void EnotikMaster:: bind(byte opcode, OpcodeHandler handle)
{
  if (cur_opcode_count < opcode_handlers_max)
  {
    handlers[cur_opcode_count].handler = handle;
    handlers[cur_opcode_count].opcode = opcode;
    cur_opcode_count++;
  }
}

void EnotikMaster:: on_message_received(byte opcode, byte dataLow, byte dataHigh)
{
  last_opcode = opcode;
  last_dataLow = dataLow;
  last_dataHigh = dataHigh;
  is_answer_received = true;
  last_answer_time = millis();
}

void EnotikMaster:: work()
{
  if (millis() - last_answer_time > time_answer)
  {
    online = false;
  }

  if (is_answer_received == false)
  {
    return;
  }

  for (byte i = 0; i < opcode_handlers_max; i++)
  {
    if (last_opcode == handlers[i].opcode)
    {
      handlers[i].handler(last_dataLow, last_dataHigh);
      online = true;
      is_answer_received = false;
      return;
    }
  }
}

void EnotikMaster:: send_request()
{
  message_data[ENOTIK_ID_OFFSET] = id;
  message_data[ENOTIK_CRC8_OFFSET] = crc8(message_data, ENOTIK_MESSAGE_LENGTH - 1);

  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  switch (serialid) {
    case 1:
      Serial1.write(message_data, ENOTIK_MESSAGE_LENGTH);
      break;
    case 2:
      Serial2.write(message_data, ENOTIK_MESSAGE_LENGTH);
      break;
    case 3:
      Serial3.write(message_data, ENOTIK_MESSAGE_LENGTH);
      break;
    case 0:
      Serial.write(message_data, ENOTIK_MESSAGE_LENGTH);
      break;
  }
  #else
  Serial.write(message_data, ENOTIK_MESSAGE_LENGTH);
  #endif
}

void EnotikMaster:: init_slave(byte slave_id)
{
  parser.init_slave(slave_id);

  id = slave_id | ENOTIK_DIRECTION_MASK_ANSWER;
}

void EnotikMaster:: init_master(byte slave_id)
{
  parser.init_master(slave_id );

  id = slave_id;
}

void EnotikMaster:: send_message(byte opcode, byte dataLow, byte dataHigh)
{
  message_data[ENOTIK_OPPCODE_OFFSET] = opcode;
  message_data[ENOTIK_DATALOW_OFFSET] = dataLow;
  message_data[ENOTIK_DATAHIGH_OFFSET] = dataHigh;

  send_request();
}

void EnotikMaster:: set_answer_time(unsigned int answer_time)
{
  time_answer = answer_time;
}

void EnotikMaster::setSerial(int id) {
  serialid = id;
}
