#ifndef MYSQL_WRITER_FACTORY_H
#define MYSQL_WRITER_FACTORY_H

namespace electricity_meter
{

class exception;
class my_sql_writer;

class mysql_writer_factory {
private:
	mysql_writer_factory(const mysql_writer_factory& rhs);
	mysql_writer_factory& operator=(const mysql_writer_factory& rhs);
	mysql_writer_factory();
	virtual ~mysql_writer_factory();

	typedef std::map<pthread_t, my_sql_writer*> writers_storage;
	writers_storage mWriters;
	
	pthread_mutex_t mMutex;
	MYSQL mConnection;
public:
	static mysql_writer_factory& Instance();
	void Connect(const std::string& HostName, 
				const std::string& DBName, 
				const std::string& UserName,
				const std::string& Password,
				unsigned int PortNumber,
				bool AutoCommit = false) throw (exception);

	my_sql_writer* GetThreadMySQLWriter() throw (exception);
	void ReleaseThreadMySQLWriter();
};

}

#endif // MYSQL_WRITER_FACTORY_H
