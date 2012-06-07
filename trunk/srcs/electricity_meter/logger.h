#ifndef LOGGER_H
#define LOGGER_H

namespace electricity_meter
{

class logger {
private:
	logger(const logger& rhs);
	logger& operator=(const logger& rhs);
	logger();
	~logger();

	std::string m_app_version;
	std::string m_app_name;
public:
	static logger& instance();
	void initialize();
	
	void set_app_version(const std::string& new_app_version) {m_app_version = new_app_version;};
	void set_app_name(const std::string& new_app_name) {m_app_name = new_app_name;};
	
	void log_message (MessType type_of_mess, const std::string &message);
	void log_message (const exception& exc);
};

}

#endif // LOGGER_H
