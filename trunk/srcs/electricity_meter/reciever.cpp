#include <string>
#include <vector>
#include <map>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdexcept>
#include <memory>
#include <inttypes.h>
#include <pthread.h>
#include <string.h>
#include <sstream>
#include <mysql.h>
#include <errno.h>

#include "common_declarations.h"
#include "exception.h"
#include "configurator.h"
#include "logger.h"
#include "reciever.h"
#include "mysql_writer_factory.h"
#include "my_sql_writer.h"

#include "socket_exception.h"
#include "generic_socket.h"
#include "server_socket.h"

namespace {
	enum {EOK=0};
	enum {CODE_TIMER=0, CODE_RING, CODE_ALARM};
	enum {ETX = 3, DLE=16};
};

namespace electricity_meter
{


//-------------------------------------------------------------------
// Table of CRC values for high-order byte
static uint8_t auchCRCHi[] =
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;
// Table of CRC values for low-order byte
static uint8_t auchCRCLo[] =
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;

	
reciever::reciever():
mStopRecieveFlag(false),
mSocket(NULL)
{
}

reciever::~reciever()
{
}

reciever::reciever(const reciever& rhs)
{
}

reciever& reciever::operator=(const reciever& rhs)
{
	return *this;
}

reciever& reciever::Instance()
{
	static reciever recInst;
	return recInst;
}


static uint16_t CRC16(const byte* puchMsg, unsigned int usDataLen)
{
unsigned char uchCRCHi = 0xFF ; /* high byte of CRC initialized */
unsigned char uchCRCLo = 0xFF ; /* low byte of CRC initialized */
unsigned uIndex=0 ; /* will index into CRC lookup table */


   /* pass through message buffer */
   while (usDataLen--)
   {
      uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
      uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
      uchCRCLo = auchCRCLo[uIndex] ;
   }
return (uint16_t(uchCRCHi << 8) | uchCRCLo);
}


static bool recieve_data_packet(generic_socket* Socket, std::vector<byte>& PacketBuffer)
{
	const uint8_t ContinueDataTransferLen = 3;
	uint16_t PacketDataLength, OrigPacketCRC, PacketCRC;
	
	std::vector<byte> TmpBuffer;
	PacketBuffer.clear();

	TmpBuffer.clear();
	try {
		Socket->recv(TmpBuffer, ContinueDataTransferLen);
	} catch (socket_exception exc) 
	{
		logger::Instance().LogMessage(exc);
		delete (Socket);
		return false;		
	};
	
	PacketDataLength = (static_cast<uint16_t>(TmpBuffer[1])<<8) + TmpBuffer[2];
	PacketBuffer.swap(TmpBuffer);

	PacketDataLength = PacketDataLength - ContinueDataTransferLen;
	
	if (PacketDataLength<0)
	{
		logger::Instance().LogMessage(ERROR, "recieve_data_packet : PacketDataLength < 0");
		delete (Socket);
		return false;		
	};

	TmpBuffer.clear();
	try {
		Socket->recv(TmpBuffer, PacketDataLength);
	} catch (socket_exception exc) 
	{
		logger::Instance().LogMessage(exc);
		delete (Socket);
		return false;		
	};

	PacketBuffer.insert(PacketBuffer.end(), TmpBuffer.begin(), TmpBuffer.end());
	
	if (PacketBuffer.size()<=2)
	{
		logger::Instance().LogMessage(ERROR, "recieve_data_packet:can't check CRC-packet data length <=2");
		delete (Socket);
		return false;				
	};
		
	OrigPacketCRC = (static_cast<uint16_t>(PacketBuffer[PacketBuffer.size()-2])<<8) + PacketBuffer[PacketBuffer.size()-1];
	PacketCRC = CRC16(&PacketBuffer[0], PacketBuffer.size()-2);

	if (OrigPacketCRC != PacketCRC)
	{
		logger::Instance().LogMessage(ERROR, "recieve_data_packet:bad CRC");
		delete (Socket);
		return false;				
	};

	return true;
};


bool ReceiveEnergy (const std::vector<byte>& BufPacket)
{
const std::string FuncName = "ReceiveEnergy"; 
//bytes count for an abonent
const uint16_t NRec = 11;

configurator&  conf =  configurator::Instance();
int i=16, Rele, Abonent, Year, Month, Day, Hour, Minute, Second, E1, E2, E3;

 if (conf.GetLoggingOptions() & NET_PACKETS)
 {
	std::ostringstream strMess;
	strMess <<FuncName.c_str()<< " the packet is ("<< pthread_self()<<")";
	strMess << std::endl;
	strMess << std::hex;
	for (std::size_t i=0; i< BufPacket.size(); i++)
	{
		strMess <<"0x"<<static_cast<int>(BufPacket[i]);
		strMess << " ";
		if (i>0 && (i%5 ==0) )
		{
			strMess <<	std::endl;
		};
	};
	
	logger::Instance().LogMessage(INFO, strMess.str());			
}

if (BufPacket.size() <=i)
{
	std::ostringstream strMess;
	strMess <<FuncName.c_str()<< " the packet is too short! ("<< pthread_self()<<")";
	strMess << std::endl;
	strMess << std::hex;
	for (std::size_t i=0; i< BufPacket.size(); i++)
	{
		strMess << static_cast<int>(BufPacket[i]);
		strMess << " ";
		if (i>0 && (i%5 ==0) )
		{
			strMess <<	std::endl;
		};
	};
	
	logger::Instance().LogMessage(ERROR, strMess.str());			
};

//decoding rele date
  Year = 0;
  Year = (BufPacket[5]<<8) | BufPacket[6];
  if ((Year < 1800) ||(Year > 2050))
  {
	std::ostringstream strMess;
	strMess <<FuncName.c_str()<< " ("<< pthread_self()<<") ";
	strMess << "wrong Year value in packet: " <<Year;
	logger::Instance().LogMessage(ERROR, strMess.str());
	return false;
  }

 Month = BufPacket[7];
 if ((Month < 1) ||(Month > 12))
 {
	std::ostringstream strMess;
	strMess <<FuncName.c_str()<< " ("<< pthread_self()<<") ";
	strMess << "wrong Month value in packet: " <<Month;
	logger::Instance().LogMessage(ERROR, strMess.str());			
	return false;
 }

 Day = BufPacket[8];
if ((Day < 1) ||(Day > 31))
 {
	std::ostringstream strMess;
	strMess <<FuncName.c_str()<< " ("<< pthread_self()<<") ";
	strMess << "wrong Day value in packet: " <<Day;
	logger::Instance().LogMessage(ERROR, strMess.str());			
	return false;
 }

 Hour = BufPacket[9];
 if ((Hour<0) ||(Hour > 23))
 {
	std::ostringstream strMess;
	strMess <<FuncName.c_str()<< " ("<< pthread_self()<<") ";
	strMess << "wrong Hour value in packet: "<<Hour;
	logger::Instance().LogMessage(ERROR, strMess.str());			
	return false;
 }

 Minute = BufPacket[10];
 if ((Minute < 0) || (Minute > 59))
 {
	std::ostringstream strMess;
	strMess <<FuncName.c_str()<< " ("<< pthread_self()<<") ";
	strMess << "wrong Minute value in packet: "<<Minute;
	logger::Instance().LogMessage(ERROR, strMess.str());			
	return false;
 }
Second = 0;


// decoding each record
while(i<(BufPacket.size()-2))
{
	// creating new unique abonent id using controller number, Modbus number & channel
	Abonent = 0;
	Abonent =   (BufPacket[3]<<24) |
			   (BufPacket[4]<<16) |
			   ((BufPacket[i+0]&0x7F)<<8) |
				BufPacket[i+1];

	if(Abonent < 0)
	  {
		std::ostringstream strMess;
		strMess <<FuncName.c_str()<< " ("<< pthread_self()<<") ";
		strMess << "wrong Abonent value in packet: "<<Abonent<<" BufPacket[3] "<< static_cast<int>(BufPacket[3])
				<<" BufPacket[4] "<<static_cast<int>(BufPacket[4])<< " BufPacket[i+0] " << static_cast<int>(BufPacket[i+0]) 
				<<" BufPacket[i+1] "<< static_cast<int>(BufPacket[i+1]);
		logger::Instance().LogMessage(ERROR, strMess.str());			

		i+=NRec;
		continue;
	  };

	  Rele = BufPacket[i+0] & 0x80;

	  E1 = E2 = E3 = 0;

	  E1 = (BufPacket[i+2]<<16) |
		   (BufPacket[i+3]<<8) |
		   (BufPacket[i+4]);

	  E2 = (BufPacket[i+5]<<16) |
		   (BufPacket[i+6]<<8) |
			BufPacket[i+7];
			 
	  E3 = (BufPacket[i+8]<<16) |
		   (BufPacket[i+9]<<8) |
			BufPacket[i+10];

	 if((E1<0) || (E2<0) || (E3 < 0))
	 {
			std::ostringstream strMess;
			strMess <<FuncName.c_str()<< " ("<< pthread_self()<<") "
					<< "wrong energy value in packet: E1 "<<E1<<" E2 "<< E2
					<<" E3 "<<E3 << " Summary " << (E1+E2+E3);
			logger::Instance().LogMessage(ERROR, strMess.str());			

			i+=NRec;
			continue;
	 }

//Write to database
my_sql_writer* mysql_wrtr = NULL;
try
  {

	mysql_writer_factory& mysql_fact = mysql_writer_factory::Instance();

		try
		  {
			mysql_wrtr = mysql_fact.GetThreadMySQLWriter();
			} catch (exception exc)
			{
				logger::Instance().LogMessage(exc);
				i+=NRec;
				continue;		
			};

		mysql_wrtr->ExecuteSQL("BEGIN");

		std::string strSQLFull = "insert into e10 ";
		std::ostringstream strSQL;
		strSQL<<"(Abonent, DateRec, StatusReleOld, DateRele, E1, E2, E3, Summary) values ("
				<<Abonent<<", NOW(), ";
		if (Rele)
		{
		 strSQL	<< "1,";
		} else {
		 strSQL	<< "0,";		
		};
		strSQL<<"\'"<<Year<<"-"<<Month<<"-"<<Day<<" "<<Hour<<":"<<Minute<<":"<<Second<<"\',"
				<<E1<<","<<E2<<","<<E3<<","<<(E1+E2+E3)<<")";


		strSQLFull +=strSQL.str();
		if (conf.GetLoggingOptions() & SQL_TO_WRITE)
		  {
			std::ostringstream strMess;
			strMess<<FuncName.c_str()<< " ("<< pthread_self()<<") sql "
					<< strSQLFull.c_str();

			logger::Instance().LogMessage(INFO, strMess.str());			
		  };	
		mysql_wrtr->ExecuteSQL(strSQLFull);

		if((Day == 1) && (Hour == 0) && (Minute < 30))
		{
		  try
		  {
				strSQLFull = "insert into enmonitor ";
				strSQLFull +=strSQL.str();
				if (conf.GetLoggingOptions() & SQL_TO_WRITE)
				  {
					std::ostringstream strMess;
					strMess<<FuncName.c_str()<< " ("<< pthread_self()<<") sql "
							<< strSQLFull;

					logger::Instance().LogMessage(INFO, strMess.str());			
				  };
				mysql_wrtr->ExecuteSQL(strSQLFull);
			} catch (exception exc)
			{
				logger::Instance().LogMessage(exc);
			};
		};

		strSQL.str("");
		strSQL.clear();
		strSQL<<"update a2 set E1="<<E1<<", E2="<<E2<<", E3="<<E3<<", Summary="<<(E1+E2+E3)<<", StatusReleOld=";
		if (Rele)
		{
		 strSQL	<< "1,";
		} else {
		 strSQL	<< "0,";		
		};
		strSQL<<" DateRele=\'"<<Year<<"-"<<Month<<"-"<<Day<<" "<<Hour<<":"<<Minute<<":"<<Second<<"\' where Abonent="<<Abonent;
		if (conf.GetLoggingOptions() & SQL_TO_WRITE)
		  {
			std::ostringstream strMess;
			strMess<<FuncName.c_str()<< " ("<< pthread_self()<<") sql "
					<< strSQL.str();
			logger::Instance().LogMessage(INFO, strMess.str());			
		  };	
		mysql_wrtr->ExecuteSQL(strSQL.str());
		unsigned long affectedRows = mysql_wrtr->NumRows();
		if (affectedRows !=1)
		{
			std::ostringstream strErr;
			strErr<<FuncName.c_str()<< " ("<< pthread_self()<<") sql "
					<< strSQL.str()<< " ; affected " << affectedRows <<" rows.";
			logger::Instance().LogMessage(ERROR, strErr.str());
		};

		mysql_wrtr->ExecuteSQL("COMMIT");
	} catch (exception exc)
	{
		mysql_wrtr->ExecuteSQL("ROLLBACK");
		logger::Instance().LogMessage(exc);
		i+=NRec;
		continue;		
	};
	 i+=NRec;

	};//while

	return true;
};

bool ReceivePower (const std::vector<byte>& BufPacket)
{
const std::string FuncName = "ReceivePower"; 
const uint PacketLength = 105;
uint32_t nByte = BufPacket.size(), BS,abonent,power;
uint8_t StatusPotreb, StatusPodkl, StatusRele;
uint currPowerIndex;

my_sql_writer* mysql_wrtr = NULL;
std::ostringstream strSQL;
configurator&  conf =  configurator::Instance();

 if (conf.GetLoggingOptions() & NET_PACKETS)
 {
	std::ostringstream strMess;
	strMess << FuncName.c_str() <<" the packet is ("<< pthread_self()<<")";
	strMess << std::endl;
	strMess << std::hex;
	for (std::size_t i=0; i< BufPacket.size(); i++)
	{
		strMess <<"0x"<<static_cast<int>(BufPacket[i]);
		strMess << " ";
		if (i>0 && (i%5 ==0) )
		{
			strMess <<	std::endl;
		};
	};
	
	logger::Instance().LogMessage(INFO, strMess.str());			
}

try
  {
	mysql_writer_factory& mysql_fact = mysql_writer_factory::Instance();
	mysql_wrtr = mysql_fact.GetThreadMySQLWriter();

} catch (exception exc)
{
	std::ostringstream strErr;
	strErr<< FuncName.c_str()<<" ("<< pthread_self()<<") GetThreadMySQLWriter returned "<< exc.what();
	throw exception(strErr.str(), ERROR );			
};

if (nByte < PacketLength)
{
   std::ostringstream strErr;
   strErr<< FuncName.c_str()<<" ("<< pthread_self()<<") BufPacket size must be =="<<PacketLength<<" but "<< nByte;
   throw exception(strErr.str(), ERROR );	
};

BS =   (BufPacket[3]<<8) | BufPacket[4];
abonent  = (BS << 16) | ((BufPacket[5] & 0x03)<<8) | (BufPacket[6]);

StatusPotreb =  (BufPacket[5] & 0x60) >> 5;
StatusPodkl =  (BufPacket[5] & 0x18) >> 3;
StatusRele = (BufPacket[5] & 0x80) >> 7;

currPowerIndex = 7;

while (currPowerIndex <= BufPacket - 4)  // 4 = 2 byte length for power + 2 byte for CRC 
{
  power 	= (BufPacket[currPowerIndex]<<8) |
			  (BufPacket[currPowerIndex+1]);


  try
  {
	strSQL.str("");
	strSQL.clear();
	strSQL<<"insert into power (BS, Users, StatusPotreb, StatusPodkl, StatusRele, Power) values ("<<
			BS<<","<<abonent<<","<<static_cast<uint32_t>(StatusPotreb) <<","
			<<static_cast<uint32_t>(StatusPodkl )<<","<<static_cast<uint32_t>(StatusRele)<<","<< power <<");";
	if (conf.GetLoggingOptions() & SQL_TO_WRITE)
	  {
		std::ostringstream strMess;
		strMess<< FuncName.c_str() << " ("<< pthread_self()<<") sql "
				<< strSQL.str();
		logger::Instance().LogMessage(INFO, strMess.str());			
	  };	
	mysql_wrtr->ExecuteSQL(strSQL.str());
	unsigned long affectedRows = mysql_wrtr->NumRows();
	if (affectedRows !=1)
	{
		std::ostringstream strErr;
		strErr<< FuncName.c_str() << " ("<< pthread_self()<<") sql "
				<< strSQL.str()<< " ; affected " << affectedRows <<" rows.";
		logger::Instance().LogMessage(ERROR, strErr.str());
	};


	mysql_wrtr->ExecuteSQL("COMMIT");
  } catch (exception exc)
  {
	mysql_wrtr->ExecuteSQL("ROLLBACK");
	logger::Instance().LogMessage(exc);
  };
  
  currPowerIndex +=2;
};
	
return true;

};

bool ReceiveAlarm (const std::vector<byte>& BufPacket)
{
const int NREC=8;
	
uint16_t BS;
uint8_t nRec, CodeAlarm, ParamAlarm, Month, Day, Hour, Minute, Second=0;
uint32_t nByte = BufPacket.size(), i, Year;

my_sql_writer* mysql_wrtr = NULL;
std::ostringstream strSQL;
configurator&  conf =  configurator::Instance();

 if (conf.GetLoggingOptions() & NET_PACKETS)
 {
	std::ostringstream strMess;
	strMess << "ReceiveAlarm the packet is ("<< pthread_self()<<")";
	strMess << std::endl;
	strMess << std::hex;
	for (std::size_t i=0; i< BufPacket.size(); i++)
	{
		strMess <<"0x"<<static_cast<int>(BufPacket[i]);
		strMess << " ";
		if (i>0 && (i%5 ==0) )
		{
			strMess <<	std::endl;
		};
	};
	
	logger::Instance().LogMessage(INFO, strMess.str());			
}

if (nByte < 8) //6 data bytes + crc16
{
   std::ostringstream strErr;
   strErr<< "ReceiveAlarm  ("<< pthread_self()<<") BufPacket size must be >=8 but "<< nByte;
   throw exception(strErr.str(), ERROR );	
};

BS =   (BufPacket[3]<<8) | BufPacket[4];
nRec = BufPacket[5];
if(nRec<=0)
{
   std::ostringstream strErr;
   strErr<< "ReceiveAlarm  ("<< pthread_self()<<") Count of alarms is "<< nRec;
   logger::Instance().LogMessage(INFO, strErr.str());
   return true;	
};

try
  {
	mysql_writer_factory& mysql_fact = mysql_writer_factory::Instance();
	mysql_wrtr = mysql_fact.GetThreadMySQLWriter();

} catch (exception exc)
{
	std::ostringstream strErr;
	strErr<< "ReceiveAlarm ("<< pthread_self()<<") GetThreadMySQLWriter returned "<< exc.what();
	throw exception(strErr.str(), ERROR );			
};

i = 6;
while(i<(nByte-2))
{
  CodeAlarm = BufPacket[i];
  ParamAlarm = BufPacket[i+1];

  Year = 0;
  Year = ((uint32_t)BufPacket[i+2]<<8) | BufPacket[i+3];
  if ((Year < 1800) ||(Year > 2150))
  {
	std::ostringstream strErr;
	strErr<< "ReceiveAlarm  ("<< pthread_self()<<") CodeAlarm "<<static_cast<uint32_t>(CodeAlarm)<<" ParamAlarm "<<static_cast<uint32_t>(ParamAlarm)<<" Year is "<< Year;
	logger::Instance().LogMessage(ERROR, strErr.str());
	i+=NREC;
	continue;
  };

   Month = BufPacket[i+4];
   if ((Month < 1) ||(Month > 12))
   {
	std::ostringstream strErr;
	strErr<< "ReceiveAlarm  ("<< pthread_self()<<") CodeAlarm "<<static_cast<uint32_t>(CodeAlarm)<<" ParamAlarm "<<static_cast<uint32_t>(ParamAlarm)<<" Month is "<< static_cast<uint32_t>(Month);
	logger::Instance().LogMessage(ERROR, strErr.str());
	i+=NREC;
	continue;
   };

   Day = BufPacket[i+5];
   if ((Day < 1) ||(Day > 31))
   {
	std::ostringstream strErr;
	strErr<< "ReceiveAlarm  ("<< pthread_self()<<") CodeAlarm "<<static_cast<uint32_t>(CodeAlarm)<<" ParamAlarm "<<static_cast<uint32_t>(ParamAlarm)<<" Month is "<< static_cast<uint32_t>(Month);
	logger::Instance().LogMessage(ERROR, strErr.str());
	i+=NREC;
	continue;
   };

   Hour = BufPacket[i+6];
   if ((Hour<0)||(Hour > 23))
   {
	std::ostringstream strErr;
	strErr<< "ReceiveAlarm  ("<< pthread_self()<<") CodeAlarm "<<static_cast<uint32_t>(CodeAlarm)<<" ParamAlarm "<<static_cast<uint32_t>(ParamAlarm)<<" Hour is "<< static_cast<uint32_t>(Hour);
	logger::Instance().LogMessage(ERROR, strErr.str());
	i+=NREC;
	continue;
   }

	Minute = BufPacket[i+7];
	if ((Minute < 0) ||(Minute > 59))
   {
	std::ostringstream strErr;
	strErr<< "ReceiveAlarm  ("<< pthread_self()<<") CodeAlarm "<<static_cast<uint32_t>(CodeAlarm)<<" ParamAlarm "<<static_cast<uint32_t>(ParamAlarm)<<" Minute is "<< static_cast<uint32_t>(Minute);
	logger::Instance().LogMessage(ERROR, strErr.str());
	i+=NREC;
	continue;
   }

try
  {
	strSQL.str("");
	strSQL.clear();
	strSQL<<"insert into alarm (BS, Code, Param, DAlarm, LoginOperator) values ("<<BS<<","<<static_cast<uint32_t>(CodeAlarm)<<","<<static_cast<uint32_t>(ParamAlarm)
		  <<",'"<<Year<<"-"<<static_cast<uint32_t>(Month)<<"-"<<static_cast<uint32_t>(Day)<<" "<<static_cast<uint32_t>(Hour)<<":"<<static_cast<uint32_t>(Minute)<<":"<<static_cast<uint32_t>(Second)<<"', '.');";
	if (conf.GetLoggingOptions() & SQL_TO_WRITE)
	  {
		std::ostringstream strMess;
		strMess<< "ReceiveAlarm ("<< pthread_self()<<") sql "
				<< strSQL.str();
		logger::Instance().LogMessage(INFO, strMess.str());			
	  };	
	mysql_wrtr->ExecuteSQL(strSQL.str());
	unsigned long affectedRows = mysql_wrtr->NumRows();
	if (affectedRows !=1)
	{
		std::ostringstream strErr;
		strErr<< "ReceiveAlarm ("<< pthread_self()<<") sql "
				<< strSQL.str()<< " ; affected " << affectedRows <<" rows.";
		logger::Instance().LogMessage(ERROR, strErr.str());
	};


	mysql_wrtr->ExecuteSQL("COMMIT");
  } catch (exception exc)
  {
	mysql_wrtr->ExecuteSQL("ROLLBACK");
	logger::Instance().LogMessage(exc);
  };


	i+=NREC;
}; //while(i<(nByte-2))
	
return true;
};

bool SendToClientMonitoring(uint16_t ClientInPacket, generic_socket* Socket)
{
const uint16_t Length = 9;
const string  FunctionName = "SendToClientMonitoring";
int Abonent, IntervalPoint, CountPoint;

std::vector<byte> RetBuff(Length, static_cast<byte> (0));
MYSQL_ROW row;
my_sql_writer* mysql_wrtr = NULL;
configurator&  conf =  configurator::Instance();

try
  {
	mysql_writer_factory& mysql_fact = mysql_writer_factory::Instance();

		try
		  {
			mysql_wrtr = mysql_fact.GetThreadMySQLWriter();
			} catch (exception exc)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientMonitoring ("<< pthread_self()<<") GetThreadMySQLWriter returned "<< exc.what();
				throw exception(strErr.str(), ERROR );			
			};

		mysql_wrtr->ExecuteSQL("select Abonent from monitor limit 0,1;"); // TODO - which row is necessary? Does it have only one?
		if (mysql_wrtr->NumRows()!= 1)
		{
			std::ostringstream strErr;
			strErr<<FunctionName.c_str() << " ("<< pthread_self()<<") query to monitor returned "<< mysql_wrtr->NumRows()<<" rows";
			throw exception(strErr.str(), ERROR );			
		};
		MYSQL_ROW row  = mysql_wrtr->FetchRow();
	
		if (row[0] == NULL)
		{
			std::ostringstream strErr;
			strErr<<FunctionName.c_str()<<" ("<< pthread_self()<<") Abonent value is empty ";
			throw exception(strErr.str(), ERROR );
		} else {
			Abonent = atoi (row[0]);
		};
		
		  RetBuff[0] = 1;
		  RetBuff[1] = static_cast<uint8_t>(Length>>8);
		  RetBuff[2] = static_cast<uint8_t>(Length);
		  RetBuff[3] = static_cast<uint8_t>(ClientInPacket>>8);
		  RetBuff[4] = static_cast<uint8_t>(ClientInPacket);
		  
		  RetBuff[5] = static_cast<uint8_t>(Abonent>>8);
		  RetBuff[6] = static_cast<uint8_t>(Abonent);
 		  
		  uint16_t crc = CRC16(&RetBuff[0],Length-2);

		  RetBuff[Length-2] = static_cast<uint8_t>(crc >> 8);
		  RetBuff[Length-1] = static_cast<uint8_t>(crc);

		if (conf.GetLoggingOptions() & NET_PACKETS)
		{
			std::ostringstream strMess;
			strMess <<FunctionName.c_str()<<" ("<< pthread_self()<<") packet ";
			strMess << std::endl;
			for (std::size_t i=0; i< RetBuff.size(); i++)
			{
				strMess << std::hex << "0x"<<static_cast<int>(RetBuff[i]);
				strMess << " ";
				if (i>0 && (i%5 ==0) )
				{
					strMess <<	std::endl;
				};
			};
			
			logger::Instance().LogMessage(INFO, strMess.str());
		};
		Socket->send(RetBuff);
  } catch (exception exc)
  {
	logger::Instance().LogMessage(exc);
	return false;
  };
	
