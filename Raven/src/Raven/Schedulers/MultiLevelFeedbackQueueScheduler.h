#pragma once
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
class MultiLevelFeedbackQueueScheduler : public Scheduler
{
public:
	MultiLevelFeedbackQueueScheduler() : m_Initialized(false), m_ScheduledProcess(nullptr) {}

	virtual void Init(const SchedulerSpecification& specification) override
	{
		m_SchedulerId = specification.m_Id;
		m_StartTime = specification.m_StartTime;
		m_TimeStamp = specification.m_StartTime;
		m_RequirePreemption = true;
		m_UnitTime = 1;
		m_SchedulingQueues = specification.m_QueueFeatures;
		
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

	std::vector<uint8_t>& GetComparatorList(const uint8_t queueIndex) override
	{
		return m_SchedulingQueues[queueIndex].m_CompareOrder;
	}
	std::vector<SchedulerQueueFeatures>& GetQueueFeatures() override
	{
		return m_SchedulingQueues;
	}
	virtual std::reference_wrapper<uint32_t> GetStartTimeRef() override
	{
		return std::ref(m_StartTime);
	}
	virtual std::reference_wrapper<uint32_t> GetQuantumRef(const uint8_t queueIndex) override
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
	// TODO: Arrange Unit Time ?
	virtual void Schedule() override
	{
		// FIXME
		m_ReadyQueues.resize(m_SchedulingQueues.size() );
		while (m_UnProcessedCount > 0)
		{
			ProgressTick();
			int32_t queueIndex = AnyAvailable();
			if (m_RequirePreemption && (queueIndex != -1))
			{
				PickToSchedule(queueIndex);
			}

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

	virtual void Progress()
	{
		
		if (!m_ProcessCharts.empty())
		{
			auto& previous = m_ProcessCharts.back();
			if (previous.m_Label == m_ScheduledProcess->GetProcessLabel())
			{
				if (previous.m_QueueIndex == m_ScheduledProcess->GetCurrentQueue())
				{
					previous.m_Usage += 1;

				}
				else
				{
					ProcessChart temp{ m_ScheduledProcess->GetProcessLabel(),1,m_TimeStamp,m_ScheduledProcess->GetCurrentQueue() };
					temp.m_QueueType =(uint8_t) m_SchedulingQueues[m_ScheduledProcess->GetCurrentQueue()].m_Type;
					temp.m_Quantum = m_SchedulingQueues[m_ScheduledProcess->GetCurrentQueue()].m_Quantum;
					m_ProcessCharts.push_back(temp);
				}

			}
			else
			{
				ProcessChart temp{ m_ScheduledProcess->GetProcessLabel(),1,m_TimeStamp,m_ScheduledProcess->GetCurrentQueue() };
				temp.m_QueueType = (uint8_t)m_SchedulingQueues[m_ScheduledProcess->GetCurrentQueue()].m_Type;
				temp.m_Quantum = m_SchedulingQueues[m_ScheduledProcess->GetCurrentQueue()].m_Quantum;
				m_ProcessCharts.push_back(temp);
			}
		}
		else
		{
			ProcessChart temp{ m_ScheduledProcess->GetProcessLabel(),1,m_TimeStamp,m_ScheduledProcess->GetCurrentQueue() };
			temp.m_QueueType = (uint8_t)m_SchedulingQueues[m_ScheduledProcess->GetCurrentQueue()].m_Type;
			temp.m_Quantum = m_SchedulingQueues[m_ScheduledProcess->GetCurrentQueue()].m_Quantum;
			m_ProcessCharts.push_back(temp);
		}

		m_ScheduledProcess->Burst(1, m_TimeStamp);
		m_TotalExecution++;
		m_ScheduledProcess->UseQuantum(1);

		if (m_ScheduledProcess->GetRemainingTime() == 0)
		{
			m_ScheduledProcess->UpdateStatus(m_TimeStamp + 1, Process::ProcessStatus::P_SCHEDULED);
			m_ScheduledProcess = nullptr;
			m_RequirePreemption = true;
			--m_UnProcessedCount;
		}
		else if (m_SchedulingQueues[m_ScheduledProcess->GetCurrentQueue()].m_Type == SchedulerType::RR && m_ScheduledProcess->GetQuantumUsage() % m_SchedulingQueues[m_ScheduledProcess->GetCurrentQueue()].m_Quantum == 0)
		{
			
			m_ScheduledProcess->UpdateStatus(m_TimeStamp + 1, Process::ProcessStatus::P_PREEMPTED);
			if(m_ScheduledProcess->GetCurrentQueue() != m_ReadyQueues.size()-1)
				m_ScheduledProcess->SetCurrentQueue(m_ScheduledProcess->GetCurrentQueue() + 1); // Push to next queue
			m_ReadyQueues[m_ScheduledProcess->GetCurrentQueue()].push_back(m_ScheduledProcess);
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
		for(auto& it: m_ReadyQueues)
			it.clear();

		m_ReadyQueues.clear();
		m_TimeStamp = 0;
		m_ScheduledProcess = nullptr;
		m_ProcessCharts.clear();
		m_UnProcessedCount = 0;
	}

	virtual void FillReadyQueue()
	{
		uint8_t queueIndex = 0;
		bool alreadyQueued = false;
		for (auto& process : m_ProcessPool)
		{
			if (process->IsWaitingToSubmit()) {
				m_IntermediateQueue.push_back(process);
				process->UpdateStatus(m_TimeStamp, Process::ProcessStatus::P_WAITING_IN_READY_QUEUE);

				if (m_SchedulingQueues[queueIndex].m_Prop == SchedulerProp::S_PREEMPTIVE && m_ScheduledProcess != nullptr && !alreadyQueued)
				{
					if (SchedulingCriterias(m_SchedulingQueues[queueIndex].m_PreemptionOrder, m_TimeStamp)(process, m_ScheduledProcess))
					{
						m_RequirePreemption = true;
						m_IntermediateQueue.push_back(m_ScheduledProcess);
						alreadyQueued = true;
					}
				}

			}
		}
		std::sort(m_IntermediateQueue.begin(), m_IntermediateQueue.end(), SchedulingCriterias(m_SchedulingQueues[queueIndex].m_CompareOrder, m_TimeStamp));
		for (auto& it : m_IntermediateQueue)
		{
			m_ReadyQueues[queueIndex].push_back(m_IntermediateQueue.front());
			m_IntermediateQueue.pop_front();
		}
	}

	virtual void ProgressTick()
	{

		for (auto& proc : m_ProcessPool)
			proc->CheckAndUpdateStatus(m_TimeStamp, Process::ProcessStatus::P_WAITING_TO_SUBMIT);
		FillReadyQueue();
	}

	void PickToSchedule(uint32_t queueIdx)
	{
		//std::sort(m_ReadyQueue.begin(), m_ReadyQueue.end(), SchedulingCriterias(m_CompareOrder));
		Process* nextScheduledProcess = m_ReadyQueues[queueIdx].front();

		m_ReadyQueues[queueIdx].pop_front();
		if (m_ScheduledProcess != nullptr && m_ScheduledProcess != nextScheduledProcess)
		{
			m_ScheduledProcess->UpdateStatus(m_TimeStamp, Process::ProcessStatus::P_PREEMPTED);
		}
		m_ScheduledProcess = nextScheduledProcess;
		m_RequirePreemption = false;
	}

	int32_t AnyAvailable()
	{
		
		for (int32_t i = 0; i < m_ReadyQueues.size(); ++i)
		{
			if (m_ReadyQueues[i].size() > 0)
				return i;
		}

		return -1;
	}

private:
	uint32_t m_SchedulerId;
	uint32_t m_StartTime;
	uint32_t m_TimeStamp; // Might use to keep actions as well ? 
	uint32_t m_UnitTime;
	bool m_Initialized;
	std::vector<Process*> m_ProcessPool;
	uint32_t m_UnProcessedCount;
	std::vector<std::deque<Process*>> m_ReadyQueues;
	std::deque<Process*> m_IntermediateQueue;
	Process* m_ScheduledProcess;
	bool m_RequirePreemption;
	std::vector<ProcessChart> m_ProcessCharts;
	std::vector<SchedulerQueueFeatures> m_SchedulingQueues;
	std::vector<SchedulingMetrics> m_SchedulingMetrics;
	float m_Throughput;
	float m_Utilization;
	uint32_t m_TotalExecution;
	/*
		std::vector<uint8_t> m_CompareOrder;
		std::vector<uint8_t> m_PreemptionOrder;
		uint32_t m_Quantum;
		SchedulerType m_Type;
		SchedulerProp m_Prop;
	*/
	bool m_Result;
	float m_AVGTurnaroundTime;
	float m_AVGResponseTime;
	float m_AVGWaitingTime;
};