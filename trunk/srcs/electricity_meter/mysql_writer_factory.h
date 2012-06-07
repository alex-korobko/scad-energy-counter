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
	writers_storage m_writers;
	
	pthread_mutex_t m_mutex;
	MYSQL m_connection;
public:
	static mysql_writer_factory& instance();
	void connect(const std::string& host_name, 
				const std::string& db_name, 
				const std::string& user_name,
				const std::string& password,
				unsigned int port_number,
				bool auto_commit = false) throw (exception);

	my_sql_writer* get_thread_mySQL_writer() throw (exception);
	void release_thread_mySQL_writer();
};

}

#endif // MYSQL_WRITER_FACTORY_H
