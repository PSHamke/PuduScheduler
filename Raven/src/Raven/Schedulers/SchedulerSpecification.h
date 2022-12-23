#pragma once

enum class SchedulerProp : uint32_t
{
	S_NON_PREEMPTIVE,
	S_PREEMPTIVE
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

struct SchedulerQueueFeatures
{
public:
	SchedulerQueueFeatures()
	{
	}

	SchedulerQueueFeatures(const std::vector<uint8_t>& compareOrder, const std::vector<uint8_t>& preemptionOrder,
		const uint32_t quantum, const SchedulerType type, const SchedulerProp prop)
		: m_CompareOrder(compareOrder), m_PreemptionOrder(preemptionOrder), m_Quantum(quantum), m_Type(type), m_Prop(prop)
	{
	}
public:
	std::vector<uint8_t> m_CompareOrder;
	std::vector<uint8_t> m_PreemptionOrder;
	uint32_t m_Quantum;
	SchedulerType m_Type;
	SchedulerProp m_Prop;
};

struct SchedulerSpecification
{
public:

	SchedulerSpecification() // Default constructor
	{

	}
	// 
	SchedulerSpecification(const uint32_t id, const uint32_t startTime, const std::vector<SchedulerQueueFeatures>& queueFeatures )
		: m_Id(id),m_StartTime(startTime), m_QueueFeatures(queueFeatures)
	{
	}
	

public:
	uint32_t m_Id;
	uint32_t m_StartTime;
	std::vector<SchedulerQueueFeatures> m_QueueFeatures;
};