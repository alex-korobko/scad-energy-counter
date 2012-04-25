#include <stdexcept>

#include "common_declarations.h"
#include "exception.h"

namespace electricity_meter
{

exception::exception(std::string ErrMessage, MessType MessageType) :
	 std::runtime_error(ErrMessage), mMessageType(MessageType)
{
}

exception::~exception() throw()
{
}

}

