#include "baselib.h"
#include "application.h"

#include "appsettings.h"

#include "util/standardlog.h"

#include "framelistener.h"

#include "scripting/types/globalscope.h"

namespace Engine {	

	API_IMPL(App::Application, MAP_F(shutdown));

	namespace App {

		Application::Application() :
			mShutDown(false),
			mGlobalScope(nullptr),
			mTimeBank(0.0f)
		{

		}

		Application::~Application()
		{
		}

		void Application::setup(const AppSettings &settings)
		{
			mLog = std::make_unique<Util::StandardLog>(settings.mAppName);
			Util::UtilMethods::setup(mLog.get());
			mGlobalScope = std::make_unique<Scripting::GlobalScope>(settings.mTable, this);
		}

		bool Application::init()
		{			
			return mGlobalScope->init() && MadgineObject::init();
		}

		void Application::finalize() {
			MadgineObject::finalize();
			mGlobalScope->finalize();
		}

		void Application::shutdown()
		{
			mShutDown = true;
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
					mGlobalScope->update();
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

		Scripting::GlobalScope *Application::globalScope() {
			return mGlobalScope.get();
		}

		void Application::_clear()
		{
			mGlobalScope->clear();
		}

		lua_State * Application::lua_state()
		{
			return mGlobalScope->lua_state();
		}

		const char *Application::key() const
		{
			return "Application";
		}

	}
}

