#pragma once
#include "../Process/Process.h"
#include <functional>
#include <array>

struct SchedulingCriterias
{
public:
#define equal 2
	using t_ComparatorList = std::array<std::function<const uint8_t(const Process*, const Process*)>, 14>;
	bool operator() (const Process* lhs, const Process* rhs) const
	{
		uint8_t result = 0;

		for (const auto idx : m_CompareOrder)
		{
			result = m_Comparators[idx](lhs, rhs);
			if (result != equal)
				break;
		}
		return (bool) result;
	}

	SchedulingCriterias(std::vector<uint8_t>& compareOrder, uint32_t currentTime)
	{
		m_CompareOrder = compareOrder;
		s_CurrentTime = currentTime;
	}
private:
	std::vector<uint8_t> m_CompareOrder;
	static uint32_t s_CurrentTime;

	t_ComparatorList m_Comparators{ {
		BurstLess,
		BurstHigh,
		RemainingTimeLess,
		RemainingTimeHigh,
		PriorityLess,
		PriorityHigh,
		ArrivalLess,
		ArrivalHigh,
		QuantumUsageLess,
		QuantumUsageHigh,
		ProcessIdLess,
		ProcessIdHigh,
		ResponseRatioLess,
		ResponseRatioHigh
	}};
	static const uint8_t BurstLess(const Process* lhs, const Process* rhs)
	{
		if (lhs->GetBurstTime() == rhs->GetBurstTime())
			return equal;
		
		return uint8_t(lhs->GetBurstTime() < rhs->GetBurstTime());
	}

	static const uint8_t BurstHigh(const Process* lhs, const Process* rhs) 
	{
		if (lhs->GetBurstTime() == rhs->GetBurstTime())
			return equal;

		return uint8_t(lhs->GetBurstTime() > rhs->GetBurstTime());
	}

	static const uint8_t RemainingTimeLess(const Process* lhs, const Process* rhs) 
	{
		if (lhs->GetRemainingTime() == rhs->GetRemainingTime())
			return equal;

		return uint8_t(lhs->GetRemainingTime() < rhs->GetRemainingTime());
	}

	static const uint8_t RemainingTimeHigh(const Process* lhs, const Process* rhs) 
	{
		if (lhs->GetRemainingTime() == rhs->GetRemainingTime())
			return equal;

		return uint8_t(lhs->GetRemainingTime() > rhs->GetRemainingTime());
	}

	static const uint8_t PriorityLess(const Process* lhs, const Process* rhs) 
	{
		if (lhs->GetPriority() == rhs->GetPriority())
			return equal;
		std::cout << "Priority less \n";
		return uint8_t(lhs->GetPriority() < rhs->GetPriority());
	}

	static const uint8_t PriorityHigh(const Process* lhs, const Process* rhs) 
	{
		if (lhs->GetPriority() == rhs->GetPriority())
			return equal;
		std::cout << "Priority high\n";
		return uint8_t(lhs->GetPriority() > rhs->GetPriority());
	}
	
	static const uint8_t ArrivalLess(const Process* lhs, const Process* rhs) 
	{
		if (lhs->GetArrivalTime() == rhs->GetArrivalTime())
			return equal;

		std::cout << "arrival less \n";
		return uint8_t(lhs->GetArrivalTime() < rhs->GetArrivalTime());
	}

	static const uint8_t ArrivalHigh(const Process* lhs, const Process* rhs) 
	{
		if (lhs->GetArrivalTime() == rhs->GetArrivalTime())
			return equal;
		std::cout << "arrival high \n";
		return uint8_t(lhs->GetArrivalTime() > rhs->GetArrivalTime());
	}

	static const uint8_t QuantumUsageLess(const Process* lhs, const Process* rhs) 
	{
		if (lhs->GetQuantumUsage() == rhs->GetQuantumUsage())
			return equal;

		std::cout << "Quantum usage less\n";
		return uint8_t(lhs->GetQuantumUsage() < rhs->GetQuantumUsage());
	}

	static const uint8_t QuantumUsageHigh(const Process* lhs, const Process* rhs) 
	{
		if (lhs->GetQuantumUsage() == rhs->GetQuantumUsage())
			return equal;
		std::cout << "Quantum usage high\n";
		return uint8_t(lhs->GetQuantumUsage() > rhs->GetQuantumUsage());
	}

	static const uint8_t ProcessIdLess(const Process* lhs, const Process* rhs) 
	{
		std::cout << "pid less \n";
		return uint8_t(lhs->GetProcessId() < rhs->GetProcessId());
	}

	static const uint8_t ProcessIdHigh(const Process* lhs, const Process* rhs) 
	{

		std::cout << "pid high \n";
		return uint8_t(lhs->GetProcessId() > rhs->GetProcessId());
	}
	//((s_CurrentTime - proc->GetArrivalTime()) == Waiting Time
	// Score = Waiting Time + Remaining Time / Remaining Time
	static const uint8_t ResponseRatioLess(const Process* lhs, const Process* rhs)
	{
		float scoreL = ((s_CurrentTime - lhs->GetArrivalTime()) + lhs->GetRemainingTime()) / lhs->GetRemainingTime();
		float scoreR = ((s_CurrentTime - rhs->GetArrivalTime()) + rhs->GetRemainingTime()) / rhs->GetRemainingTime();
		if (scoreL == scoreR)
			return equal;
		return scoreL < scoreR;
	}

	static const uint8_t ResponseRatioHigh(const Process* lhs, const Process* rhs)
	{
		float scoreL = float ((s_CurrentTime - lhs->GetArrivalTime()) + lhs->GetRemainingTime()) / lhs->GetRemainingTime();
		float scoreR = float ((s_CurrentTime - rhs->GetArrivalTime()) + rhs->GetRemainingTime()) / rhs->GetRemainingTime();
		if (scoreL == scoreR)
			return equal;
		return scoreL > scoreR;
	}
#undef equal
};