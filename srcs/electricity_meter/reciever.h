#ifndef RECIEVER_H
#define RECIEVER_H

namespace electricity_meter
{

class server_socket;
	
class reciever {

public:
	static reciever& instance();

	void start_recieve(uint64_t port_number,
						unsigned int send_recv_timeout,
						unsigned short max_connect_queue);
	void stop_recieve();
private:
	bool mStopRecieveFlag;
	std::auto_ptr<server_socket> mSocket;

	reciever();
	~reciever();
	reciever(const reciever& rhs);
	reciever& operator=(const reciever& rhs);
};

}

#endif // RECIEVER_H
