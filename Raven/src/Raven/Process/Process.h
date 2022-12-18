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
	//ImU32 m_Color;
	CompleteReason m_Reason;
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
	Process(const std::string& name, const uint32_t id, const uint32_t arrivalTime, const uint32_t burstTime, const uint32_t priority) :
		m_ProcessName(name), m_ProcessId(id), m_ArrivalTime(arrivalTime), m_CPUBustTime(burstTime), m_Priority(priority)
	{
		m_ProcessLabel = std::format("P{}", m_ProcessId);
		m_RemainingBurst = m_CPUBustTime;
		m_QuantumUsage = 0;
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
	inline const bool IsAvailableAt(uint32_t currentTime) const
	{
		return m_ArrivalTime <= currentTime;
	}
	inline void CheckAndUpdateStatus(uint32_t currentTime, ProcessStatus status)
	{
		if (m_Status == ProcessStatus::P_UNAVAILABLE && IsAvailableAt(currentTime))
		{
			SetStatus(ProcessStatus::P_WAITING_TO_SUBMIT);
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
		m_Used = true;
		m_RemainingBurst -= burstAmount;
		m_TimeStamps.push_back({ Action::BURST, startTime, startTime + burstAmount });
	}
	inline const uint32_t GetQuantumUsage() const
	{
		return m_QuantumUsage;
	}
	inline void UseQuantum(uint32_t burstAmount)
	{
		m_QuantumUsage += burstAmount;
	}

	inline void RevertBack()
	{
		if (m_Used)
		{
			m_QuantumUsage = 0;
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
	std::string m_ProcessLabel;
	uint32_t m_RemainingBurst;
	uint32_t m_QuantumUsage;
	std::vector<TimeStamp> m_TimeStamps;
	ProcessStatus m_Status;
	bool m_Used;
};