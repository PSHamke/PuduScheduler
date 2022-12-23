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
	virtual std::vector<uint8_t>& GetComparatorList(const uint8_t queueIndex) = 0;
	virtual std::vector<SchedulerQueueFeatures>& GetQueueFeatures() = 0;
	virtual std::reference_wrapper<uint32_t> GetStartTimeRef() = 0;
	virtual std::reference_wrapper<uint32_t> GetQuantumRef(const uint8_t queueIndex) = 0;
	virtual const uint8_t GetId() = 0;
	virtual void Schedule() = 0;
	virtual void SubmitProcessPool(std::vector<Process>& processes) = 0;
	virtual void ClearPreviousData() = 0;
	virtual bool IsReadyToGrabResult() const = 0;
	virtual std::vector<ProcessChart>& GetProcessChart() = 0;
	virtual std::vector<SchedulingMetrics>& GetSchedulingMetrics() = 0;
	virtual const float GetUtilization() = 0;
	virtual const float GetThroughput() = 0;
	virtual const float GetAVGTurnaroundTime() = 0;
	virtual const float GetAVGResponseTime() = 0;
	virtual const float GetAVGWaitingTime() = 0;

protected:
};