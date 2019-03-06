#include "../baselib.h"
#include "application.h"

#include "appsettings.h"




#include "globalapibase.h"

#include "Interfaces/util/standardlog.h"

#include "Interfaces/threading/frameloop.h"

#include "../scene/scenemanager.h"

#include "Interfaces/scripting/types/api.h"

#include "Interfaces/generic/keyvalueiterate.h"

#include "Interfaces/scripting/types/luastate.h"

#include "Interfaces/debug/profiler/profiler.h"

namespace Engine
{	

	namespace App
	{
		Application::Application(const AppSettings& settings, Threading::WorkGroup &workGroup) :
			Scope(Scripting::LuaState::getSingleton()),
			mProfiler(std::make_unique<Debug::Profiler::Profiler>(workGroup)),
			mSettings(settings),
			mGlobalAPIs(*this),
			mGlobalAPIInitCounter(0)			
		{
			mLog = std::make_unique<Util::StandardLog>(settings.mAppName);
			Util::UtilMethods::setup(mLog.get());			

			loadFrameLoop();
		}

		Application::~Application()
		{
			mLoop->removeFrameListener(this);
			Util::UtilMethods::setup();
			mLog.reset();
		}

		bool Application::init()
		{
			
			markInitialized();

			for (const std::unique_ptr<GlobalAPIBase>& api : mGlobalAPIs)
			{
				if (!api->callInit(mGlobalAPIInitCounter))
					return false;
			}

			return true;
		}

		void Application::finalize()
		{
			for (; mGlobalAPIInitCounter > 0; --mGlobalAPIInitCounter) {
				for (const std::unique_ptr<GlobalAPIBase>& api : mGlobalAPIs)
				{
					api->callFinalize(mGlobalAPIInitCounter);
				}
			}			
		}

		void Application::loadFrameLoop(std::unique_ptr<Threading::FrameLoop>&& loop)
		{
			if (!loop) {
				loop = std::make_unique<Threading::FrameLoop>();
			}

			if (mLoop) {
				mLoop->removeFrameListener(this);
			}
			mLoop = std::move(loop);
			mLoop->addFrameListener(this);
		}

		void Application::shutdown()
		{
			mLoop->shutdown();
		}

		/*int Application::go()
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
				SignalSlot::DefaultTaskQueue::getSingleton().execute();
			} while (mRestartLoop);
			clear();
			return result;
		}*/

		bool Application::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
		{
			PROFILE();
			try
			{
				for (const std::unique_ptr<GlobalAPIBase>& p : mGlobalAPIs)
				{
					p->update();
				}
			}
			catch (const std::exception& e)
			{
				LOG_ERROR("Unhandled Exception during GlobalScope-update!");
				LOG_EXCEPTION(e);
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

		GlobalAPIBase& Application::getGlobalAPIComponent(size_t i, bool init)
		{
			GlobalAPIBase &api = mGlobalAPIs.get(i);
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
			for (const std::unique_ptr<GlobalAPIBase>& p : mGlobalAPIs)
			{
				//p->clear();
			}
			mGlobalAPIs.get<Scene::SceneManager>().clear();
		}

		void Application::addFrameListener(Threading::FrameListener* listener)
		{
			mLoop->addFrameListener(listener);
		}

		void Application::removeFrameListener(Threading::FrameListener* listener)
		{
			mLoop->removeFrameListener(listener);
		}

		void Application::singleFrame()
		{
			mLoop->singleFrame();
		}

		Threading::FrameLoop & Application::frameLoop()
		{
			return *mLoop;
		}

		Util::Log &Application::log() {
			return *mLog;
		}

		const AppSettings &Application::settings()
		{
			return mSettings;
		}

	}

}
