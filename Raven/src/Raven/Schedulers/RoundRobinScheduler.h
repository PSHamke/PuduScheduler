#pragma once
#pragma once
#include "Scheduler.h"
#include <deque>
// Non-Preemptive while some process executing in its quantum
// TODO: Preemptive even some process executing in its quantum according to some criterias 
// Remaining Time ? Priority

/*
	Types of RR scheduling:
	Non-Preemptive while some process executing in its quantum
	Preemptive even some process executing in its quantum
*/
class RoundRobinScheduler : public Scheduler
{
public:
	RoundRobinScheduler(std::vector<uint8_t>& compareOrder, uint32_t quantum)
	{
		m_TimeStamp = 0;
		m_ScheduledProcess = nullptr;
		m_RequirePreemption = true;
		m_UnitTime = 1;
		m_Quantum = quantum;
		m_CompareOrder = compareOrder;
		m_Spec = SchedulerSpec::S_NON_PREEMPTIVE;

	}
	RoundRobinScheduler(std::vector<uint8_t>& compareOrder, std::vector<uint8_t>& preemptionOrder, uint32_t quantum)
	{
		m_TimeStamp = 0;
		m_ScheduledProcess = nullptr;
		m_RequirePreemption = true;
		m_UnitTime = 1;
		m_Quantum = quantum;
		m_CompareOrder = compareOrder;
		m_Spec = SchedulerSpec::S_PREEMPTIVE;
		m_PreemptionOrder = preemptionOrder;

	}
	// TODO: Arrange Unit Time ?
	virtual void Schedule() override
	{
		
		while (m_UnProcessedCount > 0)
		{
			ProgressTick();
			if (m_RequirePreemption && !m_ReadyQueue.empty())
			{
				PickToSchedule();
				
			}

			if(m_ScheduledProcess != nullptr)
				Progress();
			m_TimeStamp += m_UnitTime;
		}
	}

	virtual void Progress()
	{
		
		m_ScheduledProcess->Burst(1, m_TimeStamp);
		m_ScheduledProcess->UseQuantum(1);

		if (m_ScheduledProcess->GetRemainingTime() == 0)
		{
			m_ScheduledProcess->UpdateStatus(m_TimeStamp + 1, Process::ProcessStatus::P_SCHEDULED);
			m_ScheduledProcess = nullptr;
			m_RequirePreemption = true;
			--m_UnProcessedCount;
		}
		else if (m_ScheduledProcess->GetQuantumUsage() % m_Quantum == 0 && !m_ReadyQueue.empty())
		{
			m_ScheduledProcess->UpdateStatus(m_TimeStamp + 1, Process::ProcessStatus::P_PREEMPTED);
			m_ReadyQueue.push_back(m_ScheduledProcess);
			m_ScheduledProcess = nullptr;
			m_RequirePreemption = true;
		}
			
		

	}

	virtual void SubmitProcessPool(std::vector<Process>& processes) override
	{

		for (auto& it : processes)
			m_ProcessPool.push_back(&it);

		std::sort(m_ProcessPool.begin(), m_ProcessPool.end(), c_ArrivalComparator);
		m_UnProcessedCount = m_ProcessPool.size();

	}

	virtual void FillReadyQueue()
	{
		bool alreadyQueued = false;
		for (auto& process : m_ProcessPool)
		{
			if (process->IsWaitingToSubmit()) {
				m_IntermediateQueue.push_back(process);
				process->UpdateStatus(m_TimeStamp, Process::ProcessStatus::P_WAITING_IN_READY_QUEUE);
				
				if (m_Spec == SchedulerSpec::S_PREEMPTIVE && m_ScheduledProcess != nullptr && !alreadyQueued)
				{
					if (SchedulingCriterias(m_PreemptionOrder, m_TimeStamp)( process, m_ScheduledProcess))
					{
						m_RequirePreemption = true;
						m_IntermediateQueue.push_back(m_ScheduledProcess);
						alreadyQueued = true;
					}
				}

			}
		}
		std::sort(m_IntermediateQueue.begin(), m_IntermediateQueue.end(), SchedulingCriterias(m_CompareOrder,m_TimeStamp));
		for (auto& it : m_IntermediateQueue)
		{
			m_ReadyQueue.push_back(m_IntermediateQueue.front());
			m_IntermediateQueue.pop_front();

		}
	}

	virtual void ProgressTick()
	{

		for (auto& proc : m_ProcessPool)
			proc->CheckAndUpdateStatus(m_TimeStamp, Process::ProcessStatus::P_WAITING_TO_SUBMIT);
		FillReadyQueue();
	}

	void PickToSchedule()
	{
		//std::sort(m_ReadyQueue.begin(), m_ReadyQueue.end(), SchedulingCriterias(m_CompareOrder));
		Process* nextScheduledProcess = m_ReadyQueue.front();

		m_ReadyQueue.pop_front();
		if (m_ScheduledProcess != nullptr && m_ScheduledProcess != nextScheduledProcess)
		{
			m_ScheduledProcess->UpdateStatus(m_TimeStamp, Process::ProcessStatus::P_PREEMPTED);
		}
		m_ScheduledProcess = nextScheduledProcess;
		m_RequirePreemption = false;
	}

private:
	uint32_t m_SchedulerId;
	uint32_t m_TimeStamp; // Might use to keep actions as well ? 
	uint32_t m_UnitTime;
	SchedulerSpec m_Spec;
	std::vector<Process*> m_ProcessPool;
	uint32_t m_UnProcessedCount;
	std::deque<Process*> m_ReadyQueue;
	std::deque<Process*> m_IntermediateQueue;
	Process* m_ScheduledProcess;
	std::vector<uint8_t> m_CompareOrder;
	bool m_RequirePreemption;
	uint32_t m_Quantum;
	std::vector<uint8_t> m_PreemptionOrder;
	
};