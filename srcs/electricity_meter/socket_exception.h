#ifndef __SOCKET_EXCEPTION_H__
#define __SOCKET_EXCEPTION_H__

namespace electricity_meter
{

class socket_exception : public exception
{
	public:
	    socket_exception(std::string ErrMessage, MessType MessageType = ERROR):exception(ErrMessage, MessageType){};
		virtual ~socket_exception () throw() {};
};

}

#endif
