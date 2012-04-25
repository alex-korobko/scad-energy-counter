#include <string> 

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "common_declarations.h"
#include "exception.h"
#include "configurator.h"

namespace electricity_meter
{

configurator::configurator()
{
	mAppVersion = "1.9.0";
	
};

configurator::~configurator()
{
}

configurator& configurator::Instance()
{
	static configurator instConf;
	
	return instConf;
}

configurator::configurator(const configurator& rhs)
{
}

void configurator::LoadConfiguration(const std::string& strConfigFileName)
{
    // Create an empty property tree object
    using boost::property_tree::ptree;
    ptree pt;

    // Load the XML file into the property tree. If reading fails
    // (cannot open file, parse error), an exception is thrown.
    read_xml(strConfigFileName, pt);

    mDBUserName = pt.get<std::string>("config.db.user_name");
    mDBPassword = pt.get<std::string>("config.db.password");
    mDBHost = pt.get("config.db.host", "localhost");
    mDBName = pt.get("config.db.name", "electro");
	mDBPort = pt.get("config.db.port",28);

	mPortNumber = pt.get("config.socket.port",3131);
	mRecvSendTimeoutSec = pt.get("config.socket.recv_send_timeout_sec",7200); // 2 hours
	mMaxConnectQueue = pt.get("config.socket.max_connect_queue",128); 

    // Iterate over the config.log.options section and store all found
    // options in the logoptions. The get_child() function
    // returns a reference to the child at the specified path; if
    // there is no such child, it throws. Property tree iterators
    // are models of BidirectionalIterator.
	mLogOpt = NOTHING;
	BOOST_FOREACH(ptree::value_type &v,pt.get_child("config.log.options"))
	{
        if(std::string("all").compare( v.second.data())==0)
		{
			mLogOpt = EVERYTHING; //log everything
		} else if (std::string("sql_to_read").compare( v.second.data())==0)
		{
			mLogOpt = SQL_TO_READ; 
		} else if (std::string("sql_to_write").compare( v.second.data())==0)
		{
			mLogOpt = SQL_TO_WRITE; 
		} else if (std::string("net_packets").compare( v.second.data())==0)
		{
			mLogOpt = NET_PACKETS; 
		} else
		{
			std::string strError = "configurator::configurator() unrecognized config.log.options entry :";
			strError += v.second.data();
			throw exception(strError, ERROR);
		};
	};
	
}

configurator& configurator::operator=(const configurator& rhs)
{
	return *this;
}

}

