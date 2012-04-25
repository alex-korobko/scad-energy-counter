#ifndef PACKETS_PROCESSOR_H
#define PACKETS_PROCESSOR_H

namespace electricity_meter
{

class packets_processor {
private:
	packets_processor(const packets_processor& rhs);
	packets_processor& operator=(const packets_processor& rhs);
	packets_processor();
	~packets_processor();

public:
	static packets_processor& Instance();
};

}

#endif // PACKETS_PROCESSOR_H
