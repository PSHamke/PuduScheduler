#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Raven/Raven.h"
#include "Renderer.h"

#include "Log/Log.h"


using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	std::shared_ptr<Walnut::Image> m_ImageOrig;
	virtual void OnAttach() {
		m_ImageOrig = std::make_shared<Walnut::Image>(1, 1,ImageFormat::RGBA);
		Raven::Handle();
		PS_CORE_INFO("Log From Pudu Scheduler");
		
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
		{
			Render();
			Timer timer;
			timer.Reset();
			Raven::DebugTest();
			float elapsed = timer.ElapsedMillis();
			PS_CORE_INFO("Elapsed Time: {} MS", elapsed);
			Raven::SchedulerHandler::FillTestProcess();
			Raven::SchedulerHandler::AddScheduler(0, Raven::SchedulerType::SJF, { (uint8_t)Raven::Comparators::BurstLess, (uint8_t)Raven::Comparators::ArrivalLess,(uint8_t)Raven::Comparators::ProcessIdHigh });
			Raven::SchedulerHandler::Run(0);
			Raven::SchedulerHandler::CleanUp();
		}
		
		ImGui::End();
		//ImGui::ShowDemoWindow();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;


		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		//m_Renderer.Render();

		m_LastRenderTime = timer.ElapsedMillis();
	}


private:
	Renderer m_Renderer;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "PuduScheduler";
#ifdef PS_DEBUG

#endif
	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}