	return true;
};

bool SendToClientOptions(uint16_t ClientInPacket, generic_socket* Socket)
{
const uint16_t Length = 32;
const uint16_t PhoneNumOffset = 15;

std::vector<byte> RetBuff(Length, static_cast<byte> (0));
MYSQL_ROW row;
my_sql_writer* mysql_wrtr = NULL;
uint32_t TempTime;
tm LocalTime;
configurator&  conf =  configurator::Instance();

try
  {
	mysql_writer_factory& mysql_fact = mysql_writer_factory::Instance();

		try
		  {
			mysql_wrtr = mysql_fact.GetThreadMySQLWriter();
			} catch (exception exc)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") GetThreadMySQLWriter returned "<< exc.what();
				throw exception(strErr.str(), ERROR );			
			};

//		mysql_wrtr->ExecuteSQL("BEGIN");
		mysql_wrtr->ExecuteSQL("select FoneServer from monitor limit 0,1;"); // TODO - which row is necessary? Does it have only one?
		if (mysql_wrtr->NumRows()!= 1)
		{
			std::ostringstream strErr;
			strErr<< "SendToClientOptions ("<< pthread_self()<<") query to monitor returned "<< mysql_wrtr->NumRows()<<" rows";
			throw exception(strErr.str(), ERROR );			
		};
		MYSQL_ROW row  = mysql_wrtr->FetchRow();
		if ((row[0] == NULL) ||
			(strlen(row[0])==0))
		{
			std::ostringstream strErr;
			strErr<< "SendToClientOptions ("<< pthread_self()<<") FoneServer value is empty ";
			throw exception(strErr.str(), ERROR );
		};
		int PhoneNumLen = strlen(row[0]);
		
	   for (int i=0;i<PhoneNumLen;i++)
	   {
			if ((row[0][i] >= 0x30)&&
				(row[0][i] <= 0x39)) 
				{
					RetBuff[PhoneNumOffset+i]= (row[0][i]-0x30);					
				};
		};

		  RetBuff[0] = 0;
		  RetBuff[1] = static_cast<uint8_t>(Length>>8);
		  RetBuff[2] = static_cast<uint8_t>(Length);
		  RetBuff[3] = static_cast<uint8_t>(ClientInPacket>>8);
		  RetBuff[4] = static_cast<uint8_t>(ClientInPacket);
		  
		  //preparing current time
		  time_t TimeNow = time(NULL);
		  if (localtime_r(&TimeNow, &LocalTime) == NULL)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") localtime_r returned NULL";
				throw exception(strErr.str(), ERROR );
			};
		  TempTime = (static_cast<uint32_t>(LocalTime.tm_year-100)<<20) + //-100 because tm_year has year as year-1900
				 (static_cast<uint32_t>(LocalTime.tm_mon+1)<<16) + //+1 beacause tm_mon counts month from 0
				 (static_cast<uint32_t>(LocalTime.tm_mday)<<11) +
				 (static_cast<uint32_t>(LocalTime.tm_hour)<<6) +
				  LocalTime.tm_min;
		  RetBuff[5] = static_cast<uint8_t>(TempTime>>24);
		  RetBuff[6] = static_cast<uint8_t>(TempTime>>16);
		  RetBuff[7] = static_cast<uint8_t>(TempTime>>8);
		  RetBuff[8] = static_cast<uint8_t>(TempTime);

		  std::ostringstream strSQL;
		  strSQL<<"select HalfPic1, Pic1, HalfPic2, Pic2, HalfPic3, Night from tz where Num =" << LocalTime.tm_mon+1; //tm_mon started from 0 but monthis in DB from 1
		  if (conf.GetLoggingOptions() & SQL_TO_READ)
			  {
				std::ostringstream strMess;
				strMess<< "SendToClientOptions ("<< pthread_self()<<") sql "
						<< strSQL.str();
				logger::Instance().LogMessage(INFO, strMess.str());			
			  };
			mysql_wrtr->ExecuteSQL(strSQL.str()); 
			if (mysql_wrtr->NumRows()!= 1)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") query "<< strSQL.str() <<" returned "<< mysql_wrtr->NumRows()<<" rows";
				throw exception(strErr.str(), ERROR );
			};

		  row  = mysql_wrtr->FetchRow();
		  
			if (row[0] ==NULL)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") query "<< strSQL.str() <<" returned NULL for HalfPic1";
				throw exception(strErr.str(), ERROR );
			};
			if (row[1] ==NULL)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") query "<< strSQL.str() <<" returned NULL for Pic1";
				throw exception(strErr.str(), ERROR );
			};
			if (row[2] ==NULL)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") query "<< strSQL.str() <<" returned NULL for HalfPic2";
				throw exception(strErr.str(), ERROR );
			};
			if (row[3] ==NULL)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") query "<< strSQL.str() <<" returned NULL for Pic2";
				throw exception(strErr.str(), ERROR );
			};
			if (row[4] ==NULL)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") query "<< strSQL.str() <<" returned NULL for HalfPic3";
				throw exception(strErr.str(), ERROR );
			};
			if (row[5] ==NULL)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") query "<< strSQL.str() <<" returned NULL for Night";
				throw exception(strErr.str(), ERROR );
			};

		  RetBuff[9] = static_cast<uint8_t>(atoi(row[0])); 
		  RetBuff[10] = static_cast<uint8_t>(atoi(row[1]));
		  RetBuff[11] = static_cast<uint8_t>(atoi(row[2]));
		  RetBuff[12] = static_cast<uint8_t>(atoi(row[3]));
		  RetBuff[13] = static_cast<uint8_t>(atoi(row[4]));
		  RetBuff[14] = static_cast<uint8_t>(atoi(row[5]));

		  strSQL.str("");
		  strSQL<<"Select Disbalans,TimeDisbalans  from gsm where BSNum = " << ClientInPacket;
		  if (conf.GetLoggingOptions() & SQL_TO_READ)
			  {
				std::ostringstream strMess;
				strMess<< "SendToClientOptions ("<< pthread_self()<<") sql "
						<< strSQL.str();
				logger::Instance().LogMessage(INFO, strMess.str());			
			  };
		  mysql_wrtr->ExecuteSQL(strSQL.str());
		  if (mysql_wrtr->NumRows()!= 1)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") query "<< strSQL.str()<<" returned "<< mysql_wrtr->NumRows()<<" rows";
				throw exception(strErr.str(), ERROR );
			};

		  row  = mysql_wrtr->FetchRow();

			if (row[0] ==NULL)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") query "<< strSQL.str() <<" returned NULL for Disbalans";
				throw exception(strErr.str(), ERROR );
			};

			if (row[1] ==NULL)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientOptions ("<< pthread_self()<<") query "<< strSQL.str() <<" returned NULL for TimeBalans";
				throw exception(strErr.str(), ERROR );
			};

		  RetBuff[27] = static_cast<uint8_t>(atoi(row[0])); 
		  RetBuff[28] = static_cast<uint8_t>(atoi(row[1]));

		  uint16_t crc = CRC16(&RetBuff[0],Length-2);

		  RetBuff[Length-2] = static_cast<uint8_t>(crc >> 8);
		  RetBuff[Length-1] = static_cast<uint8_t>(crc);

		if (conf.GetLoggingOptions() & NET_PACKETS)
		{
			std::ostringstream strMess;
			strMess << "SendToClientOptions ("<< pthread_self()<<") packet ";
			strMess << std::endl;
			for (std::size_t i=0; i< RetBuff.size(); i++)
			{
				strMess << std::hex << "0x"<<static_cast<int>(RetBuff[i]);
				strMess << " ";
				if (i>0 && (i%5 ==0) )
				{
					strMess <<	std::endl;
				};
			};
			
			logger::Instance().LogMessage(INFO, strMess.str());
		};
		Socket->send(RetBuff);
