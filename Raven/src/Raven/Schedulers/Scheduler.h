#pragma once
#include "../Process/Process.h"
#include "SchedulerSpecification.h"
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
	virtual void Init(const SchedulerSpecification& specification) = 0;
	virtual std::vector<uint8_t>& GetComparatorList() = 0;
	virtual std::reference_wrapper<uint32_t> GetStartTimeRef() = 0;
	virtual std::reference_wrapper<uint32_t> GetQuantumRef() = 0;
	virtual const uint8_t GetId() = 0;
	virtual void Schedule() = 0;
	virtual void SubmitProcessPool(std::vector<Process>& processes) = 0;
	virtual void ClearPreviousData() = 0;
	virtual bool IsReadyToGrabResult() const = 0;
	virtual std::vector<ProcessChart>& GetProcessChart() = 0;
protected:

};