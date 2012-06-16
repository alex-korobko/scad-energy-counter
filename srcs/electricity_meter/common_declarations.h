#ifndef COMMON_DECLARATIONS_H
#define COMMON_DECLARATIONS_H


namespace {
	enum {EOK=0};
	enum {CODE_TIMER=0, CODE_RING, CODE_ALARM};
	enum {ETX = 3, DLE=16};
};

namespace electricity_meter
{
		typedef enum {INFO=0, WARINIG, ERROR} MessType;
		typedef unsigned char byte;
		typedef char modem_character;
		typedef enum {NOTHING=0x0, NET_PACKETS = 0x01, SQL_TO_WRITE =0x02, SQL_TO_READ =0x04, MODEM =0x08,EVERYTHING=0xffff} LoggingOptions;

        enum{ BAUD_9600=B9600,
              BAUD_19200=B19200,
              BAUD_115200=B115200};

        enum { STOP_BITS_1=1,
               STOP_BITS_2,
               DATA_BITS_5=5,
               DATA_BITS_6,
               DATA_BITS_7, 
               DATA_BITS_8,
               PARITY_DISABLE, 
               PARITY_ODD,
               PARITY_EVEN,
               RTS_SET_1,
               RTS_SET_0};

};

#endif
