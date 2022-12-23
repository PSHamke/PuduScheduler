#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include <iostream>
#include <format>

enum class Action : uint32_t
{
	SUBMITTED_TO_SCHEDULER,
	SUBMITTED_TO_WAIT_QUEUE,
	SUBMITTED_TO_READY_QUEUE,
	CONTEXT_SWITCH_TO_IN,
	CONTEXT_SWITCH_TO_OUT,
	SUBMIT_BACK_TO_WAIT_QUEUE,
	SUBMIT_BACK_TO_READY_QUEUE,
	BURST,
	PREEMPTED,
	SCHEDULED
};

struct TimeStamp
{
	Action m_Action;
	uint32_t m_StartTime;
	uint32_t m_EndTime;
};

enum class ProcessClass : uint8_t
{
	Time_Sharing,
	Interactive,
	Fair_Share,
	Fixed_Priority,
	System,
	Real_Time
};
struct ProcessChart
{
	enum class CompleteReason
	{
		Finished,
		Preempted
	};

	std::string m_Label;
	int32_t m_Usage;
	int32_t m_StartTime;
	int32_t m_QueueIndex;
	uint32_t m_Quantum;
	uint8_t m_QueueType;
	
	//ImU32 m_Color;
	//CompleteReason m_Reason;
};



struct SchedulingMetrics
{
	std::string m_Label;
	uint32_t m_TurnaroundTime;
	uint32_t m_WaitingTime;
	uint32_t m_ResponseTime;
};

class Process
{
public:
	enum class ProcessStatus : uint32_t
	{
		P_UNAVAILABLE,
		P_WAITING_TO_SUBMIT,
		P_WAITING_IN_READY_QUEUE,
		P_PROCESSING,
		P_PREEMPTED,
		P_SCHEDULED
	};
	Process() = default;
public:
	Process(const std::string& name, const uint32_t id, const uint32_t arrivalTime, const uint32_t burstTime, 
		const uint32_t priority, const uint32_t vruntime, const uint32_t niceValue, const ProcessClass processClass, const uint32_t globalPriority) :
		m_ProcessName(name), m_ProcessId(id), m_ArrivalTime(arrivalTime), m_CPUBustTime(burstTime), m_Priority(priority),
		m_VRunTime(vruntime), m_NiceValue(niceValue), m_ProcessClass(processClass), m_GlobalPriority(globalPriority)
	{
		m_ProcessLabel = std::format("P{}", m_ProcessId);
		m_RemainingBurst = m_CPUBustTime;
		m_QuantumUsage = 0;
		m_CurrentQueue = 0;
		m_NextVRunTime = vruntime;
		m_StartSchedulingTime = 0;
		m_EndSchedulingTime = 0;
		m_FirstResponseTime = -1;
		m_Status = ProcessStatus::P_UNAVAILABLE;
		m_Used = false;
	}
	friend std::ostream& operator <<(std::ostream& os, Process  const& proc)
	{
		proc.PrintOn(os);
		return os;
	}

	inline const uint32_t GetArrivalTime() const
	{
		return m_ArrivalTime;
	}
	inline std::reference_wrapper<uint32_t> GetArrivalTimeRef()
	{
		return std::ref(m_ArrivalTime);
	}

	inline std::reference_wrapper<uint32_t> GetBurstTimeRef()
	{
		return std::ref(m_CPUBustTime);
	}

	inline std::reference_wrapper<uint32_t> GetPriorityRef()
	{
		return std::ref(m_Priority);
	}

	inline std::reference_wrapper<uint32_t> GetVRuntimeRef()
	{
		return std::ref(m_VRunTime);
	}

	inline std::reference_wrapper<uint32_t> GetNiceValueRef()
	{
		return std::ref(m_NiceValue);
	}

