#ifndef __MODEM_EXCEPTION__
#define __MODEM_EXCEPTION__

namespace electricity_meter
{

class modem_exception {
private:
    std::string modem_exc_descr;

public:
    explicit modem_exception (std::string description) : 
	    modem_exc_descr (description) {};

    std::string get_description () {return modem_exc_descr;};
};

}
#endif
