#pragma once
#include "../Process/Process.h"
#include "SchedulerSpec.h"
#include "SchedulingCriterias.h"
class Scheduler
{
private:
public:

	struct {
		bool operator() (Process* lhs, Process* rhs) const
		{
			return lhs->GetArrivalTime() < rhs->GetArrivalTime();
		}
	} c_ArrivalComparator;

	Scheduler() = default;
	virtual ~Scheduler() {};
	virtual void Schedule() = 0;
	virtual void SubmitProcessPool(std::vector<Process>& processes) = 0;
protected:

};