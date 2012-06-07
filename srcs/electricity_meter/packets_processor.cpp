#include <string>
#include <stdexcept>
#include <termios.h>

#include "common_declarations.h"

#include "exception.h"
#include "configurator.h"
#include "logger.h"
#include "packets_processor.h"

namespace electricity_meter
{

packets_processor::packets_processor()
{
}

packets_processor::~packets_processor()
{
}

packets_processor& packets_processor::Instance()
{
	static packets_processor instPackProc;
	return instPackProc;
}

packets_processor::packets_processor(const packets_processor& rhs)
{
}

packets_processor& packets_processor::operator=(const packets_processor& rhs)
{
	return *this;
}

}

