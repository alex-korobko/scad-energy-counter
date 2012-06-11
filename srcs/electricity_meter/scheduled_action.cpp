#include <termios.h>
#include <time.h>
#include <string>
#include <sstream>
#include <stdexcept>

#include "common_declarations.h"
#include "exception.h"
#include "logger.h"
#include "scheduled_action.h"

namespace electricity_meter
{

scheduled_action::scheduled_action(double interval_between_invocations) :
m_interval_between_invoc(interval_between_invocations)
{
}

scheduled_action::~scheduled_action(){
}

void scheduled_action::try_invoce() {
	if (m_interval_between_invoc < difftime(time(NULL), m_time_of_prev_invoc)) {
		m_time_of_prev_invoc = time(NULL);
		this->invoce();
	};
}

void scheduled_action::invoce()
{
	logger::instance().log_message(ERROR, "scheduled_action::invoce() called!");			
};

}

