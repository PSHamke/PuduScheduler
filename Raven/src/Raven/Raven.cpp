#include "Raven.h"
#include "Log/Log.h"



void Raven::Handle()
{
	Log::Init();
	PS_CORE_INFO("Log");

}

void Raven::DebugTest()
{







	//std::vector<uint8_t> compareOrder{ (uint8_t)Comparators::BurstLess, (uint8_t)Comparators::ArrivalLess,(uint8_t)Comparators::ProcessIdHigh };
	//ShortestJobFirstScheduler scheduler{ compareOrder };

	//std::vector<uint8_t> compareOrder6{ (uint32_t)Comparators::ResponseRatioHigh,(uint8_t)Comparators::RemainingTimeLess,(uint8_t)Comparators::ProcessIdHigh };
	//HighestResponseRatioNextScheduler scheduler{ compareOrder6, SchedulerSpec::S_PREEMPTIVE };

	//scheduler.SubmitProcessPool(processPool);
	//scheduler.Schedule();

	//for (const auto it : processPool)
	//{
	//	it.PrintTimeStamps(std::cout);
	//}
}
