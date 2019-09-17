#include "Madgine/baselib.h"

#if ANDROID

#    include "androidlauncher.h"

extern "C" DLL_EXPORT void ANativeActivity_onCreate(ANativeActivity *activity,
    void *savedState, size_t savedStateSize)
{
    new Engine::Android::AndroidLauncher(activity);
}

#endif