	inline std::reference_wrapper<ProcessClass> GetProcessClassRef()
	{
		return std::ref(m_ProcessClass);
	}
	inline const bool IsAvailableAt(uint32_t currentTime) const
	{
		return m_ArrivalTime <= currentTime;
	}
	inline void CheckAndUpdateStatus(uint32_t currentTime, ProcessStatus status)
	{
		if (m_Status == ProcessStatus::P_UNAVAILABLE && IsAvailableAt(currentTime))
		{
			SetStatus(ProcessStatus::P_WAITING_TO_SUBMIT);
			m_StartSchedulingTime = currentTime;
			m_TimeStamps.push_back({ Action::SUBMITTED_TO_SCHEDULER,currentTime,currentTime }); // Submitted using only one cycle
			m_TimeStamps.push_back({ Action::SUBMITTED_TO_WAIT_QUEUE,currentTime,currentTime }); // Submitted using only one cycle
		}
	}
	inline void UpdateStatus(uint32_t currentTime, ProcessStatus status)
	{
		switch (status)
		{
		case ProcessStatus::P_WAITING_IN_READY_QUEUE: // In ready queue
			SetStatus(ProcessStatus::P_WAITING_IN_READY_QUEUE);
			m_TimeStamps.push_back({ Action::SUBMITTED_TO_READY_QUEUE,currentTime,currentTime }); // Submitted using only one cycle
			break;
		case ProcessStatus::P_PROCESSING:
			if (m_Status == ProcessStatus::P_PROCESSING)
				break;
			SetStatus(ProcessStatus::P_PROCESSING);
			m_TimeStamps.push_back({ Action::CONTEXT_SWITCH_TO_IN,currentTime,currentTime }); // Switched with using few cycle
			break;
		case ProcessStatus::P_PREEMPTED:
			SetStatus(ProcessStatus::P_PREEMPTED);
			m_TimeStamps.push_back({ Action::CONTEXT_SWITCH_TO_OUT,currentTime,currentTime }); // Switched with using few cycle
			m_TimeStamps.push_back({ Action::PREEMPTED,currentTime,currentTime }); // Switched with using few cycle
			break;
		case ProcessStatus::P_SCHEDULED: // TODO: 
			SetStatus(ProcessStatus::P_SCHEDULED);
			m_TimeStamps.push_back({ Action::SCHEDULED,currentTime,currentTime }); // Signalled only one cycle
			m_TimeStamps.push_back({ Action::CONTEXT_SWITCH_TO_OUT,currentTime,currentTime }); // Signalled only one cycle
			m_EndSchedulingTime = currentTime;
			break;
		}

	}

	inline const bool IsWaitingToSubmit() const
	{
		return m_Status == ProcessStatus::P_WAITING_TO_SUBMIT;
	}
	inline const uint32_t GetBurstTime() const
	{
		return m_CPUBustTime;
	}
	inline const uint32_t GetRemainingTime() const
	{
		return m_RemainingBurst;
	}
	inline void SetRemainingTime(uint32_t remainingTime)
	{
		m_RemainingBurst = remainingTime;
	}
	const uint32_t GetPriority() const
	{
		return m_Priority;
	}

	const uint32_t GetVRunTime() const 
	{
		return m_VRunTime;
	}
	const uint32_t GetSchedulingStartTime() const
	{
		return m_StartSchedulingTime;
	}
	const uint32_t GetSchedulingEndTime() const
	{
		return m_EndSchedulingTime;
	}

	const uint32_t GetFirstResponseTime() const
	{
		return m_FirstResponseTime;
	}
	const float GetNextVRunTime() const
	{
		return m_NextVRunTime;
	}

	const uint32_t GetGlobalPriority() const
	{
		return m_GlobalPriority;
	}

	void SetGlobalPriority(const uint32_t globalPriority)
	{
		if ((uint32_t)m_ProcessClass <= 3)
		{
			m_GlobalPriority = globalPriority;

			if (m_GlobalPriority > 59)
			{
				m_GlobalPriority = 59;
			}
		}
		else if (m_ProcessClass == ProcessClass::System)
		{
			m_GlobalPriority = globalPriority;
			if (globalPriority < 60)
			{
				m_GlobalPriority = 60;
			}
			else if (globalPriority > 99)
			{
				m_GlobalPriority = 99;
			}
		}
		else
		{
			m_GlobalPriority = globalPriority;
			if (globalPriority < 100)
			{
				m_GlobalPriority = 100;
			}
			else if (globalPriority > 159)
			{
				m_GlobalPriority = 159;
			}
		}
	}

