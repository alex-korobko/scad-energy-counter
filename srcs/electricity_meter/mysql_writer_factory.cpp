#include <stdexcept>
#include <string>
#include <map>
#include <pthread.h>
#include <mysql.h>
#include <string.h>
#include <errno.h>
#include <termios.h>

#include "common_declarations.h"
#include "exception.h"
#include "my_sql_writer.h"
#include "mysql_writer_factory.h"

namespace electricity_meter
{
	
mysql_writer_factory::mysql_writer_factory()
{
	int errnum = pthread_mutex_init(&m_mutex, NULL);
	if ( errnum!=0 ) 
	{
		std::string srtError  = "In mysql_writer_factory::mysql_writer_factory = pthread_mutex_init failed: ";
		srtError += strerror(errnum);
		throw exception (srtError, ERROR);
	};
	
	if (mysql_library_init(0, NULL, NULL) != 0)
	{
		pthread_mutex_destroy(&m_mutex);
		std::string srtError  = "In mysql_writer_factory::mysql_writer_factory = mysql_library_init failed: ";
		srtError += strerror(errno);
		throw exception (srtError, ERROR);
	};
	
}

mysql_writer_factory::~mysql_writer_factory()
{
	mysql_close(&m_connection);
	mysql_library_end();
	pthread_mutex_destroy(&m_mutex); //add trylock/unlock before?
	writers_storage::iterator iterWriter = m_writers.begin();
	if (iterWriter != m_writers.end())
	{
		delete(iterWriter->second);
		++iterWriter;
	};
}

mysql_writer_factory& mysql_writer_factory::instance()
{
	static mysql_writer_factory inst;
	return inst;
}

void mysql_writer_factory::connect(const std::string& host_name, 
						  const std::string& db_name, 
						  const std::string& user_name,
						  const std::string& password,
						  unsigned int port_number,
						  bool auto_commit) throw (exception)
{
	const int TriesCount = 10;
	MYSQL *ConnRes = NULL;
	int CurrTry = 0;
	my_bool arg=1;
	mysql_close (&m_connection);
	
	while (CurrTry<TriesCount)
	{
		ConnRes = mysql_real_connect(&m_connection,
							host_name.c_str(),
							user_name.c_str(),
							password.c_str(),
							db_name.c_str(),
							port_number,NULL,0);
		if (ConnRes != NULL)
		{
			if (mysql_options(&m_connection, MYSQL_OPT_RECONNECT, &arg) != 0)
			{
				std::string strErr =  "mysql_writer_factory::Connect setting option to reconnect failed : ";
				strErr += mysql_error(&m_connection);
				throw exception (strErr, ERROR);
			}

			break;
		};
		
		CurrTry++;
		sleep(10);
	};
	
	if (ConnRes == NULL)
	{
		std::string strErr =  "mysql_writer_factory::Connect connection failed : ";
		strErr += mysql_error(&m_connection);
		throw exception (strErr, ERROR);
	}
	
	int AutoCommitVal = 0;  // 0 disables autocommit
	if (auto_commit)
	{
		AutoCommitVal = 1;
	};

	if (mysql_autocommit(&m_connection, AutoCommitVal))
	{
		std::string strErr =  "mysql_writer_factory::Connect setting autocommit off failed : ";
		strErr += mysql_error(&m_connection);
		throw exception (strErr, ERROR);
	}
	
}

my_sql_writer* mysql_writer_factory::get_thread_mySQL_writer() throw (exception)
{
	pthread_mutex_lock(&m_mutex);
	writers_storage::iterator iterWriter = m_writers.find(pthread_self());
	if (iterWriter == m_writers.end())
	{
		my_sql_writer* wrtr = new my_sql_writer(m_mutex, m_connection);
		std::pair<writers_storage::iterator, bool> result = m_writers.insert (writers_storage::value_type(pthread_self(),  wrtr));
		if (! result.second )
		{
			delete (wrtr);
			pthread_mutex_unlock(&m_mutex);
			throw exception("mysql_writer_factory::get_thread_mySQL_writer() : cannot create writer entry", ERROR);
		};
		iterWriter = result.first;
	};
	pthread_mutex_unlock(&m_mutex);
	return iterWriter->second;
}

void mysql_writer_factory::release_thread_mySQL_writer()
{
	pthread_mutex_lock(&m_mutex);
	writers_storage::iterator iterWriter = m_writers.find(pthread_self());
	if (iterWriter == m_writers.end())
	{
		delete (iterWriter->second);
		m_writers.erase(iterWriter);
	};
	pthread_mutex_unlock(&m_mutex);
}

}

