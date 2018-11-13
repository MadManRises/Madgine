#include "../baselib.h"
#include "application.h"

#include "appsettings.h"




#include "globalapicomponentbase.h"

#include "Interfaces/util/standardlog.h"

#include "../core/frameloop.h"

#include "../scene/scenemanager.h"

#include "Interfaces/scripting/types/api.h"

#include "Interfaces/generic/keyvalueiterate.h"

#include "Interfaces/scripting/types/luastate.h"

#if defined(STATIC_BUILD) && defined(STATIC_NOGUI)
extern "C" Engine::Core::FrameLoop* frameloop();
#endif

namespace Engine
{	

	template MADGINE_BASE_EXPORT struct UniqueComponentCollector<App::GlobalAPIComponentBase, App::Application&>;

	namespace App
	{
		Application::Application() :
			Scope(Scripting::LuaState::getSingleton()),
			mSettings(nullptr),
			mGlobalAPIs(*this),
		mGlobalAPIInitCounter(0)
		{
		}

		Application::~Application()
		{
			mLoop->removeFrameListener(this);
			mLog.reset();
		}

		void Application::setup(const AppSettings& settings)
		{
			mSettings = &settings;

			mLog = std::make_unique<Util::StandardLog>(settings.mAppName);
			Util::UtilMethods::setup(mLog.get());

#ifndef STATIC_BUILD
			Plugins::PluginManager::getSingleton()["Renderer"].addListener(this);
#endif

			loadFrameLoop();
		}

		bool Application::init()
		{
			
			markInitialized();


			if (!mLoop->callInit())
				return false;

			for (const std::unique_ptr<GlobalAPIComponentBase>& api : mGlobalAPIs)
			{
				if (!api->callInit(mGlobalAPIInitCounter))
					return false;
			}

			return true;
		}

		void Application::finalize()
		{
			for (; mGlobalAPIInitCounter > 0; --mGlobalAPIInitCounter) {
				for (const std::unique_ptr<GlobalAPIComponentBase>& api : mGlobalAPIs)
				{
					api->callFinalize(mGlobalAPIInitCounter);
				}
			}

			mLoop->callFinalize();
		}

		void Application::loadFrameLoop(std::unique_ptr<Core::FrameLoop>&& loop)
		{
			if (!loop) {
#ifndef STATIC_BUILD
				auto f = Plugins::PluginManager::getSingleton().at("Renderer").getUniqueSymbol<Core::FrameLoop*()>("frameloop");
				if (!f)
					throw 0;
				loop = std::unique_ptr<Core::FrameLoop>(f());
#elif defined(STATIC_NOGUI)
				loop = std::unique_ptr<Core::FrameLoop>(frameloop());
#endif
				if (!loop)
					throw 0;
			}

			if (mLoop) {
				mLoop->removeFrameListener(this);
			}
			mLoop = std::move(loop);
			mLoop->addFrameListener(this);
		}

		bool Application::aboutToUnloadPlugin(const Plugins::Plugin * plugin)
		{
			shutdown();
			return false;
		}

		bool Application::aboutToLoadPlugin(const Plugins::Plugin * plugin)
		{
			return false;
		}

		void Application::onPluginUnload(const Plugins::Plugin * plugin)
		{
			mLoop->callFinalize();
			mLoop.reset();
		}

		void Application::onPluginLoad(const Plugins::Plugin * plugin)
		{
			loadFrameLoop();
			mLoop->callInit();
			mRestartLoop = true;
		}

		void Application::shutdown()
		{
			mLoop->shutdown();
		}

		int Application::go()
		{

			if (mSettings->mRunMain) {
				if (!callMethodCatch("main", {}))
				{
					return -1;
				}
			}


			int result;
			do {
				mRestartLoop = false;
				result = mLoop->go();
				SignalSlot::ConnectionManager::getSingleton().update();
			} while (mRestartLoop);
			clear();
			return result;
		}

		bool Application::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
		{

			{
				//PROFILE("ScriptingManager")
				try
				{
					for (const std::unique_ptr<GlobalAPIComponentBase>& p : mGlobalAPIs)
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
			return mLoop->isShutdown();
		}

		float Application::getFPS()
		{
			return 1.0f;
		}


		KeyValueMapList Application::maps()
		{
			return Scope::maps().merge(mGlobalAPIs, this, MAP_F(shutdown));
		}

		GlobalAPIComponentBase& Application::getGlobalAPIComponent(size_t i, bool init)
		{
            GlobalAPIComponentBase &api = mGlobalAPIs.get(i); 
            if (init){
                checkInitState();
                api.callInit(mGlobalAPIInitCounter);
            }
			return api.getSelf(init);
		}

		Scene::SceneComponentBase& Application::getSceneComponent(size_t i, bool init)
		{
			Scene::SceneManager &sceneMgr = mGlobalAPIs.get<Scene::SceneManager>();
            if (init){
                checkInitState();
                sceneMgr.callInit(mGlobalAPIInitCounter);
            }
			return sceneMgr.getComponent(i, init);
		}

		Scene::SceneManager& Application::sceneMgr(bool init)
		{
			Scene::SceneManager &sceneMgr = mGlobalAPIs.get<Scene::SceneManager>();
            if (init){
                checkInitState();
				sceneMgr.callInit(mGlobalAPIInitCounter);
            }
			return sceneMgr.getSelf(init);
		}
        
        Application &Application::getSelf(bool init){
            if (init){
                checkDependency();
            }
            return *this;
        }


		void Application::clear()
		{
			for (const std::unique_ptr<GlobalAPIComponentBase>& p : mGlobalAPIs)
			{
				//p->clear();
			}
			mGlobalAPIs.get<Scene::SceneManager>().clear();
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

	}

#ifdef _MSC_VER
	template struct App::GlobalAPICollector;
#endif

}
