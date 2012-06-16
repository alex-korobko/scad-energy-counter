using namespace std;

#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <string.h>
#include <iostream>

//for usleep
#include <unistd.h>

//for devctl
#include <termios.h>
#include <unistd.h>

//stl headers
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include <stdexcept>

//local headers
#include "common_declarations.h"
#include "configurator.h"
#include "logger.h"
#include "exception.h"
#include "modem.h"

namespace electricity_meter
{

modem::modem (
    	int  port_number,
        int baud_rate,
	    byte parity,
	    byte data_bits,
	    byte stop_bits,
	    unsigned int delay_useconds,
		unsigned int recieve_timeout_seconds,
        float modem_koeff)  throw (exception)  :
	    		m_port_handle(-1),
	    	    m_dev_port("/dev/ttyS"),
	    	    m_baud_rate(baud_rate),
		    m_parity(parity),
			m_data_bits(data_bits),
		    m_stop_bits(stop_bits),
		    m_delay_useconds(delay_useconds),
			m_recieve_timeout_seconds(recieve_timeout_seconds),
            m_modem_koeff(modem_koeff) {
	ostringstream exception_message;	     

	exception_message<<port_number;
	m_dev_port+=exception_message.str();

	exception_message.str("");
	if (port_number < 0 || port_number > 4) {
	 exception_message<<"Wrong com port number "<<m_dev_port;
	 throw exception(exception_message.str());
	};
	
	if ( ((!CBAUD) & m_baud_rate != 0 ) || 
		(m_baud_rate == 0)){ // 0 means that line is disabled - there is no use for that
		exception_message<<"Wrong baud rate value "<<baud_rate;
	    throw exception(exception_message.str());
    };

	if ( (m_data_bits!=DATA_BITS_5) && 
		 (m_data_bits!=DATA_BITS_6) &&
		 (m_data_bits!=DATA_BITS_7) &&
		 (m_data_bits!=DATA_BITS_8) ) {
		exception_message<<"Wrong data bits count "<<static_cast<int>(m_data_bits);
	    throw exception(exception_message.str());
    }

	if ((m_stop_bits!=STOP_BITS_1) &&
		(m_stop_bits!=STOP_BITS_2)) {
		exception_message<<"Wrong stop bits count "<<static_cast<int>(m_stop_bits);
	    throw exception(exception_message.str());
    }

	if ((m_parity!=PARITY_DISABLE) &&
		(m_parity!=PARITY_ODD) &&
		(m_parity!=PARITY_EVEN)) {
		exception_message<<"Wrong stop parity value "<<m_parity;
	    throw exception(exception_message.str());
	  }

};

modem::~modem() throw (exception){
	if (m_port_handle>0) {
	if (close(m_port_handle)<0) {
		std::ostringstream  exception_message;
		exception_message<<"Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		throw exception(exception_message.str());
		};
}
};

void modem::init() throw (exception){
	const std::string func_name = "modem::init "; 
	modem_data_block buffer;
	configurator& conf  = configurator::instance();
	std::string modem_answer;
	std::ostringstream  exception_message;
	struct termios termios_param;		  

	if ((m_port_handle = open(m_dev_port.c_str(), 
							O_RDWR | O_NOCTTY | O_NDELAY))<0) {
		exception_message<<"Can`t open com port "<<m_dev_port<<" : "<<strerror(errno);
		throw exception(exception_message.str());
	 }
	
	if (fcntl(m_port_handle, F_SETFL, 0) < 0) { //switching to blocking read
		exception_message<<"Call fcntl to com port "<<m_dev_port<<" failed: "<<strerror(errno);
		throw exception(exception_message.str());	
	}

	if (tcgetattr(m_port_handle, &termios_param)!=0) {
	 exception_message<<"Can`t get the current terminal control settings for "<<m_dev_port<<" : "<<strerror(errno);
	 if (close(m_port_handle)!=0)
	 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
	 m_port_handle=-1;
	 throw exception(exception_message.str());
	}

	if (cfsetispeed(&termios_param, m_baud_rate)!= 0){
	 exception_message<<"Can`t set input speed "<<m_dev_port<<" : "<<strerror(errno);
	 if (close(m_port_handle)!=0)
	 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
	 m_port_handle=-1;
	 throw exception(exception_message.str());
	}

	if (cfsetospeed(&termios_param, m_baud_rate) != 0){
	 exception_message<<"Can`t set output speed "<<m_dev_port<<" : "<<strerror(errno);
	 if (close(m_port_handle)!=0)
	 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
	 m_port_handle=-1;
	 throw exception(exception_message.str());
	}

    termios_param.c_lflag &= ~( ECHO|ECHOE|ECHOK|ECHOKE|ECHOCTL|ECHONL|ICANON|ISIG);
	termios_param.c_cflag |= (CLOCAL | CREAD|CRTSCTS);
	termios_param.c_oflag &= ~ OPOST;
	termios_param.c_cc[VMIN] = 0;
	termios_param.c_cc[VTIME] = m_recieve_timeout_seconds*10;
	
	if (m_data_bits==DATA_BITS_5) {
			termios_param.c_cflag |= CS5;
		} else if ( m_data_bits==DATA_BITS_6) {
			termios_param.c_cflag |= CS6;	
		} else if (m_data_bits==DATA_BITS_7) {
			termios_param.c_cflag |=CS7;
		} else if (m_data_bits==DATA_BITS_8) {
			termios_param.c_cflag |= CS8;
		}else  {
		exception_message<<"Wrong data bits count "<<static_cast<int>(m_data_bits);
	 	 if (close(m_port_handle)!=0)
		 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		 m_port_handle=-1;
	    throw exception(exception_message.str());
    }

	if (m_stop_bits==STOP_BITS_1) {
			termios_param.c_cflag &= ~CSTOPB;
  	    } else if (m_stop_bits==STOP_BITS_2 ) {
			termios_param.c_cflag |= CSTOPB;
  	    } else {
		exception_message<<"Wrong stop bits count "<<static_cast<int>(m_stop_bits);
	 	 if (close(m_port_handle)!=0)
		 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		 m_port_handle=-1;
	    throw exception(exception_message.str());
    }

	if (m_parity==PARITY_DISABLE) {
			termios_param.c_cflag &= ~PARENB ;
  	    } else if (m_parity==PARITY_ODD) {
 			termios_param.c_cflag |=(PARENB | PARODD);
  	    } else if (m_parity==PARITY_EVEN) {
		  	termios_param.c_cflag |=(PARENB | ~PARODD);
  	    } else {
		exception_message<<"Wrong stop parity value "<<m_parity;
 		 if (close(m_port_handle)!=0)
	 		 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		 m_port_handle=-1;
	    throw exception(exception_message.str());
	  }
	
	if (tcsetattr(m_port_handle, TCSANOW , &termios_param)!=0){
	 exception_message<<"Can`t set the current terminal control settings for "<<m_dev_port<<" : "<<strerror(errno);
	 if (close(m_port_handle)!=0)
	 	 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
	 m_port_handle=-1;
	 throw exception(exception_message.str());
	}

    if (tcflush(m_port_handle, TCIOFLUSH)<0) {
		exception_message<<"Can`t flush io streams "<<strerror(errno);
		 if (close(m_port_handle)!=0)
	 		 exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
		m_port_handle =-1;		
		throw exception(exception_message.str());
    };

	buffer.push_back('A');buffer.push_back('T');buffer.push_back('\r');

	if (conf.logging_options() & MODEM)
	{
		std::string strMess = func_name;
		modem_data_block tmp_buffer(buffer);
		tmp_buffer.push_back('\0');
		strMess += &tmp_buffer[0];
		logger::instance().log_message(INFO, strMess.c_str());			
	}

	this->send(buffer, true);
	buffer.clear();
	this->recv(buffer);

	buffer.push_back('\0');
	modem_answer = &buffer[0];
	if (conf.logging_options() & MODEM)
	{
		std::string strMess = func_name;
		strMess += modem_answer;
		logger::instance().log_message(INFO, strMess.c_str());			
	}

	if (modem_answer.length() == 0)
	{
		ostringstream exception_message;
		exception_message<<"No answer on command AT from modem on "<<m_dev_port;
		throw exception(exception_message.str());				
	} else if ( (modem_answer.compare("\n\nOK\n\n") != 0) && 
					(modem_answer.compare("0\n") != 0)) { //if modem already in ATV0 it returns 0
		ostringstream exception_message;
		exception_message<<"Unrecognized answer on command AT from modem on "<<m_dev_port<<" : '"<<modem_answer.c_str()<<"'";
		throw exception(exception_message.str());		
	}
	
}

void modem::send(modem_data_block data_to_send, 
								bool flush_io_buffers_after_send) throw (exception){
	static int  delay_messages_counter = 0;
	ostringstream exception_message;
	int count_of_sended_bytes;
	const byte parity_bits_delay_koeff=3; //delay for parity bits, so on $-)
	useconds_t echo_interval;

	//impossible
	if (m_port_handle < 0) {
		exception_message<<"Port "<<m_dev_port<<" not initialized";
		throw exception(exception_message.str());
	};

	count_of_sended_bytes=write(m_port_handle,
								&data_to_send[0],
                                data_to_send.size());

	if (static_cast<modem_data_block::size_type >(count_of_sended_bytes)!=data_to_send.size()) {
		exception_message<<"Can`t  write "<<data_to_send.size()<<" bytes to "<<m_dev_port;
		if (count_of_sended_bytes<0) 
				exception_message<<" : "<<strerror(errno);
		throw exception(exception_message.str());
	};

  if (m_delay_useconds==0) {
	   echo_interval=  static_cast<useconds_t> (
                                  (m_data_bits+m_stop_bits+parity_bits_delay_koeff)*1000000
                                  *data_to_send.size()*m_modem_koeff/m_baud_rate
                                   );
	} else {
		echo_interval=m_delay_useconds;
	};

if (delay_messages_counter >64) {
	delay_messages_counter = 0;
   ostringstream description;
   description<<"Curr delay is " <<echo_interval<<" mkS predef delay is "<<m_delay_useconds<<" mkS";
   logger::instance().log_message(INFO, description.str());
};
	delay_messages_counter++;

	usleep(echo_interval);
    if (flush_io_buffers_after_send) {
	     if (tcflush(m_port_handle, TCIOFLUSH)<0) {
			exception_message<<"Can`t flush input and output streams "<<strerror(errno);
			 if (close(m_port_handle)!=0)
 					exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
             m_port_handle =-1;		
			throw exception(exception_message.str());
    };
}; // if (flush_io_buffers_after_send)

};


int  modem::recv(modem::modem_data_block &buffer_to_recieve,
	                 bool flush_io_buffers) 
					 throw (exception)
{
	ostringstream exception_message;	     
	modem_character read_char;

	//impossible
	if (m_port_handle < 0) {
		exception_message<<"Port "<<m_dev_port<<" not initialized";
		throw exception(exception_message.str());
	};

    if (flush_io_buffers) {
		 if (tcflush(m_port_handle, TCIOFLUSH)<0) {
		exception_message<<"Can`t flush input and output streams "<<strerror(errno);
		 if (close(m_port_handle)!=0)
			exception_message<<"  Can`t close "<<m_dev_port<<" : "<<strerror(errno);
					m_port_handle =-1;		
			throw exception(exception_message.str());
		};
  }; // if (flush_io_buffers) {

  while( read(m_port_handle, &read_char,1) > 0) {
		buffer_to_recieve.push_back(read_char);
		usleep(10000); //sleep for 10mS until new data arrives
    };
	
    return buffer_to_recieve.size();
};    

};