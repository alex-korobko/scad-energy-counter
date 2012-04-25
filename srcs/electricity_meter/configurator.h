#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

namespace electricity_meter
{

	
class configurator {
public:
	static configurator& Instance();
	
	void LoadConfiguration(const std::string& strConfigFileName);
	
	unsigned int GetPortNumber() const {return mPortNumber;};
	unsigned int GetRecvSendTimeoutSec() const {return mRecvSendTimeoutSec;};
	unsigned short GetMaxConnectQueue() const {return mMaxConnectQueue;}
	
	const std::string& GetDBUserName() const {return mDBUserName;};
	const std::string& GetDBPassword() const {return mDBPassword;};
	const std::string& GetDBHost() const {return mDBHost;};
	int GetDBPort() const {return mDBPort;}; 
	const std::string& GetDBName() const {return mDBName;}; 
	
	unsigned int GetLoggingOptions() const { return mLogOpt;}
	const std::string& GetAppVersion() const {return mAppVersion;};
private:
	configurator();
	~configurator();
	configurator(const configurator& rhs);
	configurator& operator=(const configurator& rhs);
	
	unsigned int mPortNumber;
	unsigned int mRecvSendTimeoutSec;
	unsigned short mMaxConnectQueue; 
	unsigned int mLogOpt;
	
	std::string mDBUserName;
	std::string mDBPassword;
	std::string mDBHost;
	int mDBPort; 
	std::string mDBName; 

	std::string mAppVersion; 
};

}

#endif // CONFIGURATOR_H
