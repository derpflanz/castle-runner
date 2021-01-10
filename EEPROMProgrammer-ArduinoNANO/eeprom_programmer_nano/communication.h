#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

// ASCII convenience defines
#define SOH 0x01
#define STX 0x02
#define ETX 0x03
#define EOT 0x04
#define ENQ 0x05
#define ACK 0x06
#define NAK 0x15
#define SYN 0x16
#define ETB 0x17

// Commands
#define RECEIVE_DATA  0x31
#define SEND_DATA     0x32
#define NOTHING       0x30
#define HEXFILE_MAX_SIZE  32768

#define PRINTLINE { Serial.print("[CR-EP1] "); Serial.println(linebuffer); }
extern char linebuffer[];

class Comm {
    public:
        void setup();
        void printf(char *format, ...);
        byte readByte();
        void sendByte(byte b);

    private:

};

extern Comm Communication; 

#endif