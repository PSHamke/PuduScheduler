#pragma once
#include "Process/ProcessHandler.h"
#include "Schedulers/ShortestJobFirstScheduler.h"
#include "Schedulers/ShortestRemainingTimeScheduler.h"
#include "Schedulers/RoundRobinScheduler.h"
#include "Schedulers/HighestResponseRatioNextScheduler.h"
#include "map"
#include "Log/Log.h"
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
		ProcessIdLess,
		ProcessIdHigh,
		QuantumUsageLess,
		QuantumUsageHigh,
		ResponseRatioLess,
		ResponseRatioHigh
	};

	enum class SchedulerType : uint8_t 
	{
		None,
		SJF,
		SRT,
		RR,
		HRRN,
		MLFQ,
		CFS,
		Solaris
	};


	class SchedulerHandler
	{
	public:
		
		struct SchedulerData  // Unique id and pointer of scheduler itself
		{
			uint32_t m_Id;
			SchedulerType m_Type;
			Scheduler* m_Scheduler;
		};

	public:
		using t_CompareOrders = std::vector<uint8_t>;
		
		static SchedulerHandler& Get()
		{
			static SchedulerHandler instance;
			return instance;
		}

		static void  AllocateForScheduler()
		{
			return Get().IAllocateForScheduler();
		}

		static void SetScheduler(const uint32_t index, const SchedulerType type)
		{
			return Get().ISetScheduler(index,type);
		}

		static void InitScheduler(const uint32_t index, SchedulerSpecification& specification)
		{
			return Get().IInitScheduler(index, specification);
		}
		static void DeleteScheduler(uint32_t index)
		{
			return Get().IDeleteScheduler(index);
		}
		static void SwapSchedulers(uint32_t lhs, uint32_t rhs)
		{
			return Get().ISwapSchedulers(lhs,rhs);
		}
	
		static std::vector<uint8_t>& GetSchedulerComparatorList(uint32_t index)
		{
			return Get().IGetSchedulerComparatorList(index);
		}
		static const std::vector<SchedulerData>& GetSchedulerMap()
		{
			return Get().IGetSchedulerMap();
		}

		static SchedulerData& GetScheduler(uint32_t index)
		{
			return Get().IGetScheduler(index);
		}
		static void Run(uint32_t id)
		{
			return Get().IRun(id);
		}

		static std::vector<std::pair<SchedulerData*,std::vector<ProcessChart>>> GetProcessCharts ()
		{
			return Get().IGetProcessCharts();
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
		SchedulerHandler()
		{
			m_SchedulerLimit = 100;
			m_AvailableIds.resize(m_SchedulerLimit);
		}
		void IAllocateForScheduler()
		{
			m_Schedulers.push_back({IGetAvailableId(),SchedulerType::None,nullptr});
		}

		void ISetScheduler(const uint32_t index, const SchedulerType type)
		{

			Scheduler* scheduler = nullptr;
			SchedulerSpecification defaultSpec{};
			defaultSpec.m_Id = m_Schedulers[index].m_Id;
			defaultSpec.m_Quantum = 0;
			defaultSpec.m_StartTime = 0;
		
			switch (type)
			{
			case SchedulerType::SJF:
				scheduler = new ShortestJobFirstScheduler();
				defaultSpec.m_CompareOrder = {
					(uint8_t)Comparators::BurstLess,
					(uint8_t)Comparators::ArrivalLess,
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::ProcessIdLess
				};
				defaultSpec.m_PreemptionOrder = {};
				defaultSpec.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				break;
			case SchedulerType::SRT:
				scheduler = new ShortestRemainingTimeScheduler();
				defaultSpec.m_CompareOrder = {
					(uint8_t)Comparators::RemainingTimeLess,
					(uint8_t)Comparators::ArrivalLess,
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::ProcessIdLess
				};
				defaultSpec.m_PreemptionOrder = {};
				defaultSpec.m_Prop = SchedulerProp::S_PREEMPTIVE;
				break;
			case SchedulerType::RR:
				scheduler = new RoundRobinScheduler();
				defaultSpec.m_CompareOrder = {
					(uint8_t)Comparators::QuantumUsageLess,
					(uint8_t)Comparators::ArrivalLess,
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::ProcessIdLess
				};
				defaultSpec.m_PreemptionOrder = { 
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::BurstLess,
					(uint8_t) Comparators::ProcessIdLess
				};

				defaultSpec.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				break;
			case SchedulerType::HRRN:
				scheduler = new HighestResponseRatioNextScheduler();
				defaultSpec.m_CompareOrder = {
					(uint8_t)Comparators::ResponseRatioHigh,
					(uint8_t)Comparators::ArrivalLess,
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::ProcessIdLess
				};
				defaultSpec.m_PreemptionOrder = {};
				defaultSpec.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				break;
			default:
				assert(0, "Fallthrough");
				break;
			}

			if (scheduler)
			{
				m_Schedulers[index].m_Scheduler =  scheduler;
				m_Schedulers[index].m_Type = type;
				m_Schedulers[index].m_Scheduler->Init(defaultSpec);
			}
			else
			{
				assert(0, "Scheduler cannot be created!");
			}
			
		}

		void IInitScheduler(const uint32_t index , SchedulerSpecification& specification)
		{
			specification.m_Id = m_Schedulers[index].m_Id;
			m_Schedulers[index].m_Scheduler->Init(specification);
		}

		void IDeleteScheduler(uint32_t index)
		{
			auto schedulerIterator = std::next(m_Schedulers.begin(), index);
			IReleaseId(schedulerIterator->m_Id);
			m_Schedulers.erase(schedulerIterator);
		}
		
		void ISwapSchedulers(uint32_t lhs, uint32_t rhs)
		{
			std::swap(m_Schedulers[lhs], m_Schedulers[rhs]);
		}
		
		const std::vector<SchedulerData>& IGetSchedulerMap()
		{
			return m_Schedulers;
		}

		std::vector<uint8_t>& IGetSchedulerComparatorList(uint32_t index)
		{
			return m_Schedulers[index].m_Scheduler->GetComparatorList();
		}

		SchedulerData& IGetScheduler(uint32_t index)
		{
			return m_Schedulers[index];
		}

		void IRun(uint32_t id)
		{
			auto matchId = [&](SchedulerData& data) {return data.m_Id == id; };
			auto schedulerIterator = std::find_if(m_Schedulers.begin(), m_Schedulers.end(), matchId);
			if (schedulerIterator != m_Schedulers.end())
			{
				auto processPool = ProcessHandler::GetProcessPool();
				schedulerIterator->m_Scheduler->ClearPreviousData();
				schedulerIterator->m_Scheduler->SubmitProcessPool(processPool);
				schedulerIterator->m_Scheduler->Schedule();
			}
			
			//IGetProcessCharts();
			for (auto it : ProcessHandler::GetProcessPool())
			{
				it.PrintTimeStamps(std::cout);
			}
		}

		std::vector<std::pair<SchedulerData*, std::vector<ProcessChart>>> IGetProcessCharts()
		{
			std::vector<std::pair<SchedulerData*, std::vector<ProcessChart>>> processCharts{};
			for (auto& it : m_Schedulers)
			{
				if (it.m_Scheduler&&it.m_Scheduler->IsReadyToGrabResult())
				{
					
					std::vector<ProcessChart> temp = it.m_Scheduler->GetProcessChart();
					IMakeContinuous(temp);
					processCharts.push_back(std::make_pair(&it, temp));
			
				}
			}
			return processCharts;
		}
		/*
			Free Scheduler pointers, then clear the map.
			Call ::CleanUp to release process pool.
		*/

		void ICleanUp()
		{
			for (auto& it : m_Schedulers)
				delete it.m_Scheduler;

			m_Schedulers.clear();

			ProcessHandler::CleanUp();

		}

		void IFillTestProcess()
		{

		}

	private:
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

		void IMakeContinuous(std::vector<ProcessChart>& processCharts)
		{
			//PS_CORE_INFO("Entering IMakeContinous");
			// Check first process start time first

			if (processCharts[0].m_StartTime != 0)
			{
				ProcessChart temp{ "Idle", processCharts[0].m_StartTime,0 };
				processCharts.insert(processCharts.begin(), temp);
			}

			for (uint32_t i = 0; i < processCharts.size() -1; ++i)
			{
				uint32_t firstProcEnd = processCharts[i].m_StartTime + processCharts[i].m_Usage;
				uint32_t secondProcStart = processCharts[i + 1].m_StartTime;
				uint32_t difference = secondProcStart - firstProcEnd;
				if ( difference > 0 )
				{
					ProcessChart temp{ "Idle",difference,firstProcEnd };
					auto processChartIterator = processCharts.begin() + i + 1;
					processCharts.insert(processChartIterator, temp);
				}
			}
		}
	private:
		std::vector<Process> m_ProcessPool;
		std::vector<SchedulerData> m_Schedulers;
		std::vector<bool> m_AvailableIds;
		uint32_t m_SchedulerLimit;
	};
}//namespace Raven