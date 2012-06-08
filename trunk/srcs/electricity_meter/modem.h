#ifndef __MODEM__
#define __MODEM__

namespace electricity_meter
{

class modem {
public:
    typedef std::vector<modem_character> modem_data_block;
private:

    int m_port_handle;
    std::string m_dev_port;
    int m_baud_rate;
    byte m_parity;
    byte m_data_bits;
    byte m_stop_bits;
    unsigned int m_delay_useconds;
    unsigned int m_recieve_timeout_seconds;
    float m_modem_koeff;
public:

    modem (
		int port_number,
        int baud_rate,
	    byte parity,
	    byte data_bits,
	    byte stop_bits,
	    unsigned int delay_useconds=0,
		unsigned int recieve_timeout_seconds = 1,
        float modem_koeff=1.0)  throw (exception);

    virtual ~modem() throw (exception);

	void init() throw (exception);

    void send(modem_data_block data_to_send , 
					bool flush_io_buffers_after_send = false) throw (exception);

    int recv( modem_data_block &buffer_to_recieve,
                   bool flush_io_buffers=false) throw (exception);
};

}

#endif
