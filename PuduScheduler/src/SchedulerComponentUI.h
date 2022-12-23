#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "Raven/Process/ProcessHandler.h"
#include "Raven/Raven.h"
namespace UI
{
	using enum_Scheduler = SchedulerType;

	static std::vector<const char*> schedulerNames
	{
		"None",
		"Shortest Job First",
		"Shortest Remaining Time",
		"Round Robin Scheduler",
		"Highest Response Ratio Next",
		"Multi Level Feedback Queue",
		"Completely Fair Scheduler",
		"Solaris Scheduler"
	};

	static std::vector<const char*> schedulerAbbrevations
	{
		"None",
		"SJF",
		"SRT",
		"RR",
		"HRRN",
		"MLFQ",
		"CFS",
		"Solaris"
	};

	static std::vector<const char*> processClassNames
	{
		"Time_Sharing",
		"Interactive",
		"Fair_Share",
		"Fixed_Priority",
		"System",
		"Real_Time"
	};
	
	static std::vector<std::pair<const char*, bool>> comparatorOptions
	{
		{"BurstLess",true},
		{"BurstHigh",true},
		{"RemainingTimeLess",true},
		{"RemainingTimeHigh",true},
		{"PriorityLess",true},
		{"PriorityHigh",true},
		{"ArrivalLess",true},
		{"ArrivalHigh",true},
		{"ProcessIdLess",true},
		{"ProcessIdHigh",true},
		{"QuantumUsageLess",true},
		{"QuantumUsageHigh",true},
		{"ResponseRatioLess",true},
		{"ResponseRatioHigh",true},
		{"VRunTimeLess",true},
		{"VRunTimeHigh",true},
		{"NiceValueLess",true},
		{"NiceValueHigh",true},
		{"GlobalPriorityLess",true},
		{"GlobalPriorityHigh",true},
	};


	void DrawSchedulerProperties(int32_t& expandedScheduler);
	void DrawProcessProperties(int32_t& expandedProcess);
	void DrawMLFQProperties(int32_t& expandedSchedulerIndex);
	void DrawQueueProperties(SchedulerQueueFeatures& expandedSchedulerIndex);
	
	void DrawSchedulers()
	{

		static int32_t deletedIdx = -1;
		static int32_t expandedIdx = -1;
		static uint8_t maxNumberOfAllowedSchedulers = 100;
		static bool isColorPushed = false;
		static bool isDisableRequired = false;
		const auto& schedulerList = Raven::SchedulerHandler::GetSchedulerMap();

		ImGui::TextColored(ImVec4(0.0f, 0.1f, 1.0f, 1.0f), "Schedulers");
		static bool runnable = false;

		for (const auto& it : schedulerList)
		{
			runnable = true;
			if (it.m_Type == SchedulerType::None)
			{
				runnable = false;
				break;
			}
		}

		if (runnable)
		{
			
			if(ImGui::Button(ICON_FA_CIRCLE_PLAY "RUN ALL", ImVec2(325, 30)))
			{
				for (uint32_t i = 0; i < schedulerList.size(); ++i)
				{
					Raven::SchedulerHandler::Run(Raven::SchedulerHandler::GetScheduler(i).m_Id);
				}
			}
		}
		for (uint32_t n = 0; n < schedulerList.size(); ++n)
		{
			
			ImGui::BeginGroup();
			ImGui::PushID(n);
			
			if (expandedIdx != -1)
			{
				isDisableRequired = true;
				ImGui::BeginDisabled();
			}

			if (expandedIdx == n)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
				isColorPushed = true;
			}


			if (ImGui::Button(schedulerNames[(uint8_t)schedulerList[n].m_Type], ImVec2(250, 30)))
			{
				ImGui::OpenPopup("SchedulerSelection");
			}
			ImGui::SameLine(0.0f, 0.0f);

			if (isColorPushed)
			{
				ImGui::PopStyleColor();
				isColorPushed = false;
			}

			if (ImGui::Button(ICON_FA_TRASH "", ImVec2(25, 30)))
			{
				deletedIdx = n;
			}

			ImGui::SameLine(0.0f, 0.0f);
			if (schedulerList[n].m_Type == enum_Scheduler::None)
				ImGui::BeginDisabled();

			if (expandedIdx == n)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
				isColorPushed = true;
			}
			
			if (isDisableRequired && expandedIdx == n)
				ImGui::EndDisabled();

			if (ImGui::Button(ICON_FA_EXPAND "", ImVec2(25, 30)))
			{
				if (expandedIdx == -1)
					expandedIdx = n;
				else
					expandedIdx = -1;
			}
			if (isDisableRequired && (expandedIdx == n || expandedIdx == -1))
				ImGui::BeginDisabled();
			if (isColorPushed)
			{
				ImGui::PopStyleColor();
				isColorPushed = false;
			}
			
			ImGui::SameLine(0.0f, 0.0f);
			if (ImGui::Button(ICON_FA_CIRCLE_PLAY "", ImVec2(25, 30)))
			{
				Raven::SchedulerHandler::Run(Raven::SchedulerHandler::GetScheduler(n).m_Id);
			}
			
			if (schedulerList[n].m_Type == enum_Scheduler::None)
				ImGui::EndDisabled();

