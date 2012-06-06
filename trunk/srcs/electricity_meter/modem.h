#ifndef __MODEM__
#define __MODEM__

namespace electricity_meter
{

class modem {
private:
    typedef vector<byte> modem_data_block;

    int m_port_handle;
    string m_dev_port;
    int m_baud_rate;
    byte m_parity;
    byte m_data_bits;
    byte m_stop_bits;
    int m_delay_useconds;
    float m_modem_koeff;
public:

    modem (
		int port_number,
        int baud_rate,
	    byte parity,
	    byte data_bits,
	    byte stop_bits,
	    int delay_useconds=0,
        float modem_koeff=1.0)  throw (modem_exception);

    virtual ~modem() throw (modem_exception);

	void init() throw (modem_exception);

    void send(modem_data_block data_to_send , 
					bool flush_io_buffers_after_send = false) throw (modem_exception);

    int recv( modem::modem_data_block &buffer_to_recieve,
                   int bytes_count,
                   bool flush_io_buffers=false,
				   unsigned int recieve_timeout = 10) throw (modem_exception);

        enum{ BAUD_9600=B9600,
              BAUD_19200=B19200,
              BAUD_115200=B115200};

        enum { STOP_BITS_1=1,
               STOP_BITS_2,
               DATA_BITS_5=5,
               DATA_BITS_6,
               DATA_BITS_7, 
               DATA_BITS_8,
               PARITY_DISABLE, 
               PARITY_ODD,
               PARITY_EVEN,
               RTS_SET_1,
               RTS_SET_0};
};

}

#endif
