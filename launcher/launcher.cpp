#include "Madgine/baselib.h"


#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"
#include "Madgine/core/root.h"
#include "Interfaces/threading/workgroup.h"

#if WINDOWS
#	include <conio.h>
#endif

DLL_EXPORT int main() {
	int result;
	{
		Engine::Threading::WorkGroup workGroup;
		Engine::Core::Root root;
		Engine::App::AppSettings settings;
		settings.mRunMain = false;
		settings.mAppName = "Madgine Client";
		settings.mWindowSettings.mTitle = "Maditor";	
		result = Engine::App::Application::run(settings, workGroup);
	}
#if WINDOWS
	while (!_kbhit());
#endif
	return result;
}


#if ANDROID

#include "android/androidlauncher.h"

extern "C" DLL_EXPORT void ANativeActivity_onCreate(ANativeActivity* activity,
	void* savedState, size_t savedStateSize) {
	new Engine::Android::AndroidLauncher(activity);
}

extern "C" DLL_EXPORT void Java_com_example_myapplication_NativeActivity_onCreate()
{
	new Engine::Android::AndroidLauncher(nullptr);
}

#endif
