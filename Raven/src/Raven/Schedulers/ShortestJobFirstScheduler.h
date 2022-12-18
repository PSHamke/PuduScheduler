#pragma once
#include "Scheduler.h"
#include <deque>
#include  <functional>
#include "Log/Log.h"

class ShortestJobFirstScheduler : public Scheduler
{
public:
	ShortestJobFirstScheduler() : m_Initialized(false), m_ScheduledProcess(nullptr){}

	virtual void Init(const SchedulerSpecification& specification) override
	{
		m_SchedulerId = specification.m_Id;
		m_StartTime = specification.m_StartTime;
		m_TimeStamp = specification.m_StartTime;
		m_UnitTime = 1;
		m_CompareOrder = specification.m_CompareOrder;
		m_Spec = specification.m_Prop;
		m_Initialized = true;
		m_Result = false;
	}

	inline const bool IsInitialized() const
	{
		return m_Initialized;
	}

	virtual std::vector<uint8_t>& GetComparatorList() override
	{
		return m_CompareOrder;
	}
	virtual std::reference_wrapper<uint32_t>GetStartTimeRef() override
	{
		return std::ref(m_StartTime);
	}
	virtual std::reference_wrapper<uint32_t> GetQuantumRef() override
	{
		return std::ref(m_StartTime);
	}
	virtual const uint8_t GetId()
	{
		return m_SchedulerId;
	}
	virtual bool IsReadyToGrabResult() const override
	{
		return m_Result;
	}
	virtual std::vector<ProcessChart>& GetProcessChart() override
	{
		return m_ProcessCharts;
	}
public:
	virtual void Schedule() override
	{

		while (m_UnProcessedCount > 0)
		{
			ProgressTick();
			if (!m_ReadyQueue.empty() && m_ScheduledProcess == nullptr) // Are there any process waiting in ready queue? But do not preempt existing process.
			{
				PickToSchedule();
			}

			if (m_ScheduledProcess != nullptr)
				Progress();
	
			m_TimeStamp+=m_UnitTime;
		}
		m_Result = true;
	}


	// Burst 1 unit time;
	virtual void Progress()
	{
		if (!m_ProcessCharts.empty())
		{
			auto& previous = m_ProcessCharts.back();
			if (previous.m_Label == m_ScheduledProcess->GetProcessLabel())
			{
				previous.m_Usage += 1;
			}
			else
			{
				ProcessChart temp{ m_ScheduledProcess->GetProcessLabel(),1,m_TimeStamp };
				m_ProcessCharts.push_back(temp);
			}
		}
		else
		{
			ProcessChart temp{ m_ScheduledProcess->GetProcessLabel(),1,m_TimeStamp };
			m_ProcessCharts.push_back(temp);
		}

		m_ScheduledProcess->Burst(m_UnitTime,m_TimeStamp);
		if (m_ScheduledProcess->GetRemainingTime() == 0)
		{
			m_ScheduledProcess->UpdateStatus(m_TimeStamp+1,Process::ProcessStatus::P_SCHEDULED);
			m_ScheduledProcess = nullptr;
			--m_UnProcessedCount;
		}
		
	}

	virtual void SubmitProcessPool(std::vector<Process>& processes) override // Fill pool according to arrival of processes
	{
		
		for (auto& it : processes)
			m_ProcessPool.push_back(&it);

		std::sort(m_ProcessPool.begin(), m_ProcessPool.end(), c_ArrivalComparator); // Less arrival time will be in the lower index (Back of the vector).
		m_UnProcessedCount = m_ProcessPool.size();

	}

	virtual void ClearPreviousData()
	{
		m_ProcessPool.clear();
		m_ReadyQueue.clear();
		m_TimeStamp = 0;
		m_ScheduledProcess = nullptr;
		m_ProcessCharts.clear();
		m_UnProcessedCount = 0;
	}
	// When process become available push_back (enqueue) to ready queue 
	// such a way that processes are sorted respect to scheduling criteria.
	virtual void FillReadyQueue() 
	{
		for (auto& procces : m_ProcessPool)
		{
			if (procces->IsWaitingToSubmit()) {
				m_ReadyQueue.push_back(procces);
				procces->UpdateStatus(m_TimeStamp,Process::ProcessStatus::P_WAITING_IN_READY_QUEUE);
				std::sort(m_ReadyQueue.begin(), m_ReadyQueue.end(), SchedulingCriterias(m_CompareOrder, m_TimeStamp));
			}
		}

	}

	// Check time stamp if process is ready, then change its status from unavailable to waiting to submit
	// Then fill ready queue
	virtual void ProgressTick()
	{
		for (auto& proc : m_ProcessPool)
			proc->CheckAndUpdateStatus(m_TimeStamp,Process::ProcessStatus::P_WAITING_TO_SUBMIT); 

		FillReadyQueue();
	}

	void PickToSchedule() // Non-Preemptive scheduling pop_front (dequeue) a process from ready queue 
	{
		
		m_ScheduledProcess = m_ReadyQueue.front();
		m_ReadyQueue.pop_front();
		m_ScheduledProcess->UpdateStatus(m_TimeStamp, Process::ProcessStatus::P_PROCESSING);
		
	}


private:
	uint32_t m_SchedulerId;
	uint32_t m_StartTime;
	uint32_t m_TimeStamp; // Might use to keep actions as well ? 
	uint32_t m_UnitTime;
	SchedulerProp m_Spec;
	bool m_Initialized;
	std::vector<Process*> m_ProcessPool;
	uint32_t m_UnProcessedCount;
	std::deque<Process*> m_ReadyQueue;
	Process* m_ScheduledProcess;
	std::vector<uint8_t> m_CompareOrder;
	std::vector<ProcessChart> m_ProcessCharts;
	bool m_Result;
};