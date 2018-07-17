#include "../baselib.h"
#include "application.h"

#include "appsettings.h"




#include "../scripting/types/globalapicomponent.h"

#include "../scripting/types/luastate.h"

#include "../util/standardlog.h"

#include "../core/framelistener.h"
#include "../core/root.h"
#include "../core/frameloop.h"
#include "../serialize/container/noparent.h"

#include "../scene/scenemanager.h"

API_IMPL(Engine::App::Application, MAP_F(shutdown));

namespace Engine
{	

	namespace App
	{
		Application::Application(Core::Root &root) :
			Scope(root.luaState()),
			mShutDown(false),
			mSettings(nullptr),
			mGlobalAPIs(root.pluginMgr(), *this),
			mRoot(root)
		{
		}

		Application::~Application()
		{
			mLoop->removeFrameListener(this);
			mLog.reset();
		}

		void Application::setup(const AppSettings& settings, std::unique_ptr<Core::FrameLoop> &&loop)
		{
			mSettings = &settings;

			if (!loop) {
				auto f = static_cast<Core::FrameLoop*(*)()>(pluginMgr().getUniqueSymbol("frameloop"));
				if (!f)
					throw 0;
				loop = std::unique_ptr<Core::FrameLoop>(f());
				if (!loop)
					throw 0;
			}

			mLog = std::make_unique<Util::StandardLog>(settings.mAppName);
			Util::UtilMethods::setup(mLog.get());
			mSceneMgr = Serialize::make_noparent_unique<Scene::SceneManager>(*this);

			mLoop = std::forward<std::unique_ptr<Core::FrameLoop>>(loop);
			mLoop->addFrameListener(this);
			mLoop->addFrameListener(mSceneMgr.get());
		}

		bool Application::init()
		{
			if (!MadgineObject::init())
				return false;

			for (const std::unique_ptr<Scripting::GlobalAPIComponentBase>& api : mGlobalAPIs)
			{
				if (!api->init())
					return false;
			}

			if (!mLoop->preInit())
				return false;

			if (mSettings->mRunMain) {
				std::optional<Scripting::ArgumentList> res = callMethodIfAvailable("afterViewInit");
				if (res && !res->empty() && (!res->front().is<bool>() || !res->front().as<bool>()))
					return false;
			}

			if (!mSceneMgr->init())
				return false;

			if (!mLoop->init())
				return false;

			return true;
		}

		void Application::finalize()
		{

			mLoop->finalize();

			mSceneMgr->finalize();

			for (const std::unique_ptr<Scripting::GlobalAPIComponentBase>& api : mGlobalAPIs)
			{
				api->finalize();
			}
			MadgineObject::finalize();
		}

		void Application::shutdown()
		{
			mShutDown = true;
		}

		int Application::go()
		{
			mShutDown = false;

			if (mSettings->mRunMain) {
				if (!callMethodCatch("main"))
				{
					return -1;
				}
			}

			int result = mLoop->go();
			clear();
			return result;
		}

		bool Application::frameRenderingQueued(float timeSinceLastFrame, Scene::ContextMask context)
		{
			if (mShutDown)
			{
				return false;
			}

			
			{
				//PROFILE("ConnectionDispatch");
				SignalSlot::ConnectionManager::getSingleton().update();
			}

			{
				//PROFILE("ScriptingManager")
				try
				{
					for (const std::unique_ptr<Scripting::GlobalAPIComponentBase>& p : mGlobalAPIs)
					{
						p->update();
					}
				}
				catch (const std::exception& e)
				{
					LOG_ERROR("Unhandled Exception during GlobalScope-update!");
					LOG_EXCEPTION(e);
				}
			}

			return true;
		}

		

		bool Application::isShutdown() const
		{
			return mShutDown;
		}

		float Application::getFPS()
		{
			return 1.0f;
		}


		KeyValueMapList Application::maps()
		{
			return Scope::maps().merge(mGlobalAPIs, mSceneMgr);
		}

		Scripting::GlobalAPIComponentBase& Application::getGlobalAPIComponent(size_t i)
		{
			return mGlobalAPIs.get(i);
		}

		Scene::SceneComponentBase& Application::getSceneComponent(size_t i)
		{
			return mSceneMgr->getComponent(i);
		}

		Scene::SceneManager& Application::sceneMgr()
		{
			return *mSceneMgr;
		}

		/*GUI::GUISystem & Application::gui()
		{
			throw 0;
		}

		UI::UIManager& Application::ui()
		{
			throw 0;
		}

		UI::GameHandlerBase& Application::getGameHandler(size_t i)
		{
			throw 0;
		}

		UI::GuiHandlerBase& Application::getGuiHandler(size_t i)
		{
			throw 0;
		}*/


		void Application::clear()
		{
			for (const std::unique_ptr<Scripting::GlobalAPIComponentBase>& p : mGlobalAPIs)
			{
				p->clear();
			}
			mSceneMgr->clear();
		}

		void Application::addFrameListener(Core::FrameListener* listener)
		{
			mLoop->addFrameListener(listener);
		}

		void Application::removeFrameListener(Core::FrameListener* listener)
		{
			mLoop->removeFrameListener(listener);
		}

		void Application::singleFrame()
		{
			mLoop->singleFrame();
		}

		Util::Log &Application::log() {
			return *mLog;
		}

		Core::Root& Application::root()
		{
			return mRoot;
		}

		const Plugins::PluginManager& Application::pluginMgr()
		{
			return mRoot.pluginMgr();
		}

		Resources::ResourceManager& Application::resources()
		{
			return mRoot.resources();
		}
	}

#ifdef _MSC_VER
	template class MADGINE_BASE_EXPORT Scripting::GlobalAPICollector;
#endif
}
