#include "../baselib.h"
#include "application.h"

#include "appsettings.h"



#include "framelistener.h"

#include "../scripting/types/globalapicomponent.h"

#include "../scripting/types/luastate.h"

#include "../util/standardlog.h"

API_IMPL(Engine::App::Application, MAP_F(shutdown));

namespace Engine
{	

	namespace App
	{
		Application::Application(Scripting::LuaState *state) :
			Scope(state),
			mShutDown(false),
			mTimeBank(0.0f),
			mGlobalAPIs(*this)
		{
		}

		Application::~Application()
		{
		}

		void Application::setup(const AppSettings& settings)
		{
			mLog = std::make_unique<Util::StandardLog>(settings.mAppName);
			Util::UtilMethods::setup(mLog.get());

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

			return true;
		}

		void Application::finalize()
		{
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
			return 0;
		}

		bool Application::update(float timeSinceLastFrame)
		{
			if (mShutDown)
			{
				return false;
			}

			if (!sendFrameRenderingQueued(timeSinceLastFrame))
				return false;

			mTimeBank += timeSinceLastFrame;

			while (mTimeBank >= FIXED_TIMESTEP)
			{
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

		float Application::fixedRemainder() const
		{
			return FIXED_TIMESTEP - mTimeBank;
		}

		bool Application::isShutdown() const
		{
			return mShutDown;
		}

		float Application::getFPS()
		{
			return 1.0f;
		}

		void Application::addFrameListener(FrameListener* listener)
		{
			mListeners.push_back(listener);
		}

		void Application::removeFrameListener(FrameListener* listener)
		{
			mListeners.remove(listener);
		}

		bool Application::fixedUpdate(float timeStep)
		{
			return true;
		}

		bool Application::sendFrameStarted(float timeSinceLastFrame)
		{
			bool result = true;
			for (FrameListener* listener : mListeners)
				result &= listener->frameStarted(timeSinceLastFrame);
			return result;
		}

		bool Application::sendFrameRenderingQueued(float timeSinceLastFrame)
		{
			bool result = true;
			for (FrameListener* listener : mListeners)
				result &= listener->frameRenderingQueued(timeSinceLastFrame);
			return result;
		}

		bool Application::sendFrameEnded(float timeSinceLastFrame)
		{
			bool result = true;
			for (FrameListener* listener : mListeners)
				result &= listener->frameEnded(timeSinceLastFrame);
			return result;
		}

		KeyValueMapList Application::maps()
		{
			return Scope::maps().merge(mGlobalAPIs);
		}

		Scripting::GlobalAPIComponentBase& Application::getGlobalAPIComponent(size_t i)
		{
			return mGlobalAPIs.get(i);
		}


		void Application::clear()
		{
			for (const std::unique_ptr<Scripting::GlobalAPIComponentBase>& p : mGlobalAPIs)
			{
				p->clear();
			}
		}

		bool Application::singleFrame(float timeSinceLastFrame)
		{
			return sendFrameStarted(timeSinceLastFrame) &&
				update(timeSinceLastFrame) &&
				sendFrameEnded(timeSinceLastFrame);
		}

		Util::Log &Application::log() {
			return *mLog;
		}

	}

#ifdef _MSC_VER
	template class MADGINE_BASE_EXPORT Scripting::GlobalAPICollector;
#endif
}
