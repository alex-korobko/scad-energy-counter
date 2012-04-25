#include <stdexcept>
#include <string>
#include <map>
#include <pthread.h>
#include <mysql.h>
#include <string.h>
#include <errno.h>

#include "common_declarations.h"
#include "exception.h"
#include "my_sql_writer.h"
#include "mysql_writer_factory.h"

namespace electricity_meter
{
	
mysql_writer_factory::mysql_writer_factory()
{
	int errnum = pthread_mutex_init(&mMutex, NULL);
	if ( errnum!=0 ) 
	{
		std::string srtError  = "In mysql_writer_factory::mysql_writer_factory = pthread_mutex_init failed: ";
		srtError += strerror(errnum);
		throw exception (srtError, ERROR);
	};
	
	if (mysql_library_init(0, NULL, NULL) != 0)
	{
		pthread_mutex_destroy(&mMutex);
		std::string srtError  = "In mysql_writer_factory::mysql_writer_factory = mysql_library_init failed: ";
		srtError += strerror(errno);
		throw exception (srtError, ERROR);
	};
	
}

mysql_writer_factory::~mysql_writer_factory()
{
	mysql_close(&mConnection);
	mysql_library_end();
	pthread_mutex_destroy(&mMutex); //add trylock/unlock before?
	writers_storage::iterator iterWriter = mWriters.begin();
	if (iterWriter != mWriters.end())
	{
		delete(iterWriter->second);
		++iterWriter;
	};
}

mysql_writer_factory& mysql_writer_factory::Instance()
{
	static mysql_writer_factory Instance;
	return Instance;
}

void mysql_writer_factory::Connect(const std::string& HostName, 
						  const std::string& DBName, 
						  const std::string& UserName,
						  const std::string& Password,
						  unsigned int PortNumber,
						  bool AutoCommit) throw (exception)
{
	const int TriesCount = 10;
	MYSQL *ConnRes = NULL;
	int CurrTry = 0;
	my_bool arg=1;
	mysql_close (&mConnection);
	
	while (CurrTry<TriesCount)
	{
		ConnRes = mysql_real_connect(&mConnection,
							HostName.c_str(),
							UserName.c_str(),
							Password.c_str(),
							DBName.c_str(),
							PortNumber,NULL,0);
		if (ConnRes != NULL)
		{
			if (mysql_options(&mConnection, MYSQL_OPT_RECONNECT, &arg) != 0)
			{
				std::string strErr =  "mysql_writer_factory::Connect setting option to reconnect failed : ";
				strErr += mysql_error(&mConnection);
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
		strErr += mysql_error(&mConnection);
		throw exception (strErr, ERROR);
	}
	
	int AutoCommitVal = 0;  // 0 disables autocommit
	if (AutoCommit)
	{
		AutoCommitVal = 1;
	};

	if (mysql_autocommit(&mConnection, AutoCommitVal))
	{
		std::string strErr =  "mysql_writer_factory::Connect setting autocommit off failed : ";
		strErr += mysql_error(&mConnection);
		throw exception (strErr, ERROR);
	}
	
}

my_sql_writer* mysql_writer_factory::GetThreadMySQLWriter() throw (exception)
{
	pthread_mutex_lock(&mMutex);
	writers_storage::iterator iterWriter = mWriters.find(pthread_self());
	if (iterWriter == mWriters.end())
	{
		my_sql_writer* wrtr = new my_sql_writer(mMutex, mConnection);
		std::pair<writers_storage::iterator, bool> result = mWriters.insert (writers_storage::value_type(pthread_self(),  wrtr));
		if (! result.second )
		{
			delete (wrtr);
			pthread_mutex_unlock(&mMutex);
			throw exception("mysql_writer_factory::GetThreadMySQLWriter() : cannot create writer entry", ERROR);
		};
		iterWriter = result.first;
	};
	pthread_mutex_unlock(&mMutex);
	return iterWriter->second;
}

void mysql_writer_factory::ReleaseThreadMySQLWriter()
{
	pthread_mutex_lock(&mMutex);
	writers_storage::iterator iterWriter = mWriters.find(pthread_self());
	if (iterWriter == mWriters.end())
	{
		delete (iterWriter->second);
		mWriters.erase(iterWriter);
	};
	pthread_mutex_unlock(&mMutex);
}

}

