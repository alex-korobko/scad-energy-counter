#ifndef EXCEPTION_H
#define EXCEPTION_H


namespace electricity_meter
{

class exception : public std::runtime_error {
private:
	MessType mMessageType;
public:
	exception(std::string ErrMessage, MessType MessageType = ERROR);
	MessType GetMessType() const {return mMessageType;};
	virtual ~exception() throw();

};

}

#endif // EXCEPTION_H
