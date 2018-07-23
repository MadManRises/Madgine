#include "maditorviewlib.h"

#include "inspectorwidget.h"

#include "../../qtimgui/QtImGui.h"
#include "../../qtimgui/imgui/imgui.h"

namespace Maditor
{
	namespace View
	{
		InspectorWidget::InspectorWidget(QWidget* parent) :
		QOpenGLWindow()
		{
			startTimer(0);
		}

		void InspectorWidget::initializeGL()
		{
			QtImGui::initialize(this);
			initializeOpenGLFunctions();
		}

		void InspectorWidget::paintGL()
		{
			// you can do custom GL rendering as well in paintGL

			QtImGui::newFrame();

			ImGui::SetNextWindowSize(ImVec2(width(), height()), ImGuiSetCond_Always);
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
			ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings);

			ImGui::Button("hi");
			ImGui::Text("Hello");
			
			ImGui::End();

			// Do render before ImGui UI is rendered
			glViewport(0, 0, width(), height());
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);

			ImGui::Render();
		}

		void InspectorWidget::timerEvent(QTimerEvent* event)
		{
			update();
		}
	}
}