#include <string>
#include <stdexcept>
#include <syslog.h>

#include "common_declarations.h"

#include "exception.h"
#include "logger.h"

namespace electricity_meter
{

logger::logger() :mAppVersion("0.0.0.0"), mAppName ("electricity_meter")
{
}

logger::~logger()
{
	closelog();
}

logger& logger::Instance()
{
	static logger instLogg;
	return instLogg;
}

void logger::Initialize()
{
	openlog(mAppName.c_str(), LOG_PID, LOG_LOCAL5);
};

void logger::LogMessage (const exception& exc)
{
	LogMessage(exc.GetMessType(), exc.what());
}

void logger::LogMessage (MessType TypeOfMess, const std::string &Message)
{
	int level;
	std::string FullMessage;
	std::string type_of_mess;
	switch (TypeOfMess) {
		case INFO:
			type_of_mess= "INFO";
			level = LOG_INFO;
			break;
		case WARINIG:
			type_of_mess= "WARNING";
			level = LOG_WARNING;
			break;
		case ERROR:
			type_of_mess= "ERROR";
			level = LOG_ERR;
			break;			
		default:
			level = LOG_ERR;
	};
	
	FullMessage="v";
	FullMessage+=mAppVersion;
	FullMessage+=" ";
	FullMessage+=type_of_mess;
	FullMessage+=" ";
	FullMessage+=Message;
	syslog(level, "%s", FullMessage.c_str());
}

}

