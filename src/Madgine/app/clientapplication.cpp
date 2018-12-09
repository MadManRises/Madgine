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

		ClientApplication::~ClientApplication()
		{
		}

		void ClientApplication::setup(const ClientAppSettings& settings)
		{
			mSettings = &settings;
			
			Application::setup(settings);

			mGUI = std::make_unique<GUI::GUISystem>(*this);
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

		bool ClientApplication::init()
		{
			if (!Application::init())
				return false;
			return mGUI->callInit();
		}

		void ClientApplication::finalize()
		{
			mGUI->callFinalize();
			Application::finalize();
		}

		KeyValueMapList ClientApplication::maps()
		{
			return Application::maps().merge(mGUI, &mGUI->ui());
		}
	}
}
