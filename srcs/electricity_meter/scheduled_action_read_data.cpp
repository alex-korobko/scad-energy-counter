#include <time.h>
#include <termios.h>
#include <mysql.h>
#include <string.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <map>

#include "common_declarations.h"
#include "exception.h"
#include "configurator.h"
#include "logger.h"
#include "modem.h"
#include "mysql_writer_factory.h"
#include "my_sql_writer.h"
#include "scheduled_action.h"
#include "scheduled_action_read_data.h"

namespace electricity_meter
{

scheduled_action_read_data::scheduled_action_read_data(double interval_between_invocations) :
scheduled_action(interval_between_invocations)
{
	m_time_of_prev_invoc  = time(NULL); //wait for one interval before invocation  - to give time init everything like modem
}

scheduled_action_read_data::~scheduled_action_read_data()
{
}

void scheduled_action_read_data::invoce()
{
	const std::string func_name = "scheduled_action_read_data::invoce";
	modem::modem_data_block modem_buffer, contrl_phone_number;
	configurator& config  = configurator::instance();
	std::string tmp_string1, tmp_string2;
	
	try
	{
		mysql_writer_factory& mysql_fact = mysql_writer_factory::instance();
		my_sql_writer* mysql_wrtr = NULL;
		mysql_wrtr = mysql_fact.get_thread_mySQL_writer();
	
		try {
			mysql_wrtr->ExecuteSQL("BEGIN");
			if (!find_controller_to_call(contrl_phone_number, mysql_wrtr))
			{
				return; //nothing to do
			}
			
			std::auto_ptr<modem> current_modem ( new modem(
					config.modem_port_number(),
					config.modem_baud_rate(),
					config.modem_parity(),
					config.modem_data_bits(),
					config.modem_stop_bits(),
					config.modem_delay_usecond(),
					config.modem_recieve_timeout_seconds() ));
					
			current_modem->init();
			
			modem_buffer.push_back('A');modem_buffer.push_back('T');
			modem_buffer.insert(modem_buffer.end(), contrl_phone_number.begin(), contrl_phone_number.end());
			modem_buffer.push_back(';');modem_buffer.push_back('\r');modem_buffer.push_back('\0');
			tmp_string1 = &modem_buffer[0];
			
			current_modem->send(modem_buffer, true);
			modem_buffer.clear();
			sleep(90); //wait for 90 seconds for calling result
			current_modem->recv(modem_buffer);
			
			if (! modem_buffer.empty()) // already got some answer
			{
				modem_buffer.push_back('\0');
				tmp_string2 = &modem_buffer[0];
				if (tmp_string2.compare("0\n") != 0)
				{
					std::ostringstream strMess;
					strMess<<"command to modem "<<tmp_string1.c_str() << " returned ATV0  code '"<<tmp_string2.c_str()<<"'";
					if (tmp_string2.compare("3\n") == 0)
					{
						modem_buffer.clear();
						modem_buffer.push_back('A');modem_buffer.push_back('T');modem_buffer.push_back('+');modem_buffer.push_back('C');
						modem_buffer.push_back('E');modem_buffer.push_back('R');modem_buffer.push_back('R');modem_buffer.push_back('\r');
						current_modem->send(modem_buffer, true);
						modem_buffer.clear();
						current_modem->recv(modem_buffer);
						modem_buffer.push_back('\0');
						strMess<<" AT+CERR info: "<<&modem_buffer[0];
					};
					throw exception(strMess.str());
				}
			modem_buffer.clear();
			}; // else modem is still calling and we need to stop it

			modem_buffer.push_back('A');modem_buffer.push_back('T');modem_buffer.push_back('H');modem_buffer.push_back('\r');
			current_modem->send(modem_buffer, true);
			
			mysql_wrtr->ExecuteSQL("COMMIT");
			sleep(30); //wait for 30 seconds for ATH in modem
		} 
		catch (exception exc)
		{
			std::ostringstream strMess;
			strMess <<func_name.c_str()<< " ("<< pthread_self()<<") (DB transaction has been rolled back!) " << exc.what();
			logger::instance().log_message(exc.GetMessType(), strMess.str());
			mysql_wrtr->ExecuteSQL("ROLLBACK");
		}
		
		mysql_fact.release_thread_mySQL_writer();	
	} catch (exception exc) {
		std::ostringstream strErr;
		strErr<<func_name.c_str()<< " ("<< pthread_self()<<") get_thread_mySQL_writer returned "<< exc.what();
		logger::instance().log_message(ERROR, strErr.str());
	}
		
}

bool scheduled_action_read_data::find_controller_to_call(modem::modem_data_block& controller_phone_number,
														my_sql_writer* mysql_wrtr) throw (exception)
{
	const std::string FunctionName = "read_station_phone_number";
	std::vector<char> return_buffer;
	controller_phone_number.clear();
	
	std::string sql_query = "select FoneBS, Abonent from monitor where CountPoint > 0 limit 0,1;";
	if (configurator::instance().logging_options() & SQL_TO_READ)
	 {
			std::ostringstream strMess;
			strMess<<FunctionName.c_str()<< " ("<< pthread_self()<<") sql "
					<< sql_query.c_str();

			logger::instance().log_message(INFO, strMess.str());			
	 };

	mysql_wrtr->ExecuteSQL(sql_query);
	if (mysql_wrtr->NumRows()!= 1)
	{
		return false;
	};
	MYSQL_ROW row  = mysql_wrtr->FetchRow();
	if ((row[0] == NULL) ||
		(strlen(row[0])==0))
	{
		std::ostringstream strErr;
		strErr<<FunctionName.c_str()<< " ("<< pthread_self()<<") FoneBS value is empty ";
		throw exception(strErr.str(), ERROR );
	};
	
	int ret_length = strlen(row[0]);
	for (int i=0; i<ret_length; ++i){
		controller_phone_number.push_back(row[0][i]);
	}

	if ((row[1] == NULL) ||
		(strlen(row[1])==0))
	{
		std::ostringstream strErr;
		strErr<<FunctionName.c_str()<< " ("<< pthread_self()<<") Abonent value is empty ";
		throw exception(strErr.str(), ERROR );
	};

	sql_query = "update monitor set CountPoint = 0 where Abonent = ";
	sql_query += row[1];
	sql_query += " and CountPoint > 0;";
	if (configurator::instance().logging_options() & SQL_TO_WRITE)
	 {
			std::ostringstream strMess;
			strMess<<FunctionName.c_str()<< " ("<< pthread_self()<<") sql (under DB transaction) "
					<< sql_query.c_str();

			logger::instance().log_message(INFO, strMess.str());			
	 };
	mysql_wrtr->ExecuteSQL(sql_query);
	if (mysql_wrtr->NumRows()!= 1)
	{
		std::ostringstream strErr;
		strErr<<FunctionName.c_str()<< " ("<< pthread_self()<<") query to monitor "<<sql_query<<" affected "<< mysql_wrtr->NumRows()<<" rows";
		logger::instance().log_message(INFO, strErr.str());			
	};
	
	return true;
}

}

