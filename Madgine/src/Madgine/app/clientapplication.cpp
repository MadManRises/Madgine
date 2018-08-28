#include "../clientlib.h"

#include "clientapplication.h"

#include "clientappsettings.h"

#include "../ui/uimanager.h"

#include "../gui/guisystem.h"

#include "../generic/keyvalueiterate.h"

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
			
		
			auto f = reinterpret_cast<GUI::GUISystem*(*)(ClientApplication &)>(Plugins::PluginManager::getSingleton().at("Renderer").getUniqueSymbol("guisystem"));
			if (!f)
				throw 0;
			mGUI = f(*this);
			if (!mGUI)
				throw 0;

			Application::setup(settings, std::unique_ptr<GUI::GUISystem>(mGUI));
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

		KeyValueMapList ClientApplication::maps()
		{
			return Application::maps().merge(mGUI, &mGUI->ui());
		}
	}
}
