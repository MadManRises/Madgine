#include "../uilib.h"

#include "uimanager.h"

#include "Madgine/window/mainwindow.h"

#include "Modules/debug/profiler/profile.h"

#include "Meta/keyvalue/metatable_impl.h"

#include "Meta/serialize/configs/controlled.h"

#include "gamehandler.h"
#include "guihandler.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"

#include "Madgine/base/application.h"

#include "Madgine/widgets/widgetmanager.h"

METATABLE_BEGIN(Engine::Input::UIManager)
MEMBER(mGuiHandlers)
MEMBER(mGameHandlers)
METATABLE_END(Engine::Input::UIManager)

namespace Engine {

namespace Input {

    UIManager::UIManager(Base::Application &app, Window::MainWindow &window)
        : mApp(app)
        , mWindow(window)
        , mGuiHandlers(*this)
        , mGameHandlers(*this)
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

        for (const std::unique_ptr<GuiHandlerBase> &handler : mGuiHandlers)
            co_await handler->callInit();

        for (const std::unique_ptr<GameHandlerBase> &handler : mGameHandlers)
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
        for (const std::unique_ptr<GameHandlerBase> &handler : mGameHandlers) {
            co_await handler->callFinalize();
        }

        for (const std::unique_ptr<GuiHandlerBase> &handler : mGuiHandlers)
            co_await handler->callFinalize();
    }

    Base::Application &UIManager::app() const
    {
        return mApp;
    }

    Window::MainWindow &UIManager::window() const
    {
        return mWindow;
    }

    void UIManager::onUpdate()
    {
        for (const std::unique_ptr<GuiHandlerBase> &handler : mGuiHandlers)
            handler->onUpdate();

        for (const std::unique_ptr<GameHandlerBase> &handler : mGameHandlers)
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
        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            h->onMouseVisibilityChanged(false);
        }
        for (const std::unique_ptr<GuiHandlerBase> &h : mGuiHandlers) {
            h->onMouseVisibilityChanged(false);
        }
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
        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            h->onMouseVisibilityChanged(true);
        }
        for (const std::unique_ptr<GuiHandlerBase> &h : mGuiHandlers) {
            h->onMouseVisibilityChanged(true);
        }
    }

    bool UIManager::isCursorVisible() const
    {
        return /* mGUI.isCursorVisible()*/ true;
    }

    std::set<GameHandlerBase *> UIManager::getGameHandlers()
    {
        std::set<GameHandlerBase *> result;
        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            result.insert(h.get());
        }
        return result;
    }

    std::set<GuiHandlerBase *> UIManager::getGuiHandlers()
    {
        std::set<GuiHandlerBase *> result;
        for (const std::unique_ptr<GuiHandlerBase> &h : mGuiHandlers) {
            result.insert(h.get());
        }
        return result;
    }

    void UIManager::updateRender()
    {
        std::chrono::microseconds timeSinceLastFrame = mFrameClock.tick<std::chrono::microseconds>();

        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            h->updateRender(timeSinceLastFrame);
        }
    }

    void UIManager::fixedUpdateRender()
    {
        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            h->fixedUpdateRender(std::chrono::microseconds { 15000 });
        }
    }

    void UIManager::updateApp()
    {
        std::chrono::microseconds timeSinceLastFrame = mAppClock.tick<std::chrono::microseconds>();

        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            h->updateApp(timeSinceLastFrame);
        }
    }

    std::string_view UIManager::key() const
    {
        return "UI";
    }

    GameHandlerBase &UIManager::getGameHandler(size_t i)
    {
        return mGameHandlers.get(i);
    }

    GuiHandlerBase &UIManager::getGuiHandler(size_t i)
    {
        return mGuiHandlers.get(i);
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
