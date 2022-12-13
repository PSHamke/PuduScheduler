#pragma once

enum class SchedulerProp : uint32_t
{
	S_NON_PREEMPTIVE,
	S_PREEMPTIVE
};


struct SchedulerSpecification
{
public:

	SchedulerSpecification() // Default constructor
	{

	}
	// 
	SchedulerSpecification(uint32_t id, const std::vector<uint8_t>& compareOrder, const std::vector<uint8_t>& preemptionOrder,
						   uint32_t startTime ,uint32_t quantum, SchedulerProp prop)
		: m_Id(id),m_CompareOrder(compareOrder), m_PreemptionOrder(preemptionOrder),
		  m_StartTime(startTime), m_Quantum(quantum), m_Prop(prop)
	{
	}
	

public:
	uint32_t m_Id;
	std::vector<uint8_t> m_CompareOrder;
	std::vector<uint8_t> m_PreemptionOrder;
	uint32_t m_StartTime;
	uint32_t m_Quantum;
	SchedulerProp m_Prop;
	
};