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

namespace electricity_meter
{

scheduled_actions::scheduled_actions(): {}

scheduled_actions::~scheduled_actions() {}

scheduled_actions::scheduled_actions(const scheduled_actions& schact) {}

scheduled_actions& scheduled_actions::operator=(const scheduled_actions& schact) 
{
	return *this;
}

scheduled_actions& scheduled_actions::Instance()
{
	static scheduled_actions shactInst;
	return shactInst;
}

void scheduled_actions::StartActions(vector<scheduled_action> Actions)
{
	const std::string function_name = "StartActions";
	pthread_attr_t      attr;
	int pthread_creating_result = 0;
	pthread_t pthread_id;

	if (pthread_attr_init(&attr)!=::EOK){
          logger::Instance().LogMessage(ERROR, function_name+": Can`t pthread_attr_init(&attr)");
	      return;
          };

	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)!=::EOK){
          logger::Instance().LogMessage(ERROR,  function_name+"Can`t pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)");
	      return;
         };
	

    pthread_creating_result=pthread_create(&pthread_id, &attr, socket_thread_function, accepted_socket);
	if ( pthread_creating_result != ::EOK){
	   std::string message(function_name);
	   message+="fail to create device thread : ";	   
	   message+=strerror(pthread_creating_result);
       logger::Instance().LogMessage(ERROR, message);
	};
	
}

void scheduled_actions::StopActions()
{
}
	
extern "C" void* socket_thread_function(void* attr)
{
	const uint8_t BeginDataTransferLen = 4;
}

};