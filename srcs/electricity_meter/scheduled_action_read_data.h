#ifndef __SCHEDULED_ACTION_READ_DATA__
#define __SCHEDULED_ACTION_READ_DATA__

namespace electricity_meter
{

class scheduled_action_read_data : public scheduled_action 
{
private:
	scheduled_action_read_data(const scheduled_action_read_data& rhs);
	scheduled_action_read_data& operator=(const scheduled_action_read_data& rhs);
	
	virtual void invoce();
public:
	scheduled_action_read_data(double interval_between_invocations);
	virtual ~scheduled_action_read_data();
};

};

#endif // __SCHEDULED_ACTION_READ_DATA__
