#ifndef COMMON_DECLARATIONS_H
#define COMMON_DECLARATIONS_H

namespace electricity_meter
{
		typedef enum {INFO=0, WARINIG, ERROR} MessType;
		typedef unsigned char byte;
		typedef char modem_character;
		typedef enum {NOTHING=0x0, NET_PACKETS = 0x01, SQL_TO_WRITE =0x02, SQL_TO_READ =0x04, EVERYTHING=0xffff} LoggingOptions;
}

#endif
