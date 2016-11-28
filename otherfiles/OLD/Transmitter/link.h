#pragma once
#include <avr/io.h>
#include <arduino.h>

#define START_MARKER 0xFF
#define STOP_MARKER 0xFE
#define DEVICE_ID 0x30


struct APC
{
  byte apc_startMarker = 255  ;
  byte id_device = 48;
  byte apc_stopMarker = 254  ;
  byte check_summ ;
  byte RecieveBuf[7];
  unsigned long lasttime = 0;
  bool online = 0;
};

typedef void (*OpcodeHandler)(byte dataLow, byte dataHigh);
int cur_opcode_count = 0;
struct Item
{
  byte opcode;
  OpcodeHandler handler;
};

Item handlers[10];

extern APC APC1;
extern byte package[3];

void apc_Reset(void);
void apc_SetUp(void);
void apc_Send(int command_marker, int data1_byte, int data2_byte );
void apc_Read(void);
void on_receive(void);
void bind(byte opcode, OpcodeHandler handle);
bool online(void);
