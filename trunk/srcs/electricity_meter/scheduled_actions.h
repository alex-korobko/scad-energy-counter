#ifndef __SCHEDULED_ACTIONS_CLASS__
#define __SCHEDULED_ACTIONS_CLASS__

namespace electricity_meter
{
//definition of a class that exectues 
//actions on periodical basis
class scheduled_actions{
public:
	static scheduled_actions& Instance();

	void StartActions(vector<scheduled_action> Actions);
	void StopActions();
private:
	scheduled_actions();
	~scheduled_actions();
	scheduled_actions(const scheduled_actions& schact);
	scheduled_actions& operator=(const scheduled_actions& schact);
}

}
#endif