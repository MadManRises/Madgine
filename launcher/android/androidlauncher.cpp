#include "client/clientlib.h"

#include "androidlauncher.h"

#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"
#include "Madgine/core/root.h"
#include "Interfaces/threading/workgroup.h"
#include "Interfaces/filesystem/runtime_android.h"

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

	}
}