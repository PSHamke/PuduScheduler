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
			const uint32_t burstTime, const uint32_t priority, const uint32_t vruntime, const uint32_t niceValue, const ProcessClass processClass, const uint32_t globalPriority)
		{
			return Get().IGenerateAndSubmit(name, arrivalTime, burstTime, priority, vruntime, niceValue, processClass,globalPriority);
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
		static void LoadPresentation()
		{
			return Get().ILoadPresentation();
		}
	private:
		ProcessHandler()
		{
			Walnut::Random::Init();
			m_ProcessLimit = 100;
			m_AvailableIds.resize(m_ProcessLimit);
		
			
				
		}
		void ILoadPresentation()
		{
			/*
			
				arrivalTime, // Arrival time
				burstTime,  // Burst time
				priority,   // Priority
				vruntime,
				niceValue,
				processClass,
				globalPriority
			*/
			/* Default Processes*/
			{
				IGenerateAndSubmit("", 0, 13, 5, 1, 2, ProcessClass::Interactive, 20);
				IGenerateAndSubmit("", 0, 5, 4, 2, 3, ProcessClass::System, 61);
				IGenerateAndSubmit("", 30, 17, 12, 1, 2, ProcessClass::Time_Sharing, 9);
				IGenerateAndSubmit("", 0, 8, 7, 1, 2, ProcessClass::Fixed_Priority, 18);
				IGenerateAndSubmit("", 20, 6, 1, 1, 2, ProcessClass::Interactive, 39);
				IGenerateAndSubmit("", 30, 8, 1, 1, 2, ProcessClass::Real_Time, 124);
				IGenerateAndSubmit("", 30, 18, 8, 1, 2, ProcessClass::Interactive, 13);
				IGenerateAndSubmit("", 40, 13, 7, 1, 2, ProcessClass::Time_Sharing, 47);
				IGenerateAndSubmit("", 160, 22, 6, 1, 2, ProcessClass::Fair_Share, 11);
				IGenerateAndSubmit("", 0, 20, 1, 1, 2, ProcessClass::Real_Time, 138);
				IGenerateAndSubmit("", 70, 14, 1, 1, 2, ProcessClass::Fair_Share, 58);
				IGenerateAndSubmit("", 30, 14, 1, 1, 2, ProcessClass::Real_Time, 140);
				IGenerateAndSubmit("", 60, 14, 1, 1, 2, ProcessClass::Fixed_Priority, 32);
				IGenerateAndSubmit("", 160, 14, 1, 1, 2, ProcessClass::System, 88);
				IGenerateAndSubmit("", 55, 8, 4, 1, 2, ProcessClass::Time_Sharing, 32);
				IGenerateAndSubmit("", 80, 15, 1, 1, 2, ProcessClass::Fair_Share, 40);
				IGenerateAndSubmit("", 160, 24, 1, 1, 2, ProcessClass::Fixed_Priority, 14);
			}
		}
		std::optional<Process> IGenerateAndSubmit()
		{
			ProcessClass pclass = (ProcessClass)Walnut::Random::UInt(0, 5);
			uint32_t globalPriority = 0;
			if ((uint32_t)pclass <= 3)
				globalPriority = Walnut::Random::UInt(0, 59);
			else if (pclass == ProcessClass::System)
				globalPriority = Walnut::Random::UInt(60, 99);
			else
				globalPriority = Walnut::Random::UInt(100, 159);
			
			return IGenerateAndSubmit
			("",
				Walnut::Random::UInt(0, 7), // Arrival time
				Walnut::Random::UInt(3, 8),  // Burst time
				Walnut::Random::UInt(0, 10),
				Walnut::Random::UInt(0, 2),
				Walnut::Random::UInt(1, 3),
				pclass,
				globalPriority
			);
		}

		std::optional<Process> IGenerateAndSubmit(const std::string& name, const uint32_t arrivalTime,
			const uint32_t burstTime, const uint32_t priority, const uint32_t vruntime, const uint32_t niceValue, const ProcessClass processClass, const uint32_t globalPriority)
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
				priority,   // Priority
				vruntime,
				niceValue,
				processClass,
				globalPriority
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
