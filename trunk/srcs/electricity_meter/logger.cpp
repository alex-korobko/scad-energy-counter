#include <string>
#include <stdexcept>
#include <algorithm>
#include <syslog.h>
#include <termios.h>

#include "common_declarations.h"

#include "exception.h"
#include "logger.h"

namespace electricity_meter
{

logger::logger() :m_app_version("0.0.0.0"), m_app_name ("unset")
{
}

logger::~logger()
{
	closelog();
}

logger& logger::instance()
{
	static logger instLogg;
	return instLogg;
}

void logger::initialize()
{
	openlog(m_app_name.c_str(), LOG_PID, LOG_LOCAL5);
};

void logger::log_message (const exception& exc)
{
	log_message(exc.GetMessType(), exc.what());
}

void logger::log_message (MessType TypeOfMess, const std::string &Message)
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
	FullMessage+=m_app_version;
	FullMessage+=" ";
	FullMessage+=type_of_mess;
	FullMessage+=" ";
	FullMessage+=Message;
	
	FullMessage.erase(std::remove(FullMessage.begin(), FullMessage.end(), '\n'), FullMessage.end());
	FullMessage.erase(std::remove(FullMessage.begin(), FullMessage.end(), '\r'), FullMessage.end());
	
	syslog(level, "%s", FullMessage.c_str());
}

}

