#include "Madgine/clientlib.h"

#include "androidlauncher.h"

extern "C" DLL_EXPORT_TAG void ANativeActivity_onCreate(ANativeActivity *activity,
    void *savedState, size_t savedStateSize)
{
    new Engine::Android::AndroidLauncher(activity);
}