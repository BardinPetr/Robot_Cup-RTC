#pragma once

#define START_MARKER 0xFF
#define STOP_MARKER 0xFE
#define DEVICE_ID 0x30

#include <avr/io.h>
#include <arduino.h>

class CatLink {
  public:
    CatLink(byte id);

    static const byte opcode_handlers_max = 30;
    typedef void (*OpcodeHandler)(byte dataLow, byte dataHigh);         
    
    void Reset(void);
    void Send(int command_marker, int data1_byte, int data2_byte );
    void Read(void);
    void bind(byte opcode, OpcodeHandler handler);

    bool online = 0;
    byte package[3];
    
  private:
    void on_receive(void);
    byte startMarker = 255  ;
    byte id_device = 48;
    byte stopMarker = 254  ;
    byte check_summ ;
    byte RecieveBuf[7];
    unsigned long lasttime = 0;
    byte cur_opcode_count=0;

    struct Item                                                         
    {
      byte opcode;
      OpcodeHandler handler;
    };

    Item handlers[opcode_handlers_max];    
};
 