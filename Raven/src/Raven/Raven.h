#pragma once
#include "Process/ProcessHandler.h"
#include "Schedulers/ShortestJobFirstScheduler.h"
#include "Schedulers/ShortestRemainingTimeScheduler.h"
#include "Schedulers/RoundRobinScheduler.h"
#include "Schedulers/HighestResponseRatioNextScheduler.h"
#include "Schedulers/MultiLevelFeedbackQueueScheduler.h"
#include "Schedulers/CompletelyFairScheduler.h"
#include "Schedulers/SolarisScheduler.h"
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
		ResponseRatioHigh,
		VRunTimeLess,
		VRunTimeHigh,
		NiceValueLess,
		NiceValueHigh,
		GlobalPriorityLess,
		GlobalPriorityHigh
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
		struct SchedulingResult
		{
			SchedulerData* m_SchedulerPtr;
			std::vector<SchedulingMetrics> m_SchedulingMetrics;
			float m_Throughput;
			float m_Utilization;
			float m_AVGResponse;
			float m_AVGWaiting;
			float m_AVGTurnAround;
		};
	public:
		using t_CompareOrders = std::vector<uint8_t>;
		using t_SchedulingMetricsVector = std::vector<SchedulingResult>;
		
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
	
		static std::vector<uint8_t>& GetSchedulerComparatorList(uint32_t index, uint8_t queueIndex)
		{
			return Get().IGetSchedulerComparatorList(index,queueIndex);
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
		static t_SchedulingMetricsVector GetSchedulingMetrics()
		{
			return Get().IGetSchedulingMetrics();
		}
		static void FillTestProcess()
		{
			return Get().IFillTestProcess();
		}

		static void CleanUp()
		{
			return Get().ICleanUp();
		}

		static void SetDefaultCompareOrders(std::vector<uint8_t>& compareOrder, const SchedulerType type)
		{
			switch (type)
			{
			case SchedulerType::SJF:
				
				compareOrder = {
					(uint8_t)Comparators::BurstLess,
					(uint8_t)Comparators::ArrivalLess,
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::ProcessIdLess
				};
				break;
			case SchedulerType::SRT:
				compareOrder = {
					(uint8_t)Comparators::RemainingTimeLess,
					(uint8_t)Comparators::ArrivalLess,
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::ProcessIdLess
				};
				break;
			case SchedulerType::RR: case SchedulerType::MLFQ:
				compareOrder = {
					(uint8_t)Comparators::QuantumUsageLess,
					(uint8_t)Comparators::ArrivalLess,
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::ProcessIdLess
				};
				break;
			case SchedulerType::HRRN:
				compareOrder = {
					(uint8_t)Comparators::ResponseRatioHigh,
					(uint8_t)Comparators::ArrivalLess,
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::ProcessIdLess
				};
				break;
			case SchedulerType::Solaris:
				compareOrder = {
					(uint8_t)Comparators::GlobalPriorityHigh,
					(uint8_t)Comparators::ArrivalLess,
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::ProcessIdLess
				};
				break;
			case SchedulerType::CFS:
				compareOrder = {
					(uint8_t)Comparators::VRunTimeLess,
					(uint8_t)Comparators::NiceValueLess,
					(uint8_t)Comparators::ArrivalLess,
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::ProcessIdLess
				};
				break;
			default:
				assert(0, "Fallthrough");
				break;
			}
		}

		static void MakeContinuous(std::vector<ProcessChart>& processCharts)
		{
			//PS_CORE_INFO("Entering IMakeContinous");
			// Check first process start time first
			// FIXME
			if (processCharts.empty())
				return;

			if (processCharts[0].m_StartTime != 0)
			{
				ProcessChart temp{ "Idle", processCharts[0].m_StartTime,0 };
				processCharts.insert(processCharts.begin(), temp);
			}

			for (uint32_t i = 0; i < processCharts.size() - 1; ++i)
			{
				uint32_t firstProcEnd = processCharts[i].m_StartTime + processCharts[i].m_Usage;
				uint32_t secondProcStart = processCharts[i + 1].m_StartTime;
				uint32_t difference = secondProcStart - firstProcEnd;
				if (difference > 0)
				{
					ProcessChart temp{ "Idle",difference,firstProcEnd };
					auto processChartIterator = processCharts.begin() + i + 1;
					processCharts.insert(processChartIterator, temp);
				}
			}
		}
		static void LoadPresentation()
		{
			return Get().ILoadPresentation();
		}
	private:
		SchedulerHandler()
		{
			m_SchedulerLimit = 100;
			m_AvailableIds.resize(m_SchedulerLimit);

		}

		void ILoadPresentation()
		{
			for (int i = 0; i < 7; i++)
			{
				IAllocateForScheduler();
				ISetPresentationScheduler(i, (SchedulerType)(i + 1));
			}
		}


		void ISetPresentationScheduler(const uint32_t index, const SchedulerType type)
		{

			Scheduler* scheduler = nullptr;
			std::vector<SchedulerQueueFeatures> defaultQueueFeaturesVector{};
			SchedulerQueueFeatures defaultQueueFeature{};
			SchedulerSpecification defaultSpec{};
			defaultSpec.m_Id = m_Schedulers[index].m_Id;
			defaultSpec.m_StartTime = 0;
			SetDefaultCompareOrders(defaultQueueFeature.m_CompareOrder, type);
			defaultQueueFeature.m_Type = type;
			switch (type)
			{
			case SchedulerType::SJF:
				scheduler = new ShortestJobFirstScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 0;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::SRT:
				scheduler = new ShortestRemainingTimeScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 0;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::RR:
				scheduler = new RoundRobinScheduler();
				defaultQueueFeature.m_PreemptionOrder = {
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::BurstLess,
					(uint8_t)Comparators::ProcessIdLess
				};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 50;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::HRRN:
				scheduler = new HighestResponseRatioNextScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 0;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::MLFQ:
				scheduler = new MultiLevelFeedbackQueueScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 40;
				defaultQueueFeature.m_Type = SchedulerType::RR;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 80;
				defaultQueueFeature.m_Type = SchedulerType::RR;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 120;
				defaultQueueFeature.m_Type = SchedulerType::RR;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 0;
				defaultQueueFeature.m_Type = SchedulerType::HRRN;
				SetDefaultCompareOrders(defaultQueueFeature.m_CompareOrder, defaultQueueFeature.m_Type);
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::Solaris:
				scheduler = new SolarisScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::CFS:
				scheduler = new CompletelyFairScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 60;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			default:
				assert(0, "Fallthrough");
				break;
			}

			if (scheduler)
			{
				m_Schedulers[index].m_Scheduler = scheduler;
				m_Schedulers[index].m_Type = type;
				m_Schedulers[index].m_Scheduler->Init(defaultSpec);
			}
			else
			{
				assert(0, "Scheduler cannot be created!");
			}

		}


		void IAllocateForScheduler()
		{
			m_Schedulers.push_back({IGetAvailableId(),SchedulerType::None,nullptr});
		}

		void ISetScheduler(const uint32_t index, const SchedulerType type)
		{

			Scheduler* scheduler = nullptr;
			std::vector<SchedulerQueueFeatures> defaultQueueFeaturesVector{};
			SchedulerQueueFeatures defaultQueueFeature{};
			SchedulerSpecification defaultSpec{};
			defaultSpec.m_Id = m_Schedulers[index].m_Id;
			defaultSpec.m_StartTime = 0;
			SetDefaultCompareOrders(defaultQueueFeature.m_CompareOrder, type);
			defaultQueueFeature.m_Type = type;
			switch (type)
			{
			case SchedulerType::SJF:
				scheduler = new ShortestJobFirstScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 0;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::SRT:
				scheduler = new ShortestRemainingTimeScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 0;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::RR:
				scheduler = new RoundRobinScheduler();
				defaultQueueFeature.m_PreemptionOrder = {
					(uint8_t)Comparators::PriorityHigh,
					(uint8_t)Comparators::BurstLess,
					(uint8_t)Comparators::ProcessIdLess
				};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 2;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::HRRN:
				scheduler = new HighestResponseRatioNextScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 0;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::MLFQ:
				scheduler = new MultiLevelFeedbackQueueScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 3;
				defaultQueueFeature.m_Type = SchedulerType::RR;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::Solaris:
				scheduler = new SolarisScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
				break;
			case SchedulerType::CFS:
				scheduler = new CompletelyFairScheduler();
				defaultQueueFeature.m_PreemptionOrder = {};
				defaultQueueFeature.m_Prop = SchedulerProp::S_NON_PREEMPTIVE;
				defaultQueueFeature.m_Quantum = 3;
				defaultQueueFeaturesVector.push_back(defaultQueueFeature);
				defaultSpec.m_QueueFeatures = defaultQueueFeaturesVector;
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

		std::vector<uint8_t>& IGetSchedulerComparatorList(uint32_t index, uint8_t queueIndex)
		{
			return m_Schedulers[index].m_Scheduler->GetComparatorList(queueIndex);
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
				if (it.m_Scheduler && it.m_Scheduler->IsReadyToGrabResult())
				{
					
					std::vector<ProcessChart> temp = it.m_Scheduler->GetProcessChart();
					MakeContinuous(temp);
					processCharts.push_back(std::make_pair(&it, temp));
			
				}
			}
			return processCharts;
		}
		
		t_SchedulingMetricsVector IGetSchedulingMetrics()
		{
			t_SchedulingMetricsVector schedulingMetrics{};
			for (auto& it : m_Schedulers)
			{
				if (it.m_Scheduler && it.m_Scheduler->IsReadyToGrabResult())
				{
					SchedulingResult tempResult{};
					tempResult.m_SchedulerPtr = &it;
					tempResult.m_SchedulingMetrics = it.m_Scheduler->GetSchedulingMetrics();
					tempResult.m_Throughput = it.m_Scheduler->GetThroughput();
					tempResult.m_Utilization = it.m_Scheduler->GetUtilization();
					tempResult.m_AVGResponse = it.m_Scheduler->GetAVGResponseTime();
					tempResult.m_AVGWaiting = it.m_Scheduler->GetAVGWaitingTime();
					tempResult.m_AVGTurnAround = it.m_Scheduler->GetAVGTurnaroundTime();
					schedulingMetrics.push_back(tempResult);

				}
			}
			return schedulingMetrics;
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


	private:
		std::vector<Process> m_ProcessPool;
		std::vector<SchedulerData> m_Schedulers;
		std::vector<bool> m_AvailableIds;
		uint32_t m_SchedulerLimit;
	};
}//namespace Raven