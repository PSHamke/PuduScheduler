#pragma once
#include "Process.h"
#include "Walnut/Random.h"
#include <numeric>
#include <optional>

/*
	Singleton Class
*/
namespace Raven
{
	class ProcessHandler
	{
	public:
		ProcessHandler(const ProcessHandler&) = delete;

		static ProcessHandler& Get()
		{
			static ProcessHandler instance;
			return instance;
		}

		static std::optional<Process> GenerateAndSubmit(const std::string& name, const uint32_t arrivalTime,
			const uint32_t burstTime, const uint32_t priority)
		{
			return Get().IGenerateAndSubmit(name, arrivalTime, burstTime, priority);
		}

		static std::optional<Process> GenerateAndSubmit()
		{
			return Get().IGenerateAndSubmit();
		}

		static void CleanUp()
		{
			return Get().ICleanUp();
		}

		static void ReleaseId(uint32_t id)
		{
			return Get().IReleaseId(id);
		}

		static std::vector<Process>& GetProcessPool()
		{
			return Get().IGetProcessPool();
		}

		static Process& GetProcess(uint32_t processIndex)
		{
			return Get().IGetProcess(processIndex);
		}

		static const uint32_t GetMaxProcessLimit()
		{
			return Get().IGetMaxProcessLimit();
		}

		static void DeleteProcess(uint32_t deletedIndex)
		{
			return Get().IDeleteProcess(deletedIndex);
		}

		static void SwapProcesses(uint32_t lhs, uint32_t rhs)
		{
			return Get().ISwapSwapProcesses(lhs, rhs);
		}

	private:
		ProcessHandler()
		{
			Walnut::Random::Init();
			m_ProcessLimit = 100;
			m_AvailableIds.resize(m_ProcessLimit);
		}
		std::optional<Process> IGenerateAndSubmit()
		{
			return IGenerateAndSubmit
			("",
				Walnut::Random::UInt(0, 4), // Arrival time
				Walnut::Random::UInt(1, 5),  // Burst time
				Walnut::Random::UInt(0, 10)
			);
		}

		std::optional<Process> IGenerateAndSubmit(const std::string& name, const uint32_t arrivalTime,
			const uint32_t burstTime, const uint32_t priority) // Custom 
		{
			uint32_t p_Id = IGetAvailableId();
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

			m_ProcessPool.push_back(proc);
			return proc;
		}

		void ICleanUp()
		{
			for (const auto& it : m_ProcessPool)
				IReleaseId(it.GetProcessId());
			m_ProcessPool.clear();
		}
		const uint32_t IGetAvailableId()
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

		void IReleaseId(uint32_t id)
		{
			m_AvailableIds[id] = false;
		}

		std::vector<Process>& IGetProcessPool()
		{
			return m_ProcessPool;
		}

		Process& IGetProcess(uint32_t processIndex)
		{
			return m_ProcessPool[processIndex];
		}

		const uint32_t IGetMaxProcessLimit()
		{
			return m_ProcessLimit;
		}

		void IDeleteProcess(uint32_t deletedIndex)
		{
			auto processIterator = std::next(m_ProcessPool.begin(), deletedIndex);
			IReleaseId(processIterator->GetProcessId());
			m_ProcessPool.erase(processIterator);
		}

		void ISwapSwapProcesses(uint32_t lhs, uint32_t rhs)
		{
			std::swap(m_ProcessPool[lhs], m_ProcessPool[rhs]);
		}
	private:
		uint32_t m_ProcessLimit;
		std::vector<bool> m_AvailableIds;
		std::vector<Process> m_ProcessPool;
	};
}//namespace Raven
