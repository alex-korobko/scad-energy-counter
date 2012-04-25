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

	std::string mAppVersion;
	std::string mAppName;
public:
	static logger& Instance();
	void Initialize();
	
	void SetAppVersion(const std::string& newAppVersion) {mAppVersion = newAppVersion;};
	void SetAppName(const std::string& newAppName) {mAppName = newAppName;};
	
	void LogMessage (MessType TypeOfMess, const std::string &Message);
	void LogMessage (const exception& exc);
};

}

#endif // LOGGER_H
