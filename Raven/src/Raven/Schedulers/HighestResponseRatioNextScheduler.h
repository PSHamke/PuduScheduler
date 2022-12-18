#pragma once
#include "Scheduler.h"
#include <deque>


class HighestResponseRatioNextScheduler : public Scheduler
{
public:
	HighestResponseRatioNextScheduler() : m_Initialized(false), m_ScheduledProcess(nullptr) {}

	virtual void Init(const SchedulerSpecification& specification) override
	{
		m_SchedulerId = specification.m_Id;
		m_StartTime = specification.m_StartTime;
		m_TimeStamp = specification.m_StartTime;
		m_ScheduledProcess = nullptr;
		m_CompareOrder = specification.m_CompareOrder;
		m_RequirePreemption = true;
		m_UnitTime = 1;
		m_Prop = specification.m_Prop;
		m_Initialized = true;
		m_Result = false;
	}

	inline const bool IsInitialized() const
	{
		return m_Initialized;
	}

	std::vector<uint8_t>& GetComparatorList() override
	{
		return m_CompareOrder;
	}

	virtual std::reference_wrapper<uint32_t> GetStartTimeRef() override
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

			if (m_RequirePreemption && !m_ReadyQueue.empty())
			{
				PickToSchedule();
			}
			//PrintReadyQueue();
			if (m_ScheduledProcess != nullptr)
				Progress();

			m_TimeStamp += m_UnitTime;

		}
		m_Result = true;
	}


	/*
		std::string m_Label;
	int32_t m_Usage;
	int32_t m_StartTime;
	//ImU32 m_Color;
	CompleteReason m_Reason;
	*/
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


		m_ScheduledProcess->Burst(m_UnitTime, m_TimeStamp);

		if (m_ScheduledProcess->GetRemainingTime() == 0)
		{
			m_ScheduledProcess->UpdateStatus(m_TimeStamp + 1, Process::ProcessStatus::P_SCHEDULED);
			m_ScheduledProcess = nullptr;
			m_RequirePreemption = true;
			--m_UnProcessedCount;
		}

	}

	virtual void SubmitProcessPool(std::vector<Process>& processes) override
	{

		for (auto& it : processes)
			m_ProcessPool.push_back(&it);

		std::sort(m_ProcessPool.begin(), m_ProcessPool.end(), c_ArrivalComparator);
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
	virtual void FillReadyQueue()
	{
		bool alreadyQueued = false;
		for (auto& process : m_ProcessPool)
		{
			if (process->IsWaitingToSubmit()) {
				m_ReadyQueue.push_back(process);
				process->UpdateStatus(m_TimeStamp, Process::ProcessStatus::P_WAITING_IN_READY_QUEUE);
			}
		}

		if (m_Prop == SchedulerProp::S_PREEMPTIVE)
		{
			for (auto& process : m_ReadyQueue)
			{
				if (m_ScheduledProcess != nullptr && !alreadyQueued)
				{
					if (SchedulingCriterias(m_CompareOrder, m_TimeStamp)(process, m_ScheduledProcess))
					{
						m_RequirePreemption = true;
						m_ReadyQueue.push_back(m_ScheduledProcess);
						alreadyQueued = true;
					}
				}
			}
		}

	}

	virtual void ProgressTick()
	{

		for (auto& process : m_ProcessPool)
			process->CheckAndUpdateStatus(m_TimeStamp, Process::ProcessStatus::P_WAITING_TO_SUBMIT);
		FillReadyQueue();
	}

	void PickToSchedule()
	{
		std::sort(m_ReadyQueue.begin(), m_ReadyQueue.end(), SchedulingCriterias(m_CompareOrder,m_TimeStamp));
		Process* nextScheduledProcess = m_ReadyQueue.front();

		m_ReadyQueue.pop_front();
		if (m_ScheduledProcess != nullptr && m_ScheduledProcess != nextScheduledProcess)
		{
			m_ScheduledProcess->UpdateStatus(m_TimeStamp, Process::ProcessStatus::P_PREEMPTED);
		}
		m_ScheduledProcess = nextScheduledProcess;
		m_RequirePreemption = false;
	}

	void PrintReadyQueue() const
	{
		std::cout << "Ready Queue at TS:" << m_TimeStamp << ": {";
		const char* prefix = "";
		for (const auto& it : m_ReadyQueue)
		{
			std::cout << prefix << "p_" << it->GetProcessId();
			prefix = ", ";
		}
		std::cout << "}";

		if (m_ScheduledProcess)
			std::cout << " Current Scheduled Process: p_" << m_ScheduledProcess->GetProcessId();
		else
			std::cout << " No current scheduled process.";

		std::cout << "\n";
	}
private:
	uint32_t m_SchedulerId;
	uint32_t m_StartTime;
	uint32_t m_TimeStamp; // Might use to keep actions as well ? 
	uint32_t m_UnitTime;
	SchedulerProp m_Prop;
	bool m_Initialized = false;
	std::vector<Process*> m_ProcessPool;
	uint32_t m_UnProcessedCount;
	std::deque<Process*> m_ReadyQueue;
	Process* m_ScheduledProcess;
	std::vector<uint8_t> m_CompareOrder;
	std::vector<ProcessChart> m_ProcessCharts;
	bool m_RequirePreemption;
	bool m_Result;
};