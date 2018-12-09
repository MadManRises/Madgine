#include "../toolslib.h"

#include "imguiroot.h"

#ifdef BUILD_PLUGIN_Ogre
#include "ogre/ogreimguimanager.h"
#endif
#ifdef BUILD_PLUGIN_OpenGL
#include "opengl/openglimguimanager.h"
#endif

#include "Madgine/app/clientapplication.h"

#include "Madgine/gui/guisystem.h"
#include "Madgine/gui/widgets/toplevelwindow.h"
#include "Interfaces/math/vector3.h"

#include "Madgine/input/inputhandler.h"

#include "Interfaces/debug/profiler/profiler.h"

#include "../toolbase.h"

#include "Interfaces/math/bounds.h"

#include "Interfaces/plugins/pluginmanager.h"

#include "Interfaces/signalslot/connectionmanager.h"

#include "../imgui/imgui.h"


namespace Engine {

	namespace Tools {

		std::unique_ptr<ImGuiManager> createOpenGlManager(App::ClientApplication &);

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

			return true;
		}

		bool ImGuiRoot::aboutToUnloadPlugin(const Plugins::Plugin * p)
		{
			SignalSlot::ConnectionManager::getSingleton().queue([this]() {
				destroyManager();
			});
			return false;
		}

		void ImGuiRoot::onPluginLoad(const Plugins::Plugin * p)
		{
			createManager();
		}

		void ImGuiRoot::createManager()
		{
			assert(!mManager);
			IF_PLUGIN(OpenGL)
			mManager = createOpenGlManager(static_cast<App::ClientApplication&>(app()));
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

