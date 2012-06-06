#ifndef __MODEM_EXCEPTION__
#define __MODEM_EXCEPTION__

namespace electricity_meter
{

class modem_exception {
private:
    string modem_exc_descr;

public:
    explicit modem_exception (string description) : 
	    modem_exc_descr (description) {};

    string get_description () {return modem_exc_descr;};
};

}
#endif
