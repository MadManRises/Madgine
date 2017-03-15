#include "baselib.h"
#include "application.h"

#include "appsettings.h"

#include "Scene\scenemanager.h"

#include "Scripting\Types\scriptingmanager.h"

namespace Engine {

	API_IMPL(App::Application, &shutdown);

	namespace App {

		Application::Application() :
			mShutDown(false),
			mScriptingMgrInitialized(false),
			mScriptingMgr(nullptr),
			mTimeBank(0.0f)
		{

		}

		Application::~Application()
		{
			if (mScriptingMgr) {
				if (mScriptingMgrInitialized)
					mScriptingMgr->finalize();
				delete mScriptingMgr;
			}
		}

		void Application::setup(const AppSettings &settings)
		{
			_setup();
		}

		bool Application::init()
		{			
			mScriptingMgrInitialized = mScriptingMgr->init();
			return mScriptingMgrInitialized;
		}

		void Application::shutdown()
		{
			mShutDown = true;
		}

		void Application::callSafe(std::function<void()> f)
		{
			mSafeCallQueue.emplace(f);
		}

		int Application::go() {
			mShutDown = false;
			return 0;
		}

		bool Application::update(float timeSinceLastFrame)
		{			
			if (mShutDown) {
				return false;
			}

			mTimeBank += timeSinceLastFrame;

			while (mTimeBank >= FIXED_TIMESTEP) {

				if (!fixedUpdate(FIXED_TIMESTEP))
					return false;

				mTimeBank -= FIXED_TIMESTEP;

			}

			{
				//PROFILE("SafeCall");
				while (!mSafeCallQueue.empty()) {
					try {
						mSafeCallQueue.front()();
					}
					catch (const std::exception &e) {
						LOG_ERROR("Unhandled Exception during SafeCall!");
						LOG_EXCEPTION(e);
					}
					mSafeCallQueue.pop();
				}
			}

			{
				//PROFILE("ScriptingManager")
				try {
					mScriptingMgr->globalScope()->update(timeSinceLastFrame);
				}
				catch (const std::exception &e) {
					LOG_ERROR("Unhandled Exception during GlobalScope-update!");
					LOG_EXCEPTION(e);
				}
			}

			return true;
		}

		float Application::fixedRemainder()
		{
			return FIXED_TIMESTEP - mTimeBank;
		}

		bool Application::isShutdown()
		{
			return mShutDown;
		}

		bool Application::fixedUpdate(float timeStep) {
			return true;
		}

		void Application::_clear()
		{
			mScriptingMgr->clear();
		}

		void Application::_setup()
		{

			// Instantiate the GlobalScope class
			mScriptingMgr = new Scripting::ScriptingManager;

			mScriptingMgr->globalScope()->addAPI(this);
		}


	}
}

