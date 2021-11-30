#include "../clientlib.h"

#include "mainwindowcomponent.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/serialize/serializetable_impl.h"

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

    Threading::TaskQueue *MainWindowComponentBase::taskQueue() const
    {
        return mWindow.taskQueue();
    }

    Threading::Task<bool> MainWindowComponentBase::init()
    {
        co_return true;
    }

    Threading::Task<void> MainWindowComponentBase::finalize()
    {
        co_return;
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

    MainWindowComponentBase &MainWindowComponentBase::getWindowComponent(size_t i)
    {
        return mWindow.getWindowComponent(i);
    }

    Rect2i MainWindowComponentBase::getChildClientSpace()
    {
        return mClientSpace;
    }

}
}