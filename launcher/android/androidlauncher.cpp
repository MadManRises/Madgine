#include "Madgine/clientlib.h"

#include "androidlauncher.h"

#include "Madgine/app/clientapplication.h"
#include "Madgine/app/clientappsettings.h"
#include "Madgine/core/root.h"

#include <android/native_activity.h>

namespace Engine {
	namespace Android {

		template <auto f, typename... Args>
		static void delegate(ANativeActivity *activity, Args&&... args)
		{
			(static_cast<AndroidLauncher*>(activity->instance)->*f)(std::forward<Args>(args)...);
		}


		AndroidLauncher::AndroidLauncher(ANativeActivity * activity) :
			mActivity(activity)
		{
			activity->instance = this;

			activity->callbacks->onDestroy = delegate<&AndroidLauncher::onDestroy>;

			mThread = std::thread(&AndroidLauncher::go, this);

		}

		void AndroidLauncher::go()
		{
			Engine::Core::Root root;
			root.init();
			Engine::App::ClientAppSettings settings;
			settings.mRunMain = false;
			settings.mAppName = "Madgine Client";
			settings.mWindowSettings.mTitle = "Maditor";
			Engine::App::Application::run<Engine::App::ClientApplication>(settings);
		}

		void AndroidLauncher::onDestroy()
		{
			mActivity->instance = nullptr;
			delete this;
		}

	}
}