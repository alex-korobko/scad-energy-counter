#include <termios.h>
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
	m_app_version = "2.0.2";
	
};

configurator::~configurator()
{
}

configurator& configurator::instance()
{
	static configurator instConf;
	
	return instConf;
}

configurator::configurator(const configurator& rhs)
{
}

void configurator::load_configuration(const std::string& config_file_name)
{
    // Create an empty property tree object
    using boost::property_tree::ptree;
    ptree pt;

    // Load the XML file into the property tree. If reading fails
    // (cannot open file, parse error), an exception is thrown.
    read_xml(config_file_name, pt);

    m_database_user_name = pt.get<std::string>("config.db.user_name");
    m_database_password = pt.get<std::string>("config.db.password");
    m_database_host = pt.get("config.db.host", "localhost");
	m_database_name = pt.get("config.db.name", "electro");
	m_database_port = pt.get("config.db.port",28);

	m_tcp_port = pt.get("config.socket.port",3131);
	m_tcp_recv_send_timeout_sec = pt.get("config.socket.recv_send_timeout_sec",7200); // 2 hours
	m_tcp_max_connect_queue = pt.get("config.socket.max_connect_queue",128); 

    // Iterate over the config.log.options section and store all found
    // options in the logoptions. The get_child() function
    // returns a reference to the child at the specified path; if
    // there is no such child, it throws an exception. Property tree iterators
    // are models of BidirectionalIterator.
	m_log_opt = NOTHING;
	BOOST_FOREACH(ptree::value_type &v,pt.get_child("config.log.options"))
	{
        if(std::string("all").compare( v.second.data())==0)
		{
			m_log_opt |= EVERYTHING; //log everything
		} else if (std::string("sql_to_read").compare( v.second.data())==0)
		{
			m_log_opt |= SQL_TO_READ; 
		} else if (std::string("sql_to_write").compare( v.second.data())==0)
		{
			m_log_opt |= SQL_TO_WRITE; 
		} else if (std::string("net_packets").compare( v.second.data())==0)
		{
			m_log_opt |= NET_PACKETS;
		} else if (std::string("modem").compare( v.second.data())==0)
		{
			m_log_opt |= MODEM; 
			
		} else
		{
			std::string strError = "configurator::load_configuration unrecognized config.log.options entry :";
			strError += v.second.data();
			throw exception(strError, ERROR);
		};
	};

	m_server_phone_number = pt.get("config.server.phone_number","380972382831");

	m_modem_port_number = pt.get("config.modem.port",0);
	m_modem_delay_usecond = pt.get("config.modem.delay_usecond",1);
	m_modem_recieve_timeout_seconds = pt.get("config.modem.recieve_timeout_seconds",1);

	std::string tmp_str = pt.get("config.modem.baud_rate", "19200");
	if(std::string("9600").compare(tmp_str)==0)
	{
		m_modem_baud_rate = BAUD_9600; 
	} else if (std::string("19200").compare(tmp_str)==0)
	{
		m_modem_baud_rate = BAUD_19200; 
	} else if (std::string("115200").compare(tmp_str)==0)
	{
		m_modem_baud_rate = BAUD_115200; 
	} else
	{
		std::string strError = "configurator::load_configuration unrecognized config.modem.baud_rate entry :";
		strError += tmp_str;
		strError +=  " now supported only 9600, 19200 and 115200";
		throw exception(strError, ERROR);
	};

	tmp_str = pt.get("config.modem.parity", "disabled");
	if(std::string("disabled").compare(tmp_str)==0)
	{
		m_modem_parity = PARITY_DISABLE; 
	} else if (std::string("odd").compare(tmp_str)==0)
	{
		m_modem_parity = PARITY_ODD; 
	} else if (std::string("even").compare(tmp_str)==0)
	{
		m_modem_parity = PARITY_EVEN; 
	} else
	{
		std::string strError = "configurator::load_configuration unrecognized config.modem.parity entry :";
		strError += tmp_str;
		strError +=  " supported only 'disabled', 'odd' and 'even'";
		throw exception(strError, ERROR);
	};

	tmp_str = pt.get("config.modem.data_bits", "8 bits");
	if(std::string("5 bits").compare(tmp_str)==0)
	{
		m_modem_data_bits = DATA_BITS_5;
	} else if (std::string("6 bits").compare(tmp_str)==0)
	{
		m_modem_data_bits = DATA_BITS_6;
	} else if (std::string("7 bits").compare(tmp_str)==0)
	{
		m_modem_data_bits = DATA_BITS_7;
	} else if (std::string("8 bits").compare(tmp_str)==0)
	{
		m_modem_data_bits = DATA_BITS_8;
	} else
	{
		std::string strError = "configurator::load_configuration unrecognized config.modem.data_bits entry :";
		strError += tmp_str;
		strError +=  " supported only '5 bits', '6 bits', '7 bits' and '8 bits'";
		throw exception(strError, ERROR);
	};
	
	tmp_str = pt.get("config.modem.stop_bits", "1 bit");
	if(std::string("1 bit").compare(tmp_str)==0)
	{
		m_modem_stop_bits = STOP_BITS_1;
	} else if (std::string("2 bits").compare(tmp_str)==0)
	{
		m_modem_stop_bits = STOP_BITS_2;
	} else
	{
		std::string strError = "configurator::load_configuration unrecognized config.modem.stop_bits entry :";
		strError += tmp_str;
		strError +=  " supported only '1 bit' and '2 bits'";
		throw exception(strError, ERROR);
	};

	m_scheduled_call_to_controller_interval = pt.get("config.scheduled.call_to_controller_interval",60);
}

configurator& configurator::operator=(const configurator& rhs)
{
	return *this;
}

}

