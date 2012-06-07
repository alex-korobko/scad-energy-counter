#ifndef __SCHEDULED_ACTIONS_CLASS__
#define __SCHEDULED_ACTIONS_CLASS__

namespace electricity_meter
{
//the class exectues 
//actions on periodical basis
class scheduled_actions{
public:
	static scheduled_actions& instance();
	void start_actions();
	void stop_actions();
private:
	typedef std::vector<scheduled_action> actions_container_type;
	scheduled_actions();
	~scheduled_actions();

	scheduled_actions(const scheduled_actions& schact);
	scheduled_actions& operator=(const scheduled_actions& schact);

	static void* scheduled_actions_thread_function(void* attr);

	void thread_function();

	actions_container_type m_actions;
	bool m_stop_flag;
};

}
#endif