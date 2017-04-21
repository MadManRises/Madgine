#include "baselib.h"
#include "application.h"

#include "appsettings.h"

#include "Scripting\Types\scriptingmanager.h"

#include "Util\standardlog.h"

#include "framelistener.h"

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
			mLog = std::make_unique<Util::StandardLog>(settings.mAppName);
			Util::UtilMethods::setup(mLog.get());
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

		/*void Application::callSafe(std::function<void()> f)
		{
			mSafeCallQueue.emplace(f);
		}*/

		int Application::go() {
			mShutDown = false;
			return 0;
		}

		bool Application::update(float timeSinceLastFrame)
		{			
			if (mShutDown) {
				return false;
			}

			if (!sendFrameRenderingQueued(timeSinceLastFrame))
				return false;

			mTimeBank += timeSinceLastFrame;

			while (mTimeBank >= FIXED_TIMESTEP) {

				if (!fixedUpdate(FIXED_TIMESTEP))
					return false;

				mTimeBank -= FIXED_TIMESTEP;

			}

			{
				//PROFILE("ConnectionDispatch");
				mConnectionManager.update();
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

		float Application::getFPS()
		{
			return 1.0f;
		}

		void Application::addFrameListener(FrameListener * listener)
		{
			mListeners.push_back(listener);
		}

		void Application::removeFrameListener(FrameListener * listener)
		{
			mListeners.remove(listener);
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

		bool Application::sendFrameStarted(float timeSinceLastFrame)
		{
			bool result = true;
			for (FrameListener *listener : mListeners)
				result &= listener->frameStarted(timeSinceLastFrame);
			return result;
		}

		bool Application::sendFrameRenderingQueued(float timeSinceLastFrame)
		{
			bool result = true;
			for (FrameListener *listener : mListeners)
				result &= listener->frameRenderingQueued(timeSinceLastFrame);
			return result;
		}

		bool Application::sendFrameEnded(float timeSinceLastFrame)
		{
			bool result = true;
			for (FrameListener *listener : mListeners)
				result &= listener->frameEnded(timeSinceLastFrame);
			return result;
		}


	}
}

