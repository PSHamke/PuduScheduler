#pragma once
#include "Scheduler.h"
#include <deque>
#include <cmath>


class CompletelyFairScheduler : public Scheduler
{
public:
	CompletelyFairScheduler() : m_Initialized(false), m_ScheduledProcess(nullptr) {}

	virtual void Init(const SchedulerSpecification& specification) override
	{
		m_SchedulerId = specification.m_Id;
		m_StartTime = specification.m_StartTime;
		m_TimeStamp = specification.m_StartTime;
		m_ScheduledProcess = nullptr;
		m_SchedulingQueues = specification.m_QueueFeatures;
		m_CompareOrder = specification.m_QueueFeatures[0].m_CompareOrder;
		m_RequirePreemption = true;
		m_UnitTime = 1;
		m_Prop = specification.m_QueueFeatures[0].m_Prop;
		m_Initialized = true;
		m_Result = false;
		m_Throughput = 0;
		m_Utilization = 0;
		m_TotalExecution = 0;
		m_AVGResponseTime = 0.0f;
		m_AVGWaitingTime = 0.0f;
		m_AVGTurnaroundTime = 0.0f;
	}

	inline const bool IsInitialized() const
	{
		return m_Initialized;
	}

	std::vector<uint8_t>& GetComparatorList(uint8_t /*DUMMY*/) override
	{
		return m_CompareOrder;
	}

	std::vector<SchedulerQueueFeatures>& GetQueueFeatures() override
	{
		return m_SchedulingQueues;
	}

	virtual std::reference_wrapper<uint32_t> GetStartTimeRef() override
	{
		return std::ref(m_StartTime);
	}
	virtual std::reference_wrapper<uint32_t> GetQuantumRef(uint8_t queueIndex) override
	{
		return std::ref(m_SchedulingQueues[queueIndex].m_Quantum);
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
	virtual std::vector<SchedulingMetrics>& GetSchedulingMetrics() override
	{
		return m_SchedulingMetrics;
	}
	virtual const float GetUtilization() override
	{
		return m_Utilization;
	}
	virtual const float GetThroughput() override
	{
		return m_Throughput;
	}

	virtual const float GetAVGTurnaroundTime() override
	{
		return m_AVGTurnaroundTime;
	}
	virtual const float GetAVGResponseTime() override
	{
		return m_AVGResponseTime;
	}
	virtual const float GetAVGWaitingTime() override
	{
		return m_AVGWaitingTime;
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

		for (const auto& it : m_ProcessPool)
		{
			uint32_t start = it->GetSchedulingStartTime();
			uint32_t end = it->GetSchedulingEndTime();
			SchedulingMetrics temp{};
			temp.m_Label = it->GetProcessLabel();
			temp.m_ResponseTime = it->GetFirstResponseTime() - it->GetArrivalTime();
			temp.m_TurnaroundTime = end - start;
			temp.m_WaitingTime = end - start - it->GetBurstTime();
			m_AVGResponseTime += temp.m_ResponseTime;
			m_AVGTurnaroundTime += temp.m_TurnaroundTime;
			m_AVGWaitingTime += temp.m_WaitingTime;
			m_SchedulingMetrics.push_back(temp);
		}
		m_AVGResponseTime /= m_ProcessPool.size();
		m_AVGTurnaroundTime /= m_ProcessPool.size();
		m_AVGWaitingTime /= m_ProcessPool.size();

		m_Utilization = (m_TimeStamp - m_TotalExecution) / (float)m_TimeStamp;
		m_Throughput = m_ProcessPool.size() / (float)m_TimeStamp;
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
		{ /* Process Chart */
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

		}

		uint32_t quantumShare = m_SchedulingQueues[0].m_Quantum / (m_ReadyQueue.size()+1) + 1;

		m_ScheduledProcess->Burst(1, m_TimeStamp);
		m_TotalExecution++;
		m_ScheduledProcess->UseQuantum(1);
		m_ScheduledProcess->SetNextVRunTime(m_ScheduledProcess->GetNextVRunTime() + std::pow(1.25f, m_ScheduledProcess->GetNiceValue()));

		if (m_ScheduledProcess->GetRemainingTime() == 0)
		{
			m_ScheduledProcess->UpdateStatus(m_TimeStamp + 1, Process::ProcessStatus::P_SCHEDULED);
			m_ScheduledProcess = nullptr;
			m_RequirePreemption = true;
			--m_UnProcessedCount;
		}
		else if (m_ScheduledProcess->GetQuantumUsage() >= quantumShare && !m_ReadyQueue.empty())
		{
			PS_CORE_INFO("{} Preempted!, readyQueueSize: {}, quantumShare: {}, currentQuantum: {}", m_ScheduledProcess->GetProcessId(), m_ReadyQueue.size(), quantumShare, m_ScheduledProcess->GetQuantumUsage());
			m_ScheduledProcess->UpdateStatus(m_TimeStamp + 1, Process::ProcessStatus::P_PREEMPTED);
			m_ReadyQueue.push_back(m_ScheduledProcess);
			m_ScheduledProcess->ResetQuantumUsage();
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
	virtual void ClearPreviousData()
	{
		m_ProcessPool.clear();
		m_ReadyQueue.clear();
		m_TimeStamp = 0;
		m_ScheduledProcess = nullptr;
		m_ProcessCharts.clear();
		m_UnProcessedCount = 0;
		m_Throughput = 0;
		m_Utilization = 0;
		m_TotalExecution = 0;
		m_Result = 0;
		m_AVGTurnaroundTime = 0;
		m_AVGResponseTime = 0 ;
		m_AVGWaitingTime=0;

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
		std::sort(m_ReadyQueue.begin(), m_ReadyQueue.end(), SchedulingCriterias(m_CompareOrder, m_TimeStamp));
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
	std::vector<SchedulerQueueFeatures> m_SchedulingQueues;
	std::vector<SchedulingMetrics> m_SchedulingMetrics;
	float m_Throughput;
	float m_Utilization;
	uint32_t m_TotalExecution;
	bool m_Result;
	float m_AVGTurnaroundTime;
	float m_AVGResponseTime;
	float m_AVGWaitingTime;
};