			if (isDisableRequired)
			{
				ImGui::EndDisabled();
				isDisableRequired = false;
			}

			if (expandedIdx == n)
			{
				if (schedulerList[n].m_Type == SchedulerType::MLFQ)
				{
					DrawMLFQProperties(expandedIdx);
				}
				else {
					DrawSchedulerProperties(expandedIdx);

				}
			}
			ImGui::EndGroup();

			if (ImGui::BeginPopup("SchedulerSelection"))
			{
				ImGui::Text("Schedulers");
				ImGui::Separator();

				// Do not access None 
				for (uint32_t idx = 1; idx < schedulerNames.size(); ++idx)
				{
					
					if (ImGui::Selectable(schedulerNames[idx]))
						Raven::SchedulerHandler::SetScheduler(n,(enum_Scheduler)idx);
					
				}
				ImGui::EndPopup();
			}
			if (expandedIdx == -1)
			{

				// Our buttons are both drag sources and drag targets here!
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None | ImGuiDragDropFlags_SourceAllowNullID))
				{
					// Set payload to carry the index of our item (could be anything)
					ImGui::SetDragDropPayload("SchedulerDragDrop", &n, sizeof(int));

					// Display preview (could be anything, e.g. when dragging an image we could decide to display
					// the filename and a small preview of the image, etc.)

					ImGui::Text("Swap %s", schedulerNames[(uint8_t)schedulerList[n].m_Type]);
					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SchedulerDragDrop"))
					{
						IM_ASSERT(payload->DataSize == sizeof(int));
						int payload_n = *(const int*)payload->Data;
						Raven::SchedulerHandler::SwapSchedulers(n, payload_n);

					}
					ImGui::EndDragDropTarget();
				}
			}
			ImGui::PopID();
		}

		if (deletedIdx > -1)
		{
			PS_CORE_INFO("Size Before: {}", schedulerList.size());
		

			Raven::SchedulerHandler::DeleteScheduler(deletedIdx);
			deletedIdx = -1;
			PS_CORE_INFO("Size After: {}", schedulerList.size());
		}

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.1f, 1.0f, 1.0f));

		if (schedulerList.size() < maxNumberOfAllowedSchedulers)
			if (ImGui::Button(ICON_FA_CIRCLE_PLUS " Add Scheduler", ImVec2(325, 30)))
				Raven::SchedulerHandler::AllocateForScheduler();

		ImGui::PopStyleColor();
	}

	void DrawSchedulerProperties(int32_t& expandedSchedulerIndex)
	{
		//ImGui::Text("%s", schedulerNames[(uint8_t)expandedScheduler.first]);

		constexpr uint32_t maxNumberOfComparators = 7;
		static int deletedIdx = -1;
		auto& expandedScheduler = Raven::SchedulerHandler::GetScheduler(expandedSchedulerIndex);
		uint32_t& startTime = expandedScheduler.m_Scheduler->GetStartTimeRef();
		uint8_t id = expandedScheduler.m_Id;
		std::vector<uint8_t>& comparatorList = expandedScheduler.m_Scheduler->GetComparatorList(0 /*FIXME*/);

		for (const auto& idx : comparatorList)
		{
			if (idx == UINT8_MAX)
				continue;

			comparatorOptions[idx].second = false;
			if (idx % 2 == 0)
			{
				comparatorOptions[idx + 1].second = false;
			}
			else
			{
				comparatorOptions[idx - 1].second = false;
			}
		}

		ImGui::Text("pID: %d", id);

		ImGui::SetNextItemWidth(225);
		ImGui::InputScalar("Start Time", ImGuiDataType_S32, &startTime, NULL, NULL, "%u");
		//ImGui::InputScalar("ID", ImGuiDataType_S8, &id, NULL, NULL, "%u");
		// Only RR and CFS has quantum
		if (expandedScheduler.m_Type == SchedulerType::RR || expandedScheduler.m_Type == SchedulerType::CFS)
		{
			uint32_t& quantum = expandedScheduler.m_Scheduler->GetQuantumRef(0 /* FIXME*/);
			ImGui::SetNextItemWidth(225);
			ImGui::InputScalar("Quantum", ImGuiDataType_S32, &quantum, NULL, NULL, "%u");
		}
		

		ImGui::Text("Comparators: ");
		const char* buttonText = "";
		for (uint32_t n = 0; n < comparatorList.size(); ++n)
		{
			ImGui::BeginGroup();
			ImGui::PushID(n);

			if (comparatorList[n] == UINT8_MAX)
			{
				buttonText = "<Empty>";
			}
			else {
				buttonText = comparatorOptions[comparatorList[n]].first;
			}

			if (ImGui::Button(buttonText, ImVec2(200, 30)))
			{
				ImGui::OpenPopup("ComparatorSelection");
			}
			ImGui::SameLine(0.0f, 0.0f);
			if (ImGui::Button(ICON_FA_TRASH "", ImVec2(25, 30)))
			{
				deletedIdx = n;
			}

			ImGui::EndGroup();
			if (ImGui::BeginPopup("ComparatorSelection"))
			{
				ImGui::Text("Comparators");
				ImGui::Separator();

				for (uint32_t idx = 0; idx < comparatorOptions.size(); ++idx)
				{
					if (comparatorOptions[idx].second)
					{
						if (ImGui::Selectable(comparatorOptions[idx].first))
						{
							comparatorList[n] = idx;
							comparatorOptions[idx].second = false;
							if (idx % 2 == 0)
							{
								comparatorOptions[idx + 1].second = false;
							}
							else
							{
								comparatorOptions[idx - 1].second = false;
							}
						}
					}
				}
				ImGui::EndPopup();
			}

			// Our buttons are both drag sources and drag targets here!
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None | ImGuiDragDropFlags_SourceAllowNullID))
			{
				// Set payload to carry the index of our item (could be anything)
				ImGui::SetDragDropPayload("SchedulerPropertyDragDrop", &n, sizeof(int));

				// Display preview (could be anything, e.g. when dragging an image we could decide to display
				// the filename and a small preview of the image, etc.)
				
				ImGui::Text("Swap %s", comparatorList[n] == UINT8_MAX ? "<Empty>" : comparatorOptions[comparatorList[n]].first);
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SchedulerPropertyDragDrop"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					std::swap(comparatorList[n], comparatorList[payload_n]);

				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
		}

		if (deletedIdx > -1)
		{
			PS_CORE_INFO("Size Before: {}", comparatorList.size());
			if (comparatorList[deletedIdx] != -1)
			{
				uint8_t index = comparatorList[deletedIdx];
				comparatorOptions[index].second = true;
				if (index % 2 == 0)
				{
					comparatorOptions[index + 1].second = true;
				}
				else
				{
					comparatorOptions[index - 1].second = true;
				}
			}

			comparatorList.erase(std::next(comparatorList.begin(), deletedIdx));
			deletedIdx = -1;
			PS_CORE_INFO("Size After: {}", comparatorList.size());
		}

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.1f, 1.0f, 1.0f));
		if (comparatorList.size() < maxNumberOfComparators)
			if (ImGui::Button(ICON_FA_CIRCLE_PLUS " Add Comparator", ImVec2(225, 30)))
				comparatorList.push_back(UINT8_MAX);
		

		for (auto& it : comparatorOptions)
			it.second = true;

		ImGui::PopStyleColor();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY()+10);
	}

	void DrawMLFQProperties(int32_t& expandedSchedulerIndex)
	{
		auto& expandedScheduler = Raven::SchedulerHandler::GetScheduler(expandedSchedulerIndex);
		std::vector<SchedulerQueueFeatures>& queueFeatures = expandedScheduler.m_Scheduler->GetQueueFeatures( /*FIXME*/);
		uint32_t maxNumberOfQueues = 16;
		SchedulerQueueFeatures tempQueueFeatures{};
		static int deletedIdx = -1;
		static int32_t expandedIdx = -1;
		static bool isLastQueueSet = false;

		ImGui::Text("Queues");
		static std::vector<std::pair<const char*, bool>> comparatorOptions
		{
			{"BurstLess",true},
			{"BurstHigh",true},
			{"RemainingTimeLess",true},
			{"RemainingTimeHigh",true},
			{"PriorityLess",true},
			{"PriorityHigh",true},
			{"ArrivalLess",true},
			{"ArrivalHigh",true},
			{"ProcessIdLess",true},
			{"ProcessIdHigh",true},
			{"QuantumUsageLess",true},
			{"QuantumUsageHigh",true},
			{"ResponseRatioLess",true},
			{"ResponseRatioHigh",true}
		};

		//ImGui::SetNextItemWidth(225);
		//ImGui::InputScalar("Start Time", ImGuiDataType_S32, &startTime, NULL, NULL, "%u");
		//ImGui::InputScalar("ID", ImGuiDataType_S8, &id, NULL, NULL, "%u");
		//ImGui::SetNextItemWidth(225);
		//ImGui::InputScalar("Quantum", ImGuiDataType_S32, &quantum, NULL, NULL, "%u");
		//const char* buttonText = "";
		for (uint32_t n = 0; n < queueFeatures.size(); ++n)
		{
			ImGui::BeginGroup();
			ImGui::PushID(n);

			
			std::string buttonText = std::format("{} ", schedulerAbbrevations[(uint32_t)queueFeatures[n].m_Type]);
			if (queueFeatures[n].m_Type == SchedulerType::RR)
				buttonText += std::format("(q={})", queueFeatures[n].m_Quantum);

			if (ImGui::Button(buttonText.c_str(), ImVec2(200, 30)) && n == queueFeatures.size() - 1)
			{
				PS_CORE_INFO("Last button only ! ");
				ImGui::OpenPopup("SchedulerSelection");
				// Here add last scheduler selection
				// If there will be change then set isLastQueueSet
			}
			ImGui::SameLine(0.0f, 0.0f);
			if (ImGui::Button(ICON_FA_TRASH "", ImVec2(25, 30)))
			{
				deletedIdx = n;
			}
			ImGui::SameLine(0.0f, 0.0f);
			if (ImGui::Button(ICON_FA_EXPAND "", ImVec2(25, 30)))
			{
				if (expandedIdx == -1)
					expandedIdx = n;
				else
					expandedIdx = -1;
			}

			if (expandedIdx == n)
			{
				PS_CORE_INFO("Draw queue properties scheduler");
				DrawQueueProperties(queueFeatures[n]);
			}

			ImGui::EndGroup();

			if (n != queueFeatures.size() - 1) 
			{
				// Our buttons are both drag sources and drag targets here!
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None | ImGuiDragDropFlags_SourceAllowNullID))
				{
					// Set payload to carry the index of our item (could be anything)
					ImGui::SetDragDropPayload("SchedulerPropertyDragDrop", &n, sizeof(int));

					// Display preview (could be anything, e.g. when dragging an image we could decide to display
					// the filename and a small preview of the image, etc.)

					//ImGui::Text("Swap %s", comparatorList[n] == UINT8_MAX ? "<Empty>" : comparatorOptions[comparatorList[n]].first);
					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SchedulerPropertyDragDrop"))
					{
						IM_ASSERT(payload->DataSize == sizeof(int));
						int payload_n = *(const int*)payload->Data;
						std::swap(queueFeatures[n], queueFeatures[payload_n]);
					}
					ImGui::EndDragDropTarget();
				}

			}

			if (ImGui::BeginPopup("SchedulerSelection"))
			{
				ImGui::Text("Schedulers");
				ImGui::Separator();

				// Do not access None 
				for (uint32_t idx = 1; idx < schedulerNames.size(); ++idx)
				{
					if (idx == 5)
						continue;

					if (ImGui::Selectable(schedulerNames[idx]))
					{
						queueFeatures[n].m_Quantum = 0;
						queueFeatures[n].m_Type = (SchedulerType)idx;
						Raven::SchedulerHandler::SetDefaultCompareOrders(queueFeatures[n].m_CompareOrder, queueFeatures[n].m_Type);
						if (queueFeatures[n].m_Type == SchedulerType::RR)
							isLastQueueSet = false;
						else
							isLastQueueSet = true;

					}
						//Raven::SchedulerHandler::SetScheduler(n, (enum_Scheduler)idx);

				}
				ImGui::EndPopup();
			}
			
			ImGui::PopID();
		}