//		mysql_wrtr->ExecuteSQL("COMMIT");
  } catch (exception exc)
  {
//	mysql_wrtr->ExecuteSQL("ROLLBACK");
	logger::Instance().LogMessage(exc);
	return false;
  };
	
	return true;
}


bool SendToClientRele(uint16_t ClientInPacket, generic_socket* Socket)
{
const uint32_t ConstOffsetForRecord = 4;
std::vector<byte> RetBuff;
MYSQL_ROW row;
my_sql_writer* mysql_wrtr = NULL;
uint32_t TempTime;
tm LocalTime;
configurator&  conf =  configurator::Instance();
uint32_t t, data, p;
float f;

try
  {
	mysql_writer_factory& mysql_fact = mysql_writer_factory::Instance();

		try
		  {
			mysql_wrtr = mysql_fact.GetThreadMySQLWriter();
			} catch (exception exc)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientRele ("<< pthread_self()<<") GetThreadMySQLWriter returned "<< exc.what();
				throw exception(strErr.str(), ERROR );			
			};

//		mysql_wrtr->ExecuteSQL("BEGIN");
		std::ostringstream strSQL;
		uint32_t ClientAtBeginning = static_cast<uint32_t>(ClientInPacket)<<16;
		strSQL<<"Select Abonent,StatusPodkl,StatusPotreb,StatusReleNew,Limited from a2 where Abonent > "<<ClientAtBeginning<<"  and Abonent < ("<<ClientAtBeginning<<" + 65536) and StatusReleOld <> StatusReleNew";
		
	    if (conf.GetLoggingOptions() & SQL_TO_READ)
			  {
				std::ostringstream strMess;
				strMess<< "SendToClientRele ("<< pthread_self()<<") sql "
						<< strSQL.str();
				logger::Instance().LogMessage(INFO, strMess.str());			
			  };

		mysql_wrtr->ExecuteSQL(strSQL.str());
		if (mysql_wrtr->NumRows() == 0)
		{
			std::ostringstream strMess;
			strMess<< "SendToClientRele ("<< pthread_self()<<") query "<< strSQL.str()<<" returned "<< mysql_wrtr->NumRows()<<" rows. End the Rele state sending";
			logger::Instance().LogMessage(INFO, strMess.str());			
		};
		MYSQL_ROW row;
		unsigned long NRecords = mysql_wrtr->NumRows();
		
		uint32_t nrec = 0,RecIncrem = 0, nrecmax, Length, OffsetInsideRecord;
		do //while (nrec <= Nrecords)
		{
			 if((RecIncrem+1)*120 < NRecords) nrecmax = 120;
			   else nrecmax = NRecords - RecIncrem*120;
			 Length = 16 + nrecmax*4 + 2;
			 
			 RetBuff.resize(Length, static_cast<byte> (0));
			 
			RetBuff[0] = 100;
			RetBuff[1] = static_cast<uint8_t>(Length>>8);
			RetBuff[2] = static_cast<uint8_t>(Length);
			RetBuff[3] = static_cast<uint8_t>(ClientInPacket>>8);
			RetBuff[4] = static_cast<uint8_t>(ClientInPacket);
			  
		  //preparing current time
		  time_t TimeNow = time(NULL);
		  if (localtime_r(&TimeNow, &LocalTime) == NULL)
			{
				std::ostringstream strErr;
				strErr<< "SendToClientRele ("<< pthread_self()<<") localtime_r returned NULL";
				throw exception(strErr.str(), ERROR );
			};
		  TempTime = (static_cast<uint32_t>(LocalTime.tm_year-100)<<20) + //-100 because tm_year holds year as year-1900
				 (static_cast<uint32_t>(LocalTime.tm_mon)<<16) +
				 (static_cast<uint32_t>(LocalTime.tm_mday)<<11) +
				 (static_cast<uint32_t>(LocalTime.tm_hour)<<6) +
				  LocalTime.tm_min;
		  RetBuff[5] = static_cast<uint8_t>(TempTime>>24);
		  RetBuff[6] = static_cast<uint8_t>(TempTime>>16);
		  RetBuff[7] = static_cast<uint8_t>(TempTime>>8);
		  RetBuff[8] = static_cast<uint8_t>(TempTime);

		  OffsetInsideRecord = 0;
		  while ((row = mysql_wrtr->FetchRow())!=NULL) //ATTENTION   - FetchRow takes on next iteration (RecIncrem = 1,2,3,4) next record from recordset, it does not start from first record
			{
				//FieldByName("Abonent")->AsInteger;
				if (row[0] != NULL)
				{
					t=static_cast<uint32_t>(atoi(row[0]));
				}
				else
				{
					std::ostringstream strMess;
					strMess<< "SendToClientRele ("<< pthread_self()<<") Abonent field in row "<<static_cast<uint32_t>(OffsetInsideRecord/ConstOffsetForRecord)<<" of result of query " << strSQL.str() << " has invalid value "<< t ;
					logger::Instance().LogMessage(ERROR, strMess.str());
					OffsetInsideRecord +=ConstOffsetForRecord;
					continue;
				};
				RetBuff[16+OffsetInsideRecord+1] = static_cast<uint8_t>(t>>8);
				RetBuff[16+OffsetInsideRecord] = static_cast<uint8_t>(t);
				
				data=0;
				//FieldByName("StatusPodkl")->AsInteger;
				if (row[1] != NULL)
				{
					p=static_cast<uint32_t>(atoi(row[1]));
				} 
				else
				{
					p=0;
				};
 			    p <<= 3;
				data +=p;

				//FieldByName("StatusPotreb")->AsInteger;
				if (row[2] != NULL)
				{
					p = static_cast<uint32_t>(atoi(row[2]));
				}
				else
				{
					p = 0;
				};
				p <<= 5;
				data +=p;
				
				//FieldByName("StatusReleNew")->AsBoolean;
				if (row[3] != NULL)
				{
					p = static_cast<uint32_t>(atoi(row[3]));
				}
				else
				{
					p = 1;
				};
				
			    if(p!=0)
				{
					p = 0x80;
				}
  			    data +=p;
				RetBuff[16 + OffsetInsideRecord+1] += static_cast<uint8_t>(data);			

				//FieldByName("Limit")->AsString;
				if (row[4] != NULL)
				{			
					f = static_cast<float>(atof(row[4])) * 10;
				}
				else
				{
					f = 0;
				};
				p = static_cast<uint32_t>(f);
				p = p & 0xff; 
				RetBuff[16 + OffsetInsideRecord + 2] = static_cast<uint8_t>(p);

				RetBuff[16 + OffsetInsideRecord + 3] += 0;
				OffsetInsideRecord += ConstOffsetForRecord;
				RecIncrem++;
			};
			uint16_t crc = CRC16(&RetBuff[0],Length-2);

			RetBuff[Length-2] = static_cast<uint8_t>(crc >> 8);
			RetBuff[Length-1] = static_cast<uint8_t>(crc);
		
			try	
			 {
				if (conf.GetLoggingOptions() & NET_PACKETS)
				{
					std::ostringstream strMess;
					strMess << "SendToClientRele ("<< pthread_self()<<") packet to SEND ";
					strMess << std::endl;
					strMess << std::hex;
					for (std::size_t i=0; i< RetBuff.size(); i++)
					{
						strMess << "0x"<<static_cast<int>(RetBuff[i]);
						strMess << " ";
						if (i>0 && (i%5 ==0) )
						{
							strMess <<	std::endl;
						};
					};
					
					logger::Instance().LogMessage(INFO, strMess.str());
				};

				Socket->send(RetBuff);
				if (NRecords>0) 
				{
					std::vector<byte> RecvBuff;
					recieve_data_packet(Socket, RecvBuff);
					ReceiveEnergy(RecvBuff);
				};
			 }
			catch (exception exc)
			 {
				std::ostringstream strMess;
				strMess<< "SendToClientRele ("<< pthread_self()<<") Error send/recv data "<< exc.what();
				logger::Instance().LogMessage(ERROR, strMess.str());				 
			 };
			 
			 nrec += 120;
			 
			 if(nrec < NRecords) sleep(3000);
		 } while (nrec <= NRecords);
		
  } catch (exception exc)
  {
//	mysql_wrtr->ExecuteSQL("ROLLBACK");
	logger::Instance().LogMessage(exc);
	return false;
  };
  
	return true;
}

