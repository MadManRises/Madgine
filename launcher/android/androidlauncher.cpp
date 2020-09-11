#include "Madgine/clientlib.h"
#include "Madgine/baselib.h"

#include "androidlauncher.h"

#include "Madgine/core/root.h"

#include "Madgine/window/mainwindow.h"

#include <android/native_activity.h>

#include "Interfaces/threading/systemvariable.h"

extern int launch(Engine::Window::MainWindow **topLevelPointer = nullptr);

namespace Engine {
namespace Filesystem {
    extern AAssetManager *sAssetManager;
    extern Path sAppData;
}

namespace Window {
    extern Threading::SystemVariable<ANativeWindow *> sNativeWindow;
    extern AInputQueue *sQueue;
}

namespace Android {

    template <auto f, typename... Args>
    static void delegate(ANativeActivity *activity, Args... args)
    {
        (static_cast<AndroidLauncher *>(activity->instance)->*f)(args...);
    }

    AndroidLauncher::AndroidLauncher(ANativeActivity *activity)
        : mActivity(activity)
        , mWindow(nullptr)
    {
        activity->instance = this;

        activity->callbacks->onDestroy = delegate<&AndroidLauncher::onDestroy>;
        activity->callbacks->onNativeWindowCreated = delegate<&AndroidLauncher::onNativeWindowCreated, ANativeWindow *>;
        activity->callbacks->onNativeWindowDestroyed = delegate<&AndroidLauncher::onNativeWindowDestroyed, ANativeWindow *>;
        activity->callbacks->onInputQueueCreated = delegate<&AndroidLauncher::onInputQueueCreated, AInputQueue *>;
        activity->callbacks->onInputQueueDestroyed = delegate<&AndroidLauncher::onInputQueueDestroyed, AInputQueue *>;

        Engine::Filesystem::sAssetManager = activity->assetManager;
        Engine::Filesystem::sAppData = activity->internalDataPath;

        mThread = Threading::WorkGroupHandle(&AndroidLauncher::go, this);
    }

    void AndroidLauncher::go()
    {
        ANativeActivity *activity = mActivity;

        static Engine::Core::Root root;

		launch(&mWindow);
		
		ANativeActivity_finish(activity);
    }

    void AndroidLauncher::onDestroy()
    {
        mWindow->shutdown();
        mThread.detach();
        mActivity->instance = nullptr;
        delete this;
    }

    void AndroidLauncher::onNativeWindowCreated(ANativeWindow *window)
    {
        assert(!Window::sNativeWindow);
        Window::sNativeWindow = window;
    }

    void AndroidLauncher::onNativeWindowDestroyed(ANativeWindow *window)
    {
        assert(Window::sNativeWindow == window);
        Window::sNativeWindow = nullptr;
    }

    void AndroidLauncher::onInputQueueCreated(AInputQueue *queue)
    {
        assert(!Window::sQueue);
        Window::sQueue = queue;
    }

    void AndroidLauncher::onInputQueueDestroyed(AInputQueue *queue)
    {
        assert(Window::sQueue == queue);
        Window::sQueue = nullptr;
    }

}
}