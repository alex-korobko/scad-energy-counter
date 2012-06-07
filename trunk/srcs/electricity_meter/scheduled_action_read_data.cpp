#include <time.h>
#include <termios.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>

#include "common_declarations.h"
#include "exception.h"
#include "modem_exception.h"
#include "configurator.h"
#include "logger.h"
#include "modem.h"
#include "scheduled_action.h"
#include "scheduled_action_read_data.h"

namespace electricity_meter
{

scheduled_action_read_data::scheduled_action_read_data(double interval_between_invocations) :
scheduled_action(interval_between_invocations)
{
}

scheduled_action_read_data::~scheduled_action_read_data()
{
}

void scheduled_action_read_data::invoce()
{
	configurator& config  = configurator::instance();

	std::auto_ptr<modem> current_modem ( new modem(
			config.modem_port_number(),
			config.modem_baud_rate(),
			config.modem_parity(),
			config.modem_data_bits(),
			config.modem_stop_bits(),
			config.modem_delay_usecond(),
			config.modem_recieve_timeout_seconds() ));
			
	current_modem->init();
	
	
}

}

