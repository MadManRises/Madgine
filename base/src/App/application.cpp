#include "baselib.h"
#include "application.h"

#include "appsettings.h"

#include "Util\standardlog.h"

#include "framelistener.h"

#include "Scripting\Types\GlobalScope.h"

#include "Scripting/Types/api.h"

namespace Engine {	

	API_IMPL(App::Application, MAP(shutdown));

	namespace App {

		

		Application::Application() :
			mShutDown(false),
			mGlobalScope(nullptr),
			mTimeBank(0.0f)
		{

		}

		Application::~Application()
		{
			if (mGlobalScope)
				delete mGlobalScope;
		}

		void Application::setup(const AppSettings &settings)
		{
			mLog = std::make_unique<Util::StandardLog>(settings.mAppName);
			Util::UtilMethods::setup(mLog.get());
			mGlobalScope = new Scripting::GlobalScope(lua_state());
		}

		bool Application::init()
		{			
			return mGlobalScope->init() && GlobalAPIComponentBase::init();
		}

		void Application::finalize() {
			GlobalAPIComponentBase::finalize();
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
			return mGlobalScope;
		}

		void Application::_clear()
		{
			mGlobalScope->clear();
		}

	}
}

