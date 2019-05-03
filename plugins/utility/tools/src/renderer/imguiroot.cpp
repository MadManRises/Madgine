#include "../toolslib.h"

#include "imguiroot.h"

#ifdef BUILD_PLUGIN_Ogre
#include "ogre/ogreimguimanager.h"
#endif
#ifdef BUILD_PLUGIN_OpenGL
#include "opengl/openglimguimanager.h"
#endif

#include "Madgine/app/application.h"

#include "gui/guisystem.h"
#include "gui/widgets/toplevelwindow.h"
#include "Interfaces/math/vector3.h"

#include "input/inputhandler.h"

#include "Interfaces/debug/profiler/profiler.h"

#include "../toolbase.h"

#include "Interfaces/math/bounds.h"

#include "Interfaces/plugins/pluginmanager.h"

#include "Interfaces/signalslot/taskqueue.h"

#include "../imgui/imgui.h"

#include "imguimanager.h"


UNIQUECOMPONENT(Engine::Tools::ImGuiRoot);

namespace Engine {

	namespace Tools {

		std::unique_ptr<ImGuiManager> createOpenGlManager(App::Application &);

		ImGuiRoot::ImGuiRoot(App::Application &app) :
		Scope(app),
		mCollector(*this)
		{
		}

		ImGuiRoot::~ImGuiRoot()
		{
			
		}


		bool ImGuiRoot::init()
		{
#ifndef STATIC_BUILD
			Plugins::PluginManager::getSingleton()["Renderer"].addListener(this);
#else
			createManager();
#endif
			
			return true;
		}

		void ImGuiRoot::finalize()
		{
#ifndef STATIC_BUILD
			Plugins::PluginManager::getSingleton()["Renderer"].removeListener(this);
#else
			destroyManager();
#endif
		}

		bool ImGuiRoot::frameStarted(std::chrono::microseconds timeSinceLastFrame)
		{
			mManager->newFrame((float)timeSinceLastFrame.count() / 1000000.0f);
			
			return true;
		}

		bool ImGuiRoot::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
		{
			PROFILE_NAMED("ImGui - Rendering");


			if (ImGui::BeginMainMenuBar()) {

				mManager->setMenuHeight(ImGui::GetWindowSize().y);

				if (ImGui::BeginMenu("File")) {
					if (ImGui::MenuItem("Quit")) {
						app().shutdown();
					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Tools")) {
					for (const std::unique_ptr<ToolBase> &tool : mCollector)
					{
						bool visible = tool->isVisible();
						ImGui::MenuItem(tool->key(), "", &visible);
						tool->setVisible(visible);
					}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}

			for (const std::unique_ptr<ToolBase> &tool : mCollector)
			{
				tool->update();
			}

			/*if (ImGui::Begin("Test"))
			{
				ImGui::DragFloat2("Scale", &ImGui::GetIO().DisplayFramebufferScale.x, 0.1f, 0.1f, 2.0f);
			}
			ImGui::End();*/

			return true;
		}

#ifndef STATIC_BUILD
		bool ImGuiRoot::aboutToUnloadPlugin(const Plugins::Plugin * p)
		{
			app().frameLoop().queue([this]() {
				destroyManager();
			});
			return false;
		}

		void ImGuiRoot::onPluginLoad(const Plugins::Plugin * p)
		{
			createManager();
		}
#endif

		void ImGuiRoot::createManager()
		{
			assert(!mManager);
			IF_PLUGIN(OpenGL)
			mManager = createOpenGlManager(app(false));
			else
			THROW_PLUGIN("No ImGui-Manager available!");
			mManager->init();
			//mManager->newFrame(0.0f);
			app(false).addFrameListener(this);
		}

		void ImGuiRoot::destroyManager()
		{
			assert(mManager);
			app(false).removeFrameListener(this);
			ImGui::EndFrame();
			mManager->finalize();
			mManager.reset();
		}

	}
}

