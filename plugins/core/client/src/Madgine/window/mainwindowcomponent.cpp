#include "../clientlib.h"

#include "mainwindowcomponent.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/serialize/serializetable_impl.h"

#include "mainwindow.h"

METATABLE_BEGIN(Engine::Window::MainWindowComponentBase)
METATABLE_END(Engine::Window::MainWindowComponentBase)

SERIALIZETABLE_BEGIN(Engine::Window::MainWindowComponentBase)
SERIALIZETABLE_END(Engine::Window::MainWindowComponentBase)

namespace Engine {
namespace Window {

    MainWindowComponentBase::MainWindowComponentBase(MainWindow &window, int priority)
        : mPriority(priority)
        , mWindow(window)
    {
    }

    MainWindow &MainWindowComponentBase::window() const
    {
        return mWindow;
    }

    const MainWindow *MainWindowComponentBase::parent() const
    {
        return &mWindow;
    }

    void MainWindowComponentBase::onResize(const Rect2i &space)
    {
        mClientSpace = space;
    }

    Rect2i MainWindowComponentBase::getScreenSpace() const
    {
        return mWindow.getScreenSpace();
    }

    const Rect2i &MainWindowComponentBase::getClientSpace() const
    {
        return mClientSpace;
    }

    MainWindowComponentBase &MainWindowComponentBase::getWindowComponent(size_t i, bool init)
    {
        if (init) {
            checkInitState();
        }
        return mWindow.getWindowComponent(i, init);
    }

    Rect2i MainWindowComponentBase::getChildClientSpace()
    {
        return mClientSpace;
    }

}
}