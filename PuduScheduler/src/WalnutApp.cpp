#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Raven/Raven.h"
#include "Renderer.h"
#include "Walnut/ImGui/FontAwesome.h"
#include "Log/Log.h"
#include "SchedulerComponentUI.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"


namespace ImGui {
	bool GoxTab(const char* text, bool* v)
	{
		ImFont* font = GImGui->Font;
		const ImFontGlyph* glyph;
		char c;
		bool ret;
		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		float pad = style.FramePadding.x;
		ImVec4 color;
		ImVec2 text_size = CalcTextSize(text);
		ImGuiWindow* window = GetCurrentWindow();
		ImVec2 pos = window->DC.CursorPos + ImVec2(pad, text_size.x + pad);

		color = style.Colors[ImGuiCol_Button];
		if (*v) color = style.Colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_Button, color);
		ImGui::PushID(text);
		ret = ImGui::Button("", ImVec2(text_size.y + pad * 2,
			text_size.x + pad * 2));
		ImGui::PopStyleColor();
		while ((c = *text++)) {
			glyph = font->FindGlyph(c);
			if (!glyph) continue;

			window->DrawList->PrimReserve(6, 4);
			window->DrawList->PrimQuadUV(
				pos + ImVec2(glyph->Y0, -glyph->X0),
				pos + ImVec2(glyph->Y0, -glyph->X1),
				pos + ImVec2(glyph->Y1, -glyph->X1),
				pos + ImVec2(glyph->Y1, -glyph->X0),

				ImVec2(glyph->U0, glyph->V0),
				ImVec2(glyph->U1, glyph->V0),
				ImVec2(glyph->U1, glyph->V1),
				ImVec2(glyph->U0, glyph->V1),
				0xFFFFFFFF);
			pos.y -= glyph->AdvanceX;
		}
		ImGui::PopID();
		return ret;
	}
}



using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	std::shared_ptr<Walnut::Image> m_ImageOrig;
	virtual void OnAttach() {
		m_ImageOrig = std::make_shared<Walnut::Image>(1, 1,ImageFormat::RGBA);
		Raven::Handle();
		PS_CORE_INFO("Log From Pudu Scheduler");
		//Raven::Start();
	}

	virtual void OnUIRender() override
	{

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
		static ImVec2 item_spacing{ 0.0f, 0.0f };
		ImGui::Begin("Scheduler Explorer", nullptr, ImGuiWindowFlags_NoDecoration );
		//ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		//UI::DrawSchedulers();
		static float alignment = 0.5f;
		ImVec2 widget_size;
		widget_size.x = (ImGui::GetContentRegionAvail().x / 50);
		widget_size.y = ImGui::GetContentRegionAvail().y;
		ImVec2 layoutSize = ImGui::GetContentRegionAvail();

		static ImVec2 widgetASize = widget_size;
		static ImVec2 widgetCSize = widget_size;
		static ImVec2 widgetBSize = { widget_size.x * 48, widget_size.y};
		static float a_c_spring_weight = 0.0f;
		static float ab_spring_weight = 0.5f;
		static bool isProcessPressed = false;
		static bool isSchedulerPressed = false;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0.f,0.f });
		ImGui::BeginHorizontal("h1", layoutSize, alignment);
		ImGui::Spring(a_c_spring_weight); 
		ImGui::BeginChild("Widget A", widgetASize);
		{
			static bool p = false;
			ImGui::SetCursorPosY(40.0f);
			static bool isPushed = false;
			
			if (isProcessPressed)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
				isPushed = true;
			}
			if (ImGui::GoxTab("Processes", &p))
			{
				isProcessPressed = !isProcessPressed;
				
				PS_CORE_INFO("Tab hamke pressed ? ");
			}
			if (isPushed)
			{
				ImGui::PopStyleColor();
				isPushed = false;
			}
			if (isProcessPressed)
			{
				ImGui::SetNextWindowPos({ ImGui::GetContentRegionAvail().x, 53 });
				ImGui::SetNextWindowSize({ 250, 923 });
				ImGui::Begin("ProcessTab",NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse );
				ImGui::Text("Processs");
				UI::DrawProcessTab();
				ImGui::End();

			}
			//UI::DrawSchedulers();
		}
		ImGui::EndChild();
		ImGui::Spring(ab_spring_weight);
		ImGui::BeginChild("Widget B", widgetBSize,false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysHorizontalScrollbar);
		{
			ImVec2 avail = ImGui::GetContentRegionAvail();
			ImGui::SetCursorPosX((avail.x/2)-150);
			
			UI::DrawMid();
			//UI::DrawSchedulers();
		}
		ImGui::EndChild();

		ImGui::Spring(1.0f-ab_spring_weight);
		
		ImGui::BeginChild("Widget C", widgetCSize);
		{
			static bool p = false;
			ImGui::SetCursorPosY(40.0f);
			
			static bool isPushed = false;

			if (isSchedulerPressed)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
				
				isPushed = true;
			}
			if (ImGui::GoxTab("Schedulers", &p))
			{
				isSchedulerPressed = !isSchedulerPressed;

				PS_CORE_INFO("Tab hamke pressed ? ");
			}
			if (isPushed)
			{
				ImGui::PopStyleColor(2);
				isPushed = false;
			}
			if (isSchedulerPressed)
			{
				ImGui::SetNextWindowPos({ 1375, 53 });
				ImGui::SetNextWindowSize({ 325, 923 });
				ImGui::Begin("SchedulersTab", NULL, ImGuiWindowFlags_NoDecoration);
				
				UI::DrawSchedulers();
				ImGui::End();

			}
			//UI::DrawSchedulers();
		}
		ImGui::EndChild();
		ImGui::Spring(a_c_spring_weight);

		ImGui::EndHorizontal();

		ImGui::PopStyleVar(1);

		ImGui::End();
		ImGui::PopStyleVar();

		if (!Raven::SchedulerHandler::GetSchedulingMetrics().empty())
		{
			ImGui::Begin("Details", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing);
			if (ImGui::IsWindowFocused())
			{
				isSchedulerPressed = false;
				isProcessPressed = false;
			}
			UI::DrawDetails();
			ImGui::End();
		}

		if (!Raven::SchedulerHandler::GetSchedulingMetrics().empty())
		{
			ImGui::Begin("Charts", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing);
			if (ImGui::IsWindowFocused())
			{
				isSchedulerPressed = false;
				isProcessPressed = false;
			}
			UI::DrawPlots();
			ImGui::BeginChild("My Window");
			ImPlot::ShowDemoWindow();
			ImGui::EndChild();
			ImGui::End();
		}

//		ImGui::ShowDemoWindow();

		Render();
	}



	void TestUI()
	{
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
	spec.Name = "Pudu Scheduler";
	spec.Height =980;
	spec.Width = (spec.Height * 16) / 9 ;
#ifdef PS_DEBUG

#endif
	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("PUDU"))
			{
				if (ImGui::MenuItem("Load Presentation 01"))
				{
					Raven::SchedulerHandler::LoadPresentation();
					Raven::ProcessHandler::LoadPresentation();
				}
				if (ImGui::MenuItem("Information"))
				{
					UI::DrawInformation();
				}
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}