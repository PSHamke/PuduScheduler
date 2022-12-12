#pragma once
#include "Process/ProcessGenerator.h"
#include "Schedulers/ShortestJobFirstScheduler.h"
#include "Schedulers/HighestResponseRatioNextScheduler.h"
#include "map"
namespace Raven
{
	void Handle();

	void DebugTest();


	enum class Comparators : uint8_t {
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
	};

	enum class SchedulerType : uint8_t 
	{
		SJF,
		SRT,
		RR,
		HRRN
	};


	class SchedulerHandler
	{
	public:
		using t_CompareOrders = std::vector<uint8_t>;
		static SchedulerHandler& Get()
		{
			static SchedulerHandler instance;
			return instance;
		}

		static void  AddScheduler(uint32_t id ,SchedulerType type, t_CompareOrders compareOrder)
		{
			return Get().IAddScheduler(id,type, compareOrder);
		}

		static void Run(uint32_t id)
		{
			return Get().IRun(id);
		}

		static void FillTestProcess()
		{
			return Get().IFillTestProcess();
		}

		static void CleanUp()
		{
			return Get().ICleanUp();
		}
	private:
		void IAddScheduler(uint32_t id,SchedulerType type, t_CompareOrders compareOrder)
		{
			switch (type)
			{
			case SchedulerType::SJF:
				Scheduler* sc = new ShortestJobFirstScheduler(compareOrder);
				m_Schedulers.insert(std::make_pair(id,sc));
			}
		}

		void IRun(uint32_t id)
		{
			
			m_Schedulers[id]->SubmitProcessPool(m_ProcessPool);
			m_Schedulers[id]->Schedule();

			for (const auto it : m_ProcessPool)
			{
				it.PrintTimeStamps(std::cout);
			}
		}

		void ICleanUp()
		{
			for (auto& it : m_Schedulers)
				delete it.second;

			m_Schedulers.clear();

			for (const auto& it : m_ProcessPool)
				ProcessGenerator::ReleaseId(it.GetProcessId());
			m_ProcessPool.clear();
			

		}

		void IFillTestProcess()
		{
			m_ProcessPool.push_back(ProcessGenerator::Generate("", 3, 3, 5).value());
			m_ProcessPool.push_back(ProcessGenerator::Generate("", 0, 5, 5).value());
			m_ProcessPool.push_back(ProcessGenerator::Generate("", 15, 4, 5).value());
			m_ProcessPool.push_back(ProcessGenerator::Generate("", 3, 3, 5).value());
			m_ProcessPool.push_back(ProcessGenerator::Generate("", 1, 1, 5).value());
		}
	private:
		std::vector<Process> m_ProcessPool;
		std::map<uint32_t,Scheduler*> m_Schedulers;
	};
}//namespace Raven