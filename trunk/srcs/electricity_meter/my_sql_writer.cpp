#include <string>
#include <stdexcept>
#include <mysql.h>

#include "common_declarations.h"

#include "exception.h"
#include "configurator.h"
#include "logger.h"
#include "my_sql_writer.h"

namespace electricity_meter
{
my_sql_writer::my_sql_writer (
	pthread_mutex_t& Mutex, 
	MYSQL& Connection) : mMutex(Mutex), mConnection(Connection) 
{
	mResult=NULL;
	mAffectedRows = -1;
	mysql_thread_init();
}

my_sql_writer::~my_sql_writer()
{
	mysql_free_result(mResult);
	mysql_thread_end();
}

void my_sql_writer::ExecuteSQL(const std::string strSQL) throw (exception)
{
	mysql_free_result(mResult);
	mResult = NULL;
	mAffectedRows = -1;
	pthread_mutex_lock(&mMutex);
    if (mysql_ping(&mConnection) !=0 )
	{
		std::string strError = "my_sql_writer::ExecuteSQL mysql_ping failed \'";
		strError += strSQL;
		strError += "\' with error ";
		strError += mysql_error(&mConnection);
		pthread_mutex_unlock(&mMutex);
		throw exception(strError, ERROR);
	}
	if (mysql_query(&mConnection, strSQL.c_str()) !=0)
	{
		std::string strError = "my_sql_writer::ExecuteSQL failed with sql \'";
		strError += strSQL;
		strError += "\' with error ";
		strError += mysql_error(&mConnection);
		pthread_mutex_unlock(&mMutex);
		throw exception(strError, ERROR);
	};
	
	mResult = mysql_store_result(&mConnection);
	mAffectedRows = mysql_affected_rows(&mConnection);
	pthread_mutex_unlock(&mMutex);
}

MYSQL_ROW my_sql_writer::FetchRow() throw (exception)
{
	return mysql_fetch_row(mResult);
}

unsigned long my_sql_writer::NumRows() throw (exception)
{
	return mAffectedRows;
}


}

