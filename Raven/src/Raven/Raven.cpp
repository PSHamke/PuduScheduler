#include "Raven.h"
#include "Log/Log.h"
#include "Process/ProcessGenerator.h"
#include "Schedulers/ShortestJobFirstScheduler.h"

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


void Raven::Handle()
{
	Log::Init();
	PS_CORE_INFO("Log");

}

void Raven::DebugTest()
{
	ProcessGenerator pg;

	std::vector<Process> processPool;


	processPool.push_back(pg.Generate("", 3, 3, 5).value());
	processPool.push_back(pg.Generate("", 0, 5, 5).value());
	processPool.push_back(pg.Generate("", 15, 4, 5).value());
	processPool.push_back(pg.Generate("", 3, 3, 5).value());
	processPool.push_back(pg.Generate("", 1, 1, 5).value());

	std::vector<uint8_t> compareOrder{ (uint8_t)Comparators::BurstLess, (uint8_t)Comparators::ArrivalLess,(uint8_t)Comparators::ProcessIdHigh };
	ShortestJobFirstScheduler scheduler{ compareOrder };

	scheduler.SubmitProcessPool(processPool);
	scheduler.Schedule();

	for (const auto it : processPool)
	{
		it.PrintTimeStamps(std::cout);
	}
}
