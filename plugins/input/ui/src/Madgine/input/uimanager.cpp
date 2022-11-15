#include "../uilib.h"

#include "uimanager.h"

#include "Madgine/window/mainwindow.h"

#include "Modules/debug/profiler/profile.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/serialize/configs/controlled.h"

#include "handler.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/app/application.h"

#include "Madgine/widgets/widgetmanager.h"

METATABLE_BEGIN(Engine::Input::UIManager)
MEMBER(mHandlers)
METATABLE_END(Engine::Input::UIManager)

namespace Engine {

namespace Input {

    UIManager::UIManager(App::Application &app, Window::MainWindow &window)
        : mApp(app)
        , mWindow(window)
        , mHandlers(*this)
    {
        window.taskQueue()->addSetupSteps(
            [this]() { return callInit(); },
            [this]() { return callFinalize(); });
    }

    UIManager::~UIManager()
    {
    }

    Threading::Task<bool> UIManager::init()
    {

        mWindow.getWindowComponent<Widgets::WidgetManager>().updatedSignal().connect([this] { onUpdate(); });

        for (const std::unique_ptr<HandlerBase> &handler : mHandlers)
            co_await handler->callInit();

        mWindow.taskQueue()->addRepeatedTask([this]() {
            updateRender();
        });
        mWindow.taskQueue()->addRepeatedTask([this]() {
            fixedUpdateRender();
        },
            std::chrono::microseconds { 15000 });

        mApp.taskQueue()->addRepeatedTask([this]() {
            updateApp();
        });

        co_return true;
    }

    Threading::Task<void> UIManager::finalize()
    {
        for (const std::unique_ptr<HandlerBase> &handler : mHandlers)
            co_await handler->callFinalize();
    }

    App::Application &UIManager::app() const
    {
        return mApp;
    }

    Window::MainWindow &UIManager::window() const
    {
        return mWindow;
    }

    void UIManager::shutdown()
    {
        mWindow.shutdown();
    }

    void UIManager::onUpdate()
    {
        for (const std::unique_ptr<HandlerBase> &handler : mHandlers)
            handler->onUpdate();
    }

    void UIManager::clear()
    {
        /*while (!mModalWindowList.empty()) {
            closeModalWindow(mModalWindowList.top());
        }*/
    }

    void UIManager::hideCursor(bool keep)
    {
        if (!isCursorVisible())
            return;
        mKeepingCursorPos = keep;
        if (keep) {
            /*const OIS::MouseState &mouseState = mMouse->getMouseState();
				mKeptCursorPosition = { (float)mouseState.X.abs, (float)mouseState.Y.abs };*/
        }
        //mGUI.hideCursor();

        for (const std::unique_ptr<HandlerBase> &h : mHandlers)
            h->onMouseVisibilityChanged(false);
    }

    void UIManager::showCursor()
    {
        if (isCursorVisible())
            return;
        if (mKeepingCursorPos) {
            /*OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(mMouse->getMouseState());
				mutableMouseState.X.abs = mKeptCursorPosition.x;
				mutableMouseState.Y.abs = mKeptCursorPosition.y;
				callSafe([&]() {
					mouseMoved(OIS::MouseEvent(mMouse, mutableMouseState));*/
            //mGUI.showCursor();
            /*});*/
        } else {
            //mGUI.showCursor();
        }
        for (const std::unique_ptr<HandlerBase> &h : mHandlers)
            h->onMouseVisibilityChanged(true);
    }

    bool UIManager::isCursorVisible() const
    {
        return /* mGUI.isCursorVisible()*/ true;
    }

    std::set<HandlerBase *> UIManager::getHandlers()
    {
        std::set<HandlerBase *> result;
        for (const std::unique_ptr<HandlerBase> &h : mHandlers) {
            result.insert(h.get());
        }
        return result;
    }

    void UIManager::updateRender()
    {
        std::chrono::microseconds timeSinceLastFrame = mFrameClock.tick<std::chrono::microseconds>();

        for (const std::unique_ptr<HandlerBase> &h : mHandlers)
            h->updateRender(timeSinceLastFrame);
    }

    void UIManager::fixedUpdateRender()
    {
        for (const std::unique_ptr<HandlerBase> &h : mHandlers)
            h->fixedUpdateRender(std::chrono::microseconds { 15000 });
    }

    void UIManager::updateApp()
    {
        std::chrono::microseconds timeSinceLastFrame = mAppClock.tick<std::chrono::microseconds>();

        for (const std::unique_ptr<HandlerBase> &h : mHandlers)
            h->updateApp(timeSinceLastFrame);
    }

    std::string_view UIManager::key() const
    {
        return "UI";
    }

    HandlerBase &UIManager::getHandler(size_t i)
    {
        return mHandlers.get(i);
    }

    Threading::TaskQueue *UIManager::viewTaskQueue() const
    {
        return mWindow.taskQueue();
    }

    Threading::TaskQueue *UIManager::modelTaskQueue() const
    {
        return mApp.taskQueue();
    }

}
}