#if 1
		if (deletedIdx > -1)
		{
			//PS_CORE_INFO("Size Before: {}", comparatorList.size());
			if (deletedIdx == queueFeatures.size() - 1)
				isLastQueueSet = false;

			auto queueFeaturesIterator = std::next(queueFeatures.begin(), deletedIdx);
			queueFeatures.erase(queueFeaturesIterator);
			deletedIdx = -1;
		}
#endif
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.1f, 1.0f, 1.0f));
		if (queueFeatures.size() < maxNumberOfQueues)
			if (ImGui::Button(ICON_FA_CIRCLE_PLUS " Add Queue", ImVec2(225, 30)))
			{
				tempQueueFeatures.m_Quantum = 3;
				tempQueueFeatures.m_Type = SchedulerType::RR;
				Raven::SchedulerHandler::SetDefaultCompareOrders(tempQueueFeatures.m_CompareOrder, tempQueueFeatures.m_Type);
				queueFeatures.push_back(tempQueueFeatures);
				if (isLastQueueSet)
					std::swap(queueFeatures[queueFeatures.size() - 1], queueFeatures[queueFeatures.size() - 2]);
				PS_CORE_INFO("Queue Size = {}", queueFeatures.size());
			}


		ImGui::PopStyleColor();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	}

	void DrawQueueProperties(SchedulerQueueFeatures& expandedQueue)
	{
		constexpr uint32_t maxNumberOfComparators = 7;
		static int deletedIdx = -1;

		uint32_t& quantum = expandedQueue.m_Quantum;
		std::vector<uint8_t>& comparatorList = expandedQueue.m_CompareOrder;

		//ImGui::Text("queueID: %d", id);

		ImGui::SetNextItemWidth(225);
		ImGui::InputScalar("Quantum", ImGuiDataType_S32, &quantum, NULL, NULL, "%u");

		for (const auto& idx : comparatorList)
		{
			if (idx == UINT8_MAX)
				continue;

			comparatorOptions[idx].second = false;
			if (idx % 2 == 0)
			{
				comparatorOptions[idx + 1].second = false;
			}
			else
			{
				comparatorOptions[idx - 1].second = false;
			}
		}

		ImGui::Text("Comparators: ");
		const char* buttonText = "";
		for (uint32_t n = 0; n < comparatorList.size(); ++n)
		{
			ImGui::BeginGroup();
			ImGui::PushID(n);

			if (comparatorList[n] == UINT8_MAX)
			{
				buttonText = "<Empty>";
			}
			else {
				buttonText = comparatorOptions[comparatorList[n]].first;
			}

			if (ImGui::Button(buttonText, ImVec2(200, 30)))
			{
				ImGui::OpenPopup("ComparatorSelection");
			}
			ImGui::SameLine(0.0f, 0.0f);
			if (ImGui::Button(ICON_FA_TRASH "", ImVec2(25, 30)))
			{
				deletedIdx = n;
			}

			ImGui::EndGroup();
			if (ImGui::BeginPopup("ComparatorSelection"))
			{
				ImGui::Text("Comparators");
				ImGui::Separator();

				for (uint32_t idx = 0; idx < comparatorOptions.size(); ++idx)
				{
					if (comparatorOptions[idx].second)
					{
						if (ImGui::Selectable(comparatorOptions[idx].first))
						{
							comparatorList[n] = idx;
							comparatorOptions[idx].second = false;
							if (idx % 2 == 0)
							{
								comparatorOptions[idx + 1].second = false;
							}
							else
							{
								comparatorOptions[idx - 1].second = false;
							}
						}
					}
				}
				ImGui::EndPopup();
			}

			// Our buttons are both drag sources and drag targets here!
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None | ImGuiDragDropFlags_SourceAllowNullID))
			{
				// Set payload to carry the index of our item (could be anything)
				ImGui::SetDragDropPayload("SchedulerPropertyDragDrop", &n, sizeof(int));

				// Display preview (could be anything, e.g. when dragging an image we could decide to display
				// the filename and a small preview of the image, etc.)

				ImGui::Text("Swap %s", comparatorList[n] == UINT8_MAX ? "<Empty>" : comparatorOptions[comparatorList[n]].first);
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SchedulerPropertyDragDrop"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					std::swap(comparatorList[n], comparatorList[payload_n]);

				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
		}

		if (deletedIdx > -1)
		{
			PS_CORE_INFO("Size Before: {}", comparatorList.size());
			if (comparatorList[deletedIdx] != -1)
			{
				uint8_t index = comparatorList[deletedIdx];
				comparatorOptions[index].second = true;
				if (index % 2 == 0)
				{
					comparatorOptions[index + 1].second = true;
				}
				else
				{
					comparatorOptions[index - 1].second = true;
				}
			}

			comparatorList.erase(std::next(comparatorList.begin(), deletedIdx));
			deletedIdx = -1;
			PS_CORE_INFO("Size After: {}", comparatorList.size());
		}

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.1f, 1.0f, 1.0f));
		if (comparatorList.size() < maxNumberOfComparators)
			if (ImGui::Button(ICON_FA_CIRCLE_PLUS " Add Comparator", ImVec2(225, 30)))
				comparatorList.push_back(UINT8_MAX);


		for (auto& it : comparatorOptions)
			it.second = true;

		ImGui::PopStyleColor();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
	}


	void DrawProcessTab()
	{
		static int32_t deletedIdx = -1;
		static int32_t expandedIdx = -1;
		static uint8_t maxNumberOfAllowedSchedulers = 100;
		static bool isColorPushed = false;
		static bool isDisableRequired = false;
		auto& processPool = Raven::ProcessHandler::GetProcessPool();

		for (uint32_t n = 0; n < processPool.size(); ++n)
		{
			ImGui::BeginGroup();
			ImGui::PushID(n);


			if (expandedIdx == n)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
				isColorPushed = true;
			}


			if (isColorPushed)
			{
				ImGui::PopStyleColor();
				isColorPushed = false;
			}

			
			ImGui::Button(processPool[n].GetProcessLabel().c_str(), ImVec2(200,30));
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_TRASH "", ImVec2(25, 30)))
			{
				deletedIdx = n;
			}

			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_EXPAND "", ImVec2(25, 30)))
			{
				if (expandedIdx == -1)
					expandedIdx = n;
				else
					expandedIdx = -1;
			}

			if (expandedIdx == n)
			{
				DrawProcessProperties(expandedIdx);
			}

			ImGui::EndGroup();
			
			

			// Our buttons are both drag sources and drag targets here!
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None | ImGuiDragDropFlags_SourceAllowNullID))
			{
				// Set payload to carry the index of our item (could be anything)
				ImGui::SetDragDropPayload("DND_DEMO_CELL", &n, sizeof(int));

				// Display preview (could be anything, e.g. when dragging an image we could decide to display
				// the filename and a small preview of the image, etc.)

				ImGui::Text("Swap %s", processPool[n].GetProcessLabel().c_str());
				ImGui::EndDragDropSource();
			}
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_DEMO_CELL"))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					int payload_n = *(const int*)payload->Data;
					Raven::ProcessHandler::SwapProcesses(n, payload_n);

				}
				ImGui::EndDragDropTarget();
			}
			ImGui::PopID();
		}


		if (deletedIdx > -1)
		{
			Raven::ProcessHandler::DeleteProcess(deletedIdx);
			deletedIdx = -1;
		}


		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.1f, 1.0f, 1.0f));

		if (processPool.size() < Raven::ProcessHandler::GetMaxProcessLimit())
			if (ImGui::Button(ICON_FA_CIRCLE_PLUS "", ImVec2(250, 30)))
				Raven::ProcessHandler::GenerateAndSubmit();

		ImGui::PopStyleColor();
	}

	void DrawProcessProperties(int32_t& expandedIndex)
	{
		auto& expandedProcess = Raven::ProcessHandler::GetProcess(expandedIndex);
		ImGui::Text("Process %s", expandedProcess.GetProcessLabel().c_str() );
		uint32_t& arrivalTime = expandedProcess.GetArrivalTimeRef();
		uint32_t& cpuBurstTime = expandedProcess.GetBurstTimeRef();
		uint32_t& priority = expandedProcess.GetPriorityRef();
		uint32_t& vruntime = expandedProcess.GetVRuntimeRef();
		uint32_t& niceValue = expandedProcess.GetNiceValueRef();
		uint32_t globalPriority = expandedProcess.GetGlobalPriority();
		ProcessClass& processClass = expandedProcess.GetProcessClassRef();

		expandedProcess.SetRemainingTime(cpuBurstTime);
		ImGui::SetNextItemWidth(150);
		ImGui::InputScalar("Arrival Time", ImGuiDataType_S32, &arrivalTime, NULL, NULL, "%u");
		ImGui::SetNextItemWidth(150);
		ImGui::InputScalar("CPU Burst", ImGuiDataType_S32, &cpuBurstTime, NULL, NULL, "%u");
		ImGui::SetNextItemWidth(150);
		ImGui::InputScalar("Priority", ImGuiDataType_S32, &priority, NULL, NULL, "%u");
		ImGui::SetNextItemWidth(150);
		ImGui::InputScalar("VRun Time", ImGuiDataType_S32, &vruntime, NULL, NULL, "%u");
		ImGui::SetNextItemWidth(150);
		ImGui::InputScalar("Nice Value", ImGuiDataType_S32, &niceValue, NULL, NULL, "%u");
		ImGui::SetNextItemWidth(150);
		//ImGui::InputScalar("G Priority", ImGuiDataType_S32, &globalPriority, NULL, NULL, "%u");
		//expandedProcess.SetGlobalPriority(globalPriority);

		if (ImGui::Button(std::format("{}", processClassNames[(uint8_t)processClass]).c_str(), ImVec2(150, 0)))
		{
			ImGui::OpenPopup("ProcessClassSelection");
		}
		ImGui::SameLine(0.0f,1.0f);
		ImGui::Text("Class");
		if (ImGui::BeginPopup("ProcessClassSelection"))
		{
			ImGui::Text("Classes");
			ImGui::Separator();

			for (uint32_t idx = 0; idx < processClassNames.size(); ++idx)
			{
				if (ImGui::Selectable(processClassNames[idx]))
				{
						
					processClass = (ProcessClass)idx;
				}
			}
			ImGui::EndPopup();
		}
	}



	void DrawProcessTable()
	{
		auto& processPool = Raven::ProcessHandler::GetProcessPool();

		ImVec2 avail = ImGui::GetContentRegionAvail();
		ImGui::SetCursorPosX((avail.x / 2)-450);
		ImGui::SetCursorPosY(ImGui::GetCursorPos().y+10);
		const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
		const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
		// When using ScrollX or ScrollY we need to specify a size for our table container!
		// Otherwise by default the table will fit all available space, like a BeginChild() call.
		ImVec2 outer_size = ImVec2(900.0f, TEXT_BASE_HEIGHT * 8);
		if (ImGui::BeginTable("table_scrolly", 8, flags, outer_size))
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
			ImGui::TableSetupColumn("Process ID", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Arrival Time", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("CPU Burst", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Priority", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("VRun Time", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Nice Value", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Class", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Global Priority", ImGuiTableColumnFlags_None);
			ImGui::TableHeadersRow();

			// Demonstrate using clipper for large vertical lists
			ImGuiListClipper clipper;
			clipper.Begin(processPool.size());
			while (clipper.Step())
			{
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%d", processPool[row].GetProcessId());

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%d", processPool[row].GetArrivalTime());

					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%d", processPool[row].GetBurstTime());

					ImGui::TableSetColumnIndex(3);
					ImGui::Text("%d", processPool[row].GetPriority());

					ImGui::TableSetColumnIndex(4);
					ImGui::Text("%d", processPool[row].GetVRunTime());

					ImGui::TableSetColumnIndex(5);
					ImGui::Text("%d", processPool[row].GetNiceValue());

					ImGui::TableSetColumnIndex(6);
					ImGui::Text("%s", processClassNames[(uint8_t)processPool[row].GetProcessClass()]);

					ImGui::TableSetColumnIndex(7);
					ImGui::Text("%d", processPool[row].GetGlobalPriority());
				}
			}
			ImGui::EndTable();
		}
	}


	void DrawGanttChart(ImDrawList* drawList, const std::vector<ProcessChart>& timeLine, const ImVec2& startPos, const float height,
		ImU32 rectColor = ImGui::ColorConvertFloat4ToU32(ImVec4{ 0.05f, 0.85f, 0.73f, 0.6f }))
	{
		float maxWidth = 1600;

		int32_t maxTime = (timeLine.back().m_StartTime + timeLine.back().m_Usage);
		float proportion = maxWidth / (float)maxTime;
		ImVec2 upperLeft = startPos;
		ImColor col = ImColor(0.0f, 1.0f, 1.0f);
		ImU32 color = ImGui::ColorConvertFloat4ToU32({ 1,1,1,1 });
		ImU32 green = ImGui::ColorConvertFloat4ToU32({ 0,1,0,0.5 });

		float lineThickness = 1.5f;

		ImFont* font = ImGui::GetFont();
		float temp = font->Scale;
		font->Scale = 0.9;
		ImGui::PushFont(font);

		for (const auto& it : timeLine)
		{
			ImVec2 lowerRight = upperLeft + ImVec2(it.m_Usage* proportion, height);
			
			drawList->AddRectFilled(upperLeft, lowerRight,rectColor );
			std::string a = std::to_string(it.m_StartTime);
			drawList->AddText(upperLeft + ImVec2(-7, 53),color, a.c_str());
			ImVec2 labelCoord{  ( it.m_Usage * proportion) / 2 + upperLeft.x - 10, upperLeft.y + 15 };
			drawList->AddText(labelCoord, ImGui::ColorConvertFloat4ToU32({ 1.0f,1.0f,1.0f,1.0f }), it.m_Label.c_str());
			upperLeft.x += it.m_Usage * proportion;
		}

		upperLeft = startPos;

		for (const auto& it : timeLine)
		{
			ImVec2 lowerRight = upperLeft + ImVec2(it.m_Usage * proportion, height);
			drawList->AddLine(upperLeft, upperLeft + ImVec2(0, 50),color, lineThickness);
			upperLeft.x += it.m_Usage * proportion;
		}
		drawList->AddLine(upperLeft, upperLeft + ImVec2(0, 50), color,lineThickness);
		// Bottom vertical line
		drawList->AddLine(startPos + ImVec2(0,height), startPos+ImVec2(maxWidth, height), color, lineThickness);
		// Top vertical line
		drawList->AddLine(startPos, startPos+ImVec2(maxWidth, 0), color, lineThickness);

		std::string a = std::to_string(maxTime);
		drawList->AddText(upperLeft + ImVec2(-7, 53),color, a.c_str());
		//drawList->AddText(upperLeft + ImVec2(-4, 53), ImGui::ColorConvertFloat4ToU32(color), a.c_str());
		font->Scale = temp;
		ImGui::PopFont();
	}

	std::vector<ProcessChart> PrepareQueueTimelines(const std::vector<ProcessChart>& processChart, uint32_t currentIndex)
	{
		std::vector<ProcessChart> result{};
		ProcessChart temp{};
		for (const auto& it : processChart)
		{
			if (it.m_QueueIndex == currentIndex)
			{
				result.push_back(it);
			}
		}

		if (result.empty())
		{
			temp.m_Label = "Idle";
			temp.m_StartTime = 0;
			temp.m_Usage = (processChart.back().m_StartTime + processChart.back().m_Usage) - temp.m_StartTime;
			result.push_back(temp);
		}
		else if (result.back().m_StartTime + result.back().m_Usage != processChart.back().m_StartTime + processChart.back().m_Usage)
		{
			temp.m_Label = "Idle";
			temp.m_StartTime = result.back().m_StartTime + result.back().m_Usage;
			temp.m_Usage = (processChart.back().m_StartTime + processChart.back().m_Usage) - temp.m_StartTime;
			result.push_back(temp);
		}
		Raven::SchedulerHandler::MakeContinuous(result);
		return result;
	}
	void DrawMid()
	{
		if (!Raven::ProcessHandler::GetProcessPool().empty())
		{
			ImGui::SetNextWindowPos(ImGui::GetCursorStartPos() + ImVec2(430, 50));
			ImGui::BeginChild("Process Table", ImVec2(930, 180), true);
			DrawProcessTable();
			ImGui::EndChild();
		}
		
		ImVec2 startPos = ImGui::GetCursorStartPos() + ImVec2(60,280);

		for (const auto& it : Raven::SchedulerHandler::GetProcessCharts())
		{
			const auto& queueFeatures = it.first->m_Scheduler->GetQueueFeatures();
	
			std::string childName = std::format("{}",(uint32_t)it.first);
			ImGui::SetNextWindowPos(startPos);
			if (it.first->m_Type == SchedulerType::MLFQ)
			{
				ImGui::BeginChild(childName.c_str(), ImVec2(1630, 145 * (queueFeatures.size() + 1)), true);
				startPos.y += 60;
			}
			else
			{
				ImGui::BeginChild(childName.c_str(), ImVec2(1630, 120), true);
				startPos.y += 30;
			}
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			if (it.first->m_Type == SchedulerType::RR || it.first->m_Type == SchedulerType::CFS)
				ImGui::Text("%s sID: %d (q = %u)", schedulerNames[(uint32_t)it.first->m_Type], it.first->m_Id, (uint32_t)it.first->m_Scheduler->GetQuantumRef(0));
			else
				ImGui::Text("%s sID: %d", schedulerNames[(uint32_t)it.first->m_Type], it.first->m_Id);
			
			if (it.first->m_Type == SchedulerType::MLFQ)
			{
				for (uint32_t i = 0; i < queueFeatures.size() + 1; ++i)
				{
					ImGui::SetCursorScreenPos(startPos - ImVec2(-10, 23));
					if (i == queueFeatures.size())
					{
						ImGui::Text("Overall Result");
						DrawGanttChart(drawList, it.second, startPos + ImVec2(10, 0), 50.f, ImGui::ColorConvertFloat4ToU32(ImVec4{ 0.85f, 0.05f, 0.73f, 0.6f })); // FIXME
						break;
					}

					if (queueFeatures[i].m_Type == SchedulerType::RR)
						ImGui::Text("Queue-%d [%s with q = %d]", i + 1, schedulerAbbrevations[(uint8_t)queueFeatures[i].m_Type], queueFeatures[i].m_Quantum);
					else
						ImGui::Text("Queue-%d [%s]", i + 1, schedulerAbbrevations[(uint8_t)queueFeatures[i].m_Type]);

					const auto& queueTimeline = PrepareQueueTimelines(it.second, i);
					DrawGanttChart(drawList, queueTimeline, startPos + ImVec2(10, 0), 50.f); // FIXME
					startPos.y += 120;
				}
			}
			else
			{
				DrawGanttChart(drawList, it.second, startPos + ImVec2(10, 0), 50.f);
			}
			ImGui::EndChild();
			startPos.y += 150;

		}
		

	}




	void DrawDetails()
	{
		ImVec2 avail = ImGui::GetContentRegionAvail();
		ImGui::SetCursorPosX((avail.x / 2) - 600);
		ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 10);
		const auto& schedulingMetrics = Raven::SchedulerHandler::GetSchedulingMetrics();


		ImGui::BeginChild(std::format("OverallSchedulerComparison").c_str(), ImVec2(1230, 240), true);
		ImGui::Text("Overall Scheduler Comparison");

		const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
		const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
		// When using ScrollX or ScrollY we need to specify a size for our table container!
		// Otherwise by default the table will fit all available space, like a BeginChild() call.
		ImVec2 outer_size = ImVec2(1200.0f, TEXT_BASE_HEIGHT * 8);
		if (ImGui::BeginTable("table_scrolly", 6, flags, outer_size))
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
			ImGui::TableSetupColumn("Scheduler ID", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Throughput", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("CPU Utilization", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Average Turnaround Time", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Average Response Time", ImGuiTableColumnFlags_None);
			ImGui::TableSetupColumn("Average Waiting Time", ImGuiTableColumnFlags_None);
			ImGui::TableHeadersRow();

			// Demonstrate using clipper for large vertical lists
			ImGuiListClipper clipper;
			clipper.Begin(schedulingMetrics.size());
			while (clipper.Step())
			{
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%d", schedulingMetrics[row].m_SchedulerPtr->m_Id);

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%.3f Proc Per MS", 100*schedulingMetrics[row].m_Throughput);

					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%.3f%%", (1-schedulingMetrics[row].m_Utilization)*100);

					ImGui::TableSetColumnIndex(3);
					ImGui::Text("%.3f", schedulingMetrics[row].m_AVGTurnAround);

					ImGui::TableSetColumnIndex(4);
					ImGui::Text("%.3f", schedulingMetrics[row].m_AVGResponse);

					ImGui::TableSetColumnIndex(5);
					ImGui::Text("%.3f", schedulingMetrics[row].m_AVGWaiting);
				}
			}
			ImGui::EndTable();
		}
		ImGui::EndChild();


		for (const auto& it : schedulingMetrics)
		{
			ImGui::SetCursorPosX((avail.x / 2) - 450);
			ImGui::SetCursorPosY(ImGui::GetCursorPos().y + 10);

			ImGui::BeginChild(std::format("SchedulerDetail{}",it.m_SchedulerPtr->m_Id).c_str(), ImVec2(930, 240), true);
			ImGui::Text("%s {sId: %d} ", schedulerNames[(uint32_t)it.m_SchedulerPtr->m_Type], it.m_SchedulerPtr->m_Id);
			
			// When using ScrollX or ScrollY we need to specify a size for our table container!
			// Otherwise by default the table will fit all available space, like a BeginChild() call.
			ImVec2 outer_sizeInner = ImVec2(900.0f, TEXT_BASE_HEIGHT * 8);
			if (ImGui::BeginTable("table_scrolly", 4, flags, outer_sizeInner))
			{
				ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
				ImGui::TableSetupColumn("Process ID", ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn("Turnaround Time", ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn("Waiting Time", ImGuiTableColumnFlags_None);
				ImGui::TableSetupColumn("Response Time", ImGuiTableColumnFlags_None);
				ImGui::TableHeadersRow();

				// Demonstrate using clipper for large vertical lists
				ImGuiListClipper clipper;
				clipper.Begin(it.m_SchedulingMetrics.size());
				while (clipper.Step())
				{
					for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
					{
						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%s", it.m_SchedulingMetrics[row].m_Label);

						ImGui::TableSetColumnIndex(1);
						ImGui::Text("%d", it.m_SchedulingMetrics[row].m_TurnaroundTime);

						ImGui::TableSetColumnIndex(2);
						ImGui::Text("%d", it.m_SchedulingMetrics[row].m_WaitingTime);

						ImGui::TableSetColumnIndex(3);
						ImGui::Text("%d", it.m_SchedulingMetrics[row].m_ResponseTime);
					}
				}
				ImGui::EndTable();
			}
			ImGui::EndChild();

			//ImGui::Text("%s %d %d %d",it2.m_Label.c_str(), it2.m_TurnaroundTime, it2.m_WaitingTime, it2.m_ResponseTime);
		
		}
	}


}