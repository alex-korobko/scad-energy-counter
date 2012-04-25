#ifndef my_sql_writer_H
#define my_sql_writer_H

namespace electricity_meter
{

class my_sql_writer {
	friend class mysql_writer_factory;
private:
	pthread_mutex_t& mMutex;
	MYSQL& mConnection;
	MYSQL_RES* mResult;
	unsigned long mAffectedRows;
public:
	my_sql_writer (pthread_mutex_t& Mutex, MYSQL& Connection);
	~my_sql_writer();

	void ExecuteSQL(const std::string strSQL) throw (exception);
	MYSQL_ROW FetchRow() throw (exception);
	unsigned long NumRows() throw (exception);
};

}

#endif // my_sql_writer_H
