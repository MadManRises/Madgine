#include "client/clientlib.h"
#include "androidinputlib.h"

#include "androidlauncher.h"

#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"
#include "Madgine/core/root.h"
#include "Interfaces/threading/workgroup.h"
#include "Interfaces/filesystem/runtime_android.h"
#include "Interfaces/window/windowapi_android.h"
#include "eventbridge.h"

#include <android/native_activity.h>

namespace Engine {
	namespace Android {

		template <auto f, typename... Args>
		static void delegate(ANativeActivity *activity, Args... args)
		{
			(static_cast<AndroidLauncher*>(activity->instance)->*f)(args...);
		}


		AndroidLauncher::AndroidLauncher(ANativeActivity * activity) :
			mActivity(activity)
		{
			activity->instance = this;

			activity->callbacks->onDestroy = delegate<&AndroidLauncher::onDestroy>;
			activity->callbacks->onNativeWindowCreated = delegate<&AndroidLauncher::onNativeWindowCreated, ANativeWindow*>;
			activity->callbacks->onInputQueueCreated = delegate<&AndroidLauncher::onInputQueueCreated, AInputQueue*>;
			activity->callbacks->onInputQueueDestroyed = delegate<&AndroidLauncher::onInputQueueDestroyed, AInputQueue*>;

			Engine::Filesystem::setAndroidAssetManager(activity->assetManager);

			mThread = std::thread(&AndroidLauncher::go, this);

		}

		void AndroidLauncher::go()
		{
			Engine::Threading::WorkGroup workGroup;
			Engine::Core::Root root;
			root.init();
			Engine::App::AppSettings settings;
			settings.mRunMain = false;
			settings.mAppName = "Madgine Client";
			settings.mWindowSettings.mTitle = "Maditor";
			Engine::App::Application::run(settings, workGroup);
		}

		void AndroidLauncher::onDestroy()
		{
			mActivity->instance = nullptr;
			delete this;
		}

		void AndroidLauncher::onNativeWindowCreated(ANativeWindow * window)
		{
			Engine::Window::setAndroidNativeWindow(window);
		}

		void AndroidLauncher::onInputQueueCreated(AInputQueue *queue)
		{
			Engine::Input::setAndroidInputQueue(queue);
		}

		void AndroidLauncher::onInputQueueDestroyed(AInputQueue *queue)
		{
			Engine::Input::setAndroidInputQueue(nullptr);
		}

	}
}