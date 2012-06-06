#ifndef __SCHEDULED_ACTION__
#define __SCHEDULED_ACTION__

namespace electricity_meter
{

class scheduled_action {
	double m_interval_between_invoc;
	time_t m_time_of_prev_invoc;

protected:
	virtual void invoce() = 0;
public:
	scheduled_action(double interval_between_invocations);
	virtual ~scheduled_action();
	
	void try_invoce();
};

}

#endif // __SCHEDULED_ACTION__
