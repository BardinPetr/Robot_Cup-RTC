#pragma once
#include <avr/io.h>
#include <arduino.h>
#include "Enotik_message_format.h"
#include "parser.h"

class EnotikMaster
{
  friend void serialEvent();
  
  public:
   //EnotikMaster();
    static const byte opcode_handlers_max = 30;
    typedef void (*OpcodeHandler)(byte dataLow, byte dataHigh);         
    
    void bind(byte opcode, OpcodeHandler handler);                              
    void on_message_received(byte opcode,byte dataLow,byte dataHigh);    
    void work();
    void init_slave(byte slave_id);
    void init_master(byte slave_id);
    void send_message(byte opcode,byte dataLow,byte dataHigh);
    void set_answer_time(unsigned int answer_time);
  	
  	void setSerial(int id);
  	
  	bool online = false;
  private:  	
    unsigned int time_answer=1000;
    byte message_data[ENOTIK_MESSAGE_LENGTH];
    void send_request();
    byte id=0;
    byte last_opcode=0;
    byte last_dataLow=0;
    byte last_dataHigh=0;
    unsigned long int last_answer_time=0; 
    bool is_answer_received=false;
    byte cur_opcode_count=0;

    Parser parser;    

    struct Item                                                         
    {
      byte opcode;
      OpcodeHandler handler;
    };

    Item handlers[opcode_handlers_max];                              
    
    int serialid = 0;
};

extern EnotikMaster Enotik;
extern EnotikMaster Enotik1;


