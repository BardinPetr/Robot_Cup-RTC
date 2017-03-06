#pragma once

#define START_MARKER 0xFF
#define STOP_MARKER 0xFE
#define DEVICE_ID 0x30
  
#include <avr/io.h>
#include <arduino.h>
#include <SoftwareSerial.h>

class CatLink {
  public:
    CatLink(byte id, SoftwareSerial &theSerial);
    CatLink(byte id, HardwareSerial &theSerial);
    
    static const byte opcode_handlers_max = 30;
    typedef void (*OpcodeHandler)(byte dataLow, byte dataHigh);         
	  typedef void (*FuncHandler)();         
    
    void Reset(void);
    void Send(int command_marker, int data1_byte, int data2_byte );
    void Read(void);
    void bind(byte opcode, OpcodeHandler handler);

    void Run();
    
    bool st0(int);
    bool st1(int);

    void parseinput();
    void parseinput_s();

	  void setOnDisconnect(FuncHandler handler);
	  void clearBuffer();
	
    bool online = 0;
    byte package[3];
    
  private:
    int rnum = 0;

    //data packet: startMarker deviceId cmdId data1 data2 stopMarker checkSum
    byte startMarker = 255  ;
    byte id_device = 48;
    byte stopMarker = 254  ;
    byte check_summ ;
    byte RecieveBuf[7];

    byte datain[7];
    bool inComplete = false;

    unsigned long lasttime = 0;
    byte cur_opcode_count=0;

    struct Item                                                         
    {
      byte opcode;
      OpcodeHandler handler;
    };

    Item handlers[opcode_handlers_max];  
	  FuncHandler DisconnectHandler;
};
 
