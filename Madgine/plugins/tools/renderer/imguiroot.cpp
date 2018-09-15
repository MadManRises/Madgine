#include "../toolslib.h"

#include "imguiroot.h"

#include "OgreMadgine/scene/ogrescenerenderer.h"

#include "ogre/imguimanager.h"

#include "Madgine/app/clientapplication.h"

#include "Madgine/gui/guisystem.h"
#include "Madgine/gui/widgets/toplevelwindow.h"
#include "Madgine/math/vector3.h"

#include "Madgine/input/inputhandler.h"

#include "../toolbase.h"

RegisterClass(Engine::Tools::ImGuiRoot);

namespace Engine {

	namespace Tools {

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
			mManager = std::make_unique<Ogre::ImguiManager>(static_cast<App::ClientApplication&>(app()).gui().topLevelWindows().front()->input()->listener());
			mManager->init(getGlobalAPIComponent<OgreSceneRenderer>().getSceneManager());
			app().addFrameListener(this);
			static_cast<App::ClientApplication&>(app()).gui().topLevelWindows().front()->input()->setListener(mManager.get());
			return true;
		}

		void ImGuiRoot::finalize()
		{
			mManager.reset();
		}

		bool ImGuiRoot::frameStarted(std::chrono::microseconds timeSinceLastFrame)
		{
			Vector3 size = static_cast<App::ClientApplication&>(app()).gui().topLevelWindows().front()->getScreenSize();
			mManager->newFrame((float)timeSinceLastFrame.count() / 1000000.0f, Ogre::Rect(0, 0, size.x, size.y));

			if (ImGui::BeginMainMenuBar()) {
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
				if (tool->isVisible())
					tool->render();
			}

			return true;
		}

		bool ImGuiRoot::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
		{
			
			return true;
		}
	}
}
