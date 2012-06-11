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
#include <vector>
#include <termios.h>

#include "common_declarations.h"
#include "exception.h"
#include "configurator.h"
#include "logger.h"
#include "modem.h"
#include "mysql_writer_factory.h"
#include "my_sql_writer.h"
#include "scheduled_action.h"
#include "scheduled_actions.h"
#include "scheduled_action_read_data.h"

namespace electricity_meter {

extern "C" void* scheduled_actions_thread_function(void* attr);
	
scheduled_actions::scheduled_actions() {
	m_stop_flag = true;
	m_actions.push_back(new scheduled_action_read_data(configurator::instance().scheduled_call_to_controller_interval()));
}

scheduled_actions::~scheduled_actions() {
	actions_container_type::iterator iter = m_actions.begin();
	while (iter != m_actions.end())
	{
		delete(*iter);
		++iter;
	}
}

scheduled_actions::scheduled_actions(const scheduled_actions& schact) {}

scheduled_actions& scheduled_actions::operator=(const scheduled_actions& schact) 
{
	return *this;
}

scheduled_actions& scheduled_actions::instance()
{
	static scheduled_actions shactInst;
	return shactInst;
}

void scheduled_actions::start_actions()
{
	const std::string function_name = "start_actions";
	pthread_attr_t      attr;
	int pthread_creating_result = 0;
	pthread_t pthread_id;

	if (!m_stop_flag) {
          logger::instance().log_message(ERROR, function_name+": Actions thread is already started. Call stop_actions first.");
	      return;		
	};

	if (pthread_attr_init(&attr)!=::EOK){
          logger::instance().log_message(ERROR, function_name+": Can`t pthread_attr_init(&attr)");
	      return;
    };

	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)!=::EOK){
          logger::instance().log_message(ERROR,  function_name+": Can`t pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED)");
	      return;
	};

    pthread_creating_result=pthread_create(&pthread_id, &attr, &scheduled_actions_thread_function, NULL);
	if ( pthread_creating_result != ::EOK){
	   std::string message(function_name);
	   message+=": fail to create device thread: ";	   
	   message+=strerror(pthread_creating_result);
       logger::instance().log_message(ERROR, message);
	};
}

void scheduled_actions::stop_actions()
{
	m_stop_flag = true;
}

void scheduled_actions::thread_function()
{
	const std::string function_name = "scheduled_actions::thread_function";
	
	m_stop_flag = false;
	actions_container_type::iterator iter_actions;
	logger::instance().log_message(INFO, function_name + " - started");
	while (! m_stop_flag) {
		iter_actions=m_actions.begin();
		while (iter_actions != m_actions.end()) {
			(*iter_actions)->try_invoce();
			if (m_stop_flag) {
				logger::instance().log_message(INFO, function_name + " - exited by stop flag");			
				return;
			};
			++iter_actions;
		};
		sleep(1); //1 second
	};	
	logger::instance().log_message(INFO, function_name + " - exited by stop flag from while");				
}

	
void* scheduled_actions::scheduled_actions_thread_function(void* attr)
{
	scheduled_actions::instance().thread_function();
}

};