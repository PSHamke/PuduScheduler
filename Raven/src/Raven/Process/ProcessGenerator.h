#pragma once
#include "Process.h"
#include "Walnut/Random.h"
#include <numeric>
#include <optional>

class ProcessGenerator
{
public:
	ProcessGenerator()
	{
		Walnut::Random::Init();
		m_ProcessLimit = 100;
		m_AvailableIds.resize(m_ProcessLimit);
	}
	std::optional<Process> Generate()
	{
		return Generate
		("",
			Walnut::Random::UInt(0, 4), // Arrival time
			Walnut::Random::UInt(1, 5),  // Burst time
			Walnut::Random::UInt(0, 10)
		);
	}

	std::optional<Process> Generate(const std::string& name, const uint32_t arrivalTime, const uint32_t burstTime, const uint32_t priority) // Custom 
	{
		uint32_t p_Id = GetAvailableId();
		if (p_Id >= m_ProcessLimit)
			return std::nullopt;

		Process proc
		{
			name,
			p_Id,
			arrivalTime, // Arrival time
			burstTime,  // Burst time
			priority   // Priority
		};

		return proc;
	}
	const uint32_t GetAvailableId()
	{
		uint32_t i = 0;
		for (; i < m_AvailableIds.size(); ++i)
		{
			if (!m_AvailableIds[i])
			{
				m_AvailableIds[i] = true;
				break;
			}

		}
		return i;
	}

	void ReleaseId(uint32_t id)
	{
		m_AvailableIds[id] = false;
	}
private:
	uint32_t m_ProcessLimit;
	std::vector<Process> History;
	std::vector<bool> m_AvailableIds;
};