extern "C" void* socket_thread_function(void* attr)
{
	const uint8_t BeginDataTransferLen = 4;

	generic_socket* socket = static_cast<generic_socket*> (attr);
	mysql_writer_factory& mysql_fact = mysql_writer_factory::Instance();
	my_sql_writer* mysql_wrtr = NULL;
	configurator&  conf =  configurator::Instance();
	try {
		mysql_wrtr = mysql_fact.GetThreadMySQLWriter();
	} catch (exception exc)
	{
		logger::Instance().LogMessage(exc);
		delete (socket);
		return NULL;		
	};

	std::vector<byte> BufHeader, BufPacket;
	int Len,nByte;
	uint16_t Controller;
	uint16_t ClientInHeader, ClientInPacket;
	time_t TimeNow = time(NULL);
	tm LocalTimeNow;

	BufHeader.clear();
	try {
		socket->recv(BufHeader, BeginDataTransferLen);
	} catch (socket_exception exc) 
	{
		//commented this error message because it fires when there is no
		// connection to the server i.e. in normal mode
		//logger::Instance().LogMessage(exc);
		delete (socket);
		mysql_fact.ReleaseThreadMySQLWriter();
		return NULL;		
	};
	
	if (localtime_r (&TimeNow, &LocalTimeNow) == NULL)
	{
		std::string strError = "socket_thread_function localtime_r call failed: ";
		strError += strerror(errno);
		logger::Instance().LogMessage(ERROR, strError);
		delete (socket);
		mysql_fact.ReleaseThreadMySQLWriter();
		return NULL;
	};
	
	if (BufHeader[0] == '#')
	{
		ClientInHeader = (static_cast<uint16_t>(BufHeader[1])<<8) + BufHeader[2];
		if (conf.GetLoggingOptions() & NET_PACKETS)
		{
			std::ostringstream strMess;
			strMess << "socket_thread_function packet at the beginning of transfer("<< pthread_self()<<")";
			strMess << std::endl;
			strMess << std::hex;
			for (std::size_t i=0; i< BufHeader.size(); i++)
			{
				strMess << static_cast<int>(BufHeader[i]);
				strMess << " ";
				if (i>0 && (i%5 ==0) )
				{
					strMess <<	std::endl;
				};
			};
			
			logger::Instance().LogMessage(INFO, strMess.str());
			
		};
	
		switch (BufHeader[3]) {
			case CODE_TIMER:
				SendToClientOptions(ClientInHeader,socket); //TODO add check for result

				if (!recieve_data_packet(socket, BufPacket)){
					break;		
				};

				 ReceiveEnergy(BufPacket); //TODO add check for result
				 ClientInPacket = (BufPacket[3]<<8) + BufPacket[4];
				 
				if (ClientInHeader != ClientInPacket)
				{
					std::ostringstream strError;
					strError << "socket_thread_function ClientInHeader ("<< ClientInHeader <<") != ClientInPacket("<<ClientInPacket<<")";
					
					logger::Instance().LogMessage(ERROR, strError.str());
					break;
				};
				SendToClientRele(ClientInPacket,socket);

				break;
			case CODE_RING:
				SendToClientMonitoring(ClientInHeader,socket);

				if (recieve_data_packet(socket, BufPacket))
				{
					ReceivePower(BufPacket);
				};
				
				break;
			case CODE_ALARM:
				//initaly sends nothing back

				if (recieve_data_packet(socket, BufPacket))
				{
					ReceiveAlarm(BufPacket);
				};
				
				break;
			default:
				std::ostringstream strError;
				strError << "socket_thread_function unrecognized packet at the beginning (but with #) of transfer: ";
				strError << std::endl;
				strError << std::hex;
				for (std::size_t i=0; i< BufHeader.size(); i++)
				{
					strError << static_cast<int>(BufHeader[i]);
					strError << " ";
					if (i>0 && (i%5 ==0) )
					{
						strError <<	std::endl;
					};
				};
				
				logger::Instance().LogMessage(ERROR, strError.str());
		};
	} else { //if (BufHeader[0] == '#')
		std::ostringstream strError;
		strError << "socket_thread_function unrecognized packet at the beginning of transfer: ";
		strError << std::endl;
		strError << std::hex;
		for (std::size_t i=0; i< BufHeader.size(); i++)
		{
			strError << static_cast<int>(BufHeader[i]);
			strError << " ";
			if (i>0 && (i%5 ==0) )
			{
				strError <<	std::endl;
			};
		};
		
		logger::Instance().LogMessage(ERROR, strError.str());
	};


	sleep (200);
	
	mysql_fact.ReleaseThreadMySQLWriter();
	delete (socket);
	return NULL;
}

