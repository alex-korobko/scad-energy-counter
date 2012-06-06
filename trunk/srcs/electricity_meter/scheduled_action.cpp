#include <time.h>

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
	if (m_interval_between_invoc > difftime(time(NULL), m_time_of_prev_invoc)) {
		m_time_of_prev_invoc = time(NULL);
		invoce();
	};
}

}