	void SetNextVRunTime(const float nextVRunTime)
	{
		m_NextVRunTime = nextVRunTime;
	}

	const uint32_t GetNiceValue() const
	{
		return m_NiceValue;
	}

	const ProcessClass GetProcessClass() const
	{
		return m_ProcessClass;
	}

	inline void SetStatus(const ProcessStatus status)
	{
		m_Status = status;
	}
	inline const uint32_t GetProcessId() const
	{
		return m_ProcessId;
	}
	inline const std::string& GetProcessLabel() const
	{
		return m_ProcessLabel;
	}
	inline void Burst(uint32_t burstAmount, uint32_t startTime)
	{
		if (m_FirstResponseTime == -1)
			m_FirstResponseTime = startTime;

		m_Used = true;
		m_RemainingBurst -= burstAmount;
		m_TimeStamps.push_back({ Action::BURST, startTime, startTime + burstAmount });
	}
	inline const uint32_t GetQuantumUsage() const
	{
		return m_QuantumUsage;
	}
	inline const void ResetQuantumUsage()
	{
		m_QuantumUsage = 0;
	}
	inline void UseQuantum(uint32_t burstAmount)
	{
		m_QuantumUsage += burstAmount;
	}

	inline void SetCurrentQueue(uint32_t current)
	{
		m_CurrentQueue = current;
	}

	inline const uint32_t GetCurrentQueue() const {
		return m_CurrentQueue;
	}

	inline void RevertBack()
	{
		if (m_Used)
		{
			m_QuantumUsage = 0;
			m_CurrentQueue =0;
			m_RemainingBurst = m_CPUBustTime;
			m_Status = ProcessStatus::P_UNAVAILABLE;
			m_TimeStamps.clear();
			m_Used = false;
		}

	}
	void PrintTimeStamps(std::ostream& os) const
	{
		const char* actions[] =
		{
			"SUBMITTED_TO_SCHEDULER",
			"SUBMITTED_TO_WAIT_QUEUE",
			"SUBMITTED_READY_QUEUE",
			"CONTEXT_SWITCH_TO_IN",
			"CONTEXT_SWITCH_TO_OUT",
			"SUBMITTED_BACK_TO_WAIT_QUEUE",
			"SUBMIT_BACK_TO_READY_QUEUE",
			"BURST",
			"PREEMPTED",
			"SCHEDULED"
		};

		os << "{p_" << m_ProcessId << "}\n";
		os << "{CPU BURST :" << m_CPUBustTime << "}\n";
		for (const auto& it : m_TimeStamps)
		{
			os << "S: " << it.m_StartTime << " - F: " << it.m_EndTime;
			os << " Action: {" << actions[(uint32_t)it.m_Action] << "}\n";
		}

	}
private:

	void PrintOn(std::ostream& os) const
	{
		os << "{";
		os << " name: " << m_ProcessName;
		os << " pId: " << m_ProcessId;
		os << " arrival: " << m_ArrivalTime;
		os << " cpuBurst: " << m_CPUBustTime;
		os << " priority " << m_Priority;
		os << " label " << m_ProcessLabel;
		os << "}";
	}


private:
	std::string m_ProcessName;
	uint32_t m_ProcessId;
	uint32_t m_ArrivalTime;
	uint32_t m_CPUBustTime;
	uint32_t m_Priority;
	uint32_t m_VRunTime;
	uint32_t m_GlobalPriority;
	float m_NextVRunTime;
	uint32_t m_NiceValue;
	ProcessClass m_ProcessClass;
	std::string m_ProcessLabel;
	uint32_t m_RemainingBurst;
	uint32_t m_QuantumUsage;
	std::vector<TimeStamp> m_TimeStamps;
	ProcessStatus m_Status;
	uint32_t m_StartSchedulingTime;
	uint32_t m_EndSchedulingTime;
	int32_t m_FirstResponseTime;
	uint32_t m_CurrentQueue;
	bool m_Used;
};