#ifndef RECIEVER_H
#define RECIEVER_H

namespace electricity_meter
{

class server_socket;
	
class reciever {

public:
	static reciever& Instance();

	void StartRecieve(uint64_t PortNumber,
						unsigned int SendRecvTimeout,
						unsigned short MaxConnectQueue);
	void StopRecieve();
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
