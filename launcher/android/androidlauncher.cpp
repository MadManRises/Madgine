#include "client/clientlib.h"
#include "androidinputlib.h"

#include "androidlauncher.h"

#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"
#include "Madgine/core/root.h"
#include "Interfaces/threading/workgroup.h"
#include "Interfaces/threading/scheduler.h"

#include <android/native_activity.h>

#include "Interfaces/threading/systemvariable.h"

namespace Engine {
	namespace Filesystem {

		extern AAssetManager *sAssetManager;

	}

	namespace Window
	{

		extern Threading::SystemVariable<ANativeWindow*> sNativeWindow;

	}

	namespace Input
	{

		extern AInputQueue *sQueue;

	}

	namespace Android {

		template <auto f, typename... Args>
		static void delegate(ANativeActivity *activity, Args... args)
		{
			(static_cast<AndroidLauncher*>(activity->instance)->*f)(args...);
		}


		AndroidLauncher::AndroidLauncher(ANativeActivity * activity) :
			mActivity(activity),
			mApp(nullptr)
		{
			activity->instance = this;

			activity->callbacks->onDestroy = delegate<&AndroidLauncher::onDestroy>;
			activity->callbacks->onNativeWindowCreated = delegate<&AndroidLauncher::onNativeWindowCreated, ANativeWindow*>;
			activity->callbacks->onNativeWindowDestroyed = delegate<&AndroidLauncher::onNativeWindowDestroyed, ANativeWindow*>;
			activity->callbacks->onInputQueueCreated = delegate<&AndroidLauncher::onInputQueueCreated, AInputQueue*>;
			activity->callbacks->onInputQueueDestroyed = delegate<&AndroidLauncher::onInputQueueDestroyed, AInputQueue*>;

			Engine::Filesystem::sAssetManager = activity->assetManager;
			
			mThread = Threading::WorkGroupHandle(&AndroidLauncher::go, this);

		}

		void AndroidLauncher::go(Threading::WorkGroup &workGroup)
		{
			ANativeActivity * activity = mActivity;

			static Engine::Core::Root root;
			Engine::App::AppSettings settings;
			settings.mRunMain = false;
			settings.mAppName = "Madgine Client";
			settings.mWindowSettings.mTitle = "Maditor";
			
			Application app(settings);
			mApp = &app;
			Threading::Scheduler(workGroup, { &app.frameLoop() }).go();
			ANativeActivity_finish(activity);
		}

		void AndroidLauncher::onDestroy()
		{
			mApp->shutdown();
			mThread.detach();
			mActivity->instance = nullptr;
			delete this;
		}

		void AndroidLauncher::onNativeWindowCreated(ANativeWindow * window)
		{
			assert(!Window::sNativeWindow);
			Window::sNativeWindow = window;
		}

		void AndroidLauncher::onNativeWindowDestroyed(ANativeWindow * window)
		{
			assert(Window::sNativeWindow == window);
			Window::sNativeWindow = nullptr;
		}

		void AndroidLauncher::onInputQueueCreated(AInputQueue *queue)
		{
			assert(!Input::sQueue);
			Input::sQueue = queue;
		}

		void AndroidLauncher::onInputQueueDestroyed(AInputQueue *queue)
		{
			assert(Input::sQueue == queue);
			Input::sQueue = nullptr;
		}

	}
}