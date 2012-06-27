#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

namespace electricity_meter
{
	
class configurator {
public:
	static configurator& instance();
	
	void load_configuration(const std::string& config_file_name);
	
	unsigned int tcp_port() const {return m_tcp_port;};
	unsigned int tcp_recv_send_timeout_sec() const {return m_tcp_recv_send_timeout_sec;};
	unsigned short tcp_max_connect_queue() const {return m_tcp_max_connect_queue;}
	
	const std::string& database_user_name() const {return m_database_user_name;};
	const std::string& database_password() const {return m_database_password;};
	const std::string& database_host() const {return m_database_host;};
	int database_port() const {return m_database_port;}; 
	const std::string& database_name() const {return m_database_name;}; 

	unsigned int modem_port_number() const { return m_modem_port_number; };
	unsigned int modem_baud_rate() const { return m_modem_baud_rate;};
	byte modem_parity() const { return m_modem_parity;};
	byte modem_data_bits() const { return m_modem_data_bits;};
	byte modem_stop_bits() const { return m_modem_stop_bits;};
	unsigned int modem_delay_usecond() const { return m_modem_delay_usecond;};
	unsigned int modem_recieve_timeout_seconds() const { return m_modem_recieve_timeout_seconds;};
	
	const std::string& server_phone_number() const {return m_server_phone_number;};
	
	const double scheduled_call_to_controller_interval() const {return m_scheduled_call_to_controller_interval;};
	
	unsigned int logging_options() const { return m_log_opt;}
	const std::string& application_version() const {return m_app_version;};
private:
	configurator();
	~configurator();
	configurator(const configurator& rhs);
	configurator& operator=(const configurator& rhs);
	
	unsigned int m_tcp_port;
	unsigned int m_tcp_recv_send_timeout_sec;
	unsigned short m_tcp_max_connect_queue; 
	
	unsigned int m_log_opt;
	
	std::string m_database_user_name;
	std::string m_database_password;
	std::string m_database_host;
	int m_database_port; 
	std::string m_database_name; 

	int m_modem_port_number;
	int m_modem_baud_rate;
	byte m_modem_parity;
	byte m_modem_data_bits;
	byte m_modem_stop_bits;
	unsigned int m_modem_delay_usecond;
	unsigned int m_modem_recieve_timeout_seconds;

	std::string m_server_phone_number; 
	std::string m_app_version; 
	
	double m_scheduled_call_to_controller_interval;
};

}

#endif // CONFIGURATOR_H