void reciever::StartRecieve(uint64_t  PortNumber,
						unsigned int SendRecvTimeout,
						unsigned short MaxConnectQueue)
{
	pthread_attr_t      attr;
	int pthread_creating_result = 0;
	generic_socket* accepted_socket = NULL;
	pthread_t pthread_id;

	mStopRecieveFlag = true;
	try {
		mSocket.reset(new server_socket(SendRecvTimeout,
										MaxConnectQueue,
                                        PortNumber));
        mSocket->initialize();
	} catch (socket_exception sock_exc){
	     logger::Instance().LogMessage(sock_exc);
	     return;
	};
	

	if (pthread_attr_init(&attr)!=::EOK){
          logger::Instance().LogMessage(ERROR, "Can`t pthread_attr_init(&attr)");
	      return;
          };

	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)!=::EOK){
          logger::Instance().LogMessage(ERROR, "Can`t pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)");
	      return;
         };
	
	mStopRecieveFlag = false;
	
	while(! mStopRecieveFlag){
		try {
		    accepted_socket=mSocket->accept();
		} catch (socket_exception sock_exc){
	     logger::Instance().LogMessage(sock_exc);
	     continue;
	    };

        pthread_creating_result=pthread_create(&pthread_id, &attr, socket_thread_function, accepted_socket);
		if ( pthread_creating_result != ::EOK){
			   std::string message("fail to create device thread : ");	
			   message+=strerror(pthread_creating_result);
               logger::Instance().LogMessage(ERROR, message);
		};

	}; // while(true)
	
}


void reciever::StopRecieve()
{
	mStopRecieveFlag = true;
}


}

