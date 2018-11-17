#include "../clientlib.h"

#include "clientapplication.h"

#include "clientappsettings.h"

#include "../ui/uimanager.h"

#include "../gui/guisystem.h"

#include "Interfaces/generic/keyvalueiterate.h"

#include "Interfaces/util/exception.h"

#include "Interfaces/plugins/pluginmanager.h"

#ifdef STATIC_BUILD
extern "C" Engine::GUI::GUISystem* guisystem(Engine::App::ClientApplication &);
#endif

namespace Engine
{
	namespace App
	{
		ClientApplication::ClientApplication() :
		Application(),
		mGUI(nullptr)
		{
		}

		void ClientApplication::setup(const ClientAppSettings& settings)
		{
			mSettings = &settings;
			
			Application::setup(settings);
		}

		const ClientAppSettings& ClientApplication::settings()
		{
			return *mSettings;
		}

		GUI::GUISystem& ClientApplication::gui(bool init)
		{
			if (init)
			{
				checkInitState();
				mGUI->callInit();
			}
			return mGUI->getSelf(init);
		}

		ClientApplication& ClientApplication::getSelf(bool init)
		{
			Application::getSelf(init);
			return *this;
		}

		void ClientApplication::loadFrameLoop(std::unique_ptr<Core::FrameLoop>&& loop)
		{

			if (!loop) {
#ifndef STATIC_BUILD
				auto f = reinterpret_cast<GUI::GUISystem*(*)(ClientApplication &)>(Plugins::PluginManager::getSingleton().at("Renderer").getUniqueSymbol("guisystem"));
				if (!f)
					throw exception("A Client-Application requires a Renderer with a guisystem!");
				mGUI = f(*this);
#else
				mGUI = guisystem(*this);
#endif
				if (!mGUI)
					throw 0;
				loop = std::unique_ptr<GUI::GUISystem>(mGUI);
			}

			Application::loadFrameLoop(std::forward<std::unique_ptr<Core::FrameLoop>>(loop));
		}

		KeyValueMapList ClientApplication::maps()
		{
			return Application::maps().merge(mGUI, &mGUI->ui());
		}
	}
}
