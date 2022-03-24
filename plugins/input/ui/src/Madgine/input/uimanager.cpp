#include "../uilib.h"

#include "uimanager.h"

#include "Madgine/window/mainwindow.h"

#include "Modules/debug/profiler/profile.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Meta/serialize/configs/controlled.h"

#include "gamehandler.h"
#include "guihandler.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"


UNIQUECOMPONENT(Engine::Input::UIManager)

METATABLE_BEGIN(Engine::Input::UIManager)
MEMBER(mGuiHandlers)
MEMBER(mGameHandlers)
METATABLE_END(Engine::Input::UIManager)

SERIALIZETABLE_BEGIN(Engine::Input::UIManager)
FIELD(mGuiHandlers, Serialize::ControlledConfig<KeyCompare<std::unique_ptr<Engine::Input::GuiHandlerBase>>>)
FIELD(mGameHandlers, Serialize::ControlledConfig<KeyCompare<std::unique_ptr<Engine::Input::GameHandlerBase>>>)
SERIALIZETABLE_END(Engine::Input::UIManager)

namespace Engine {

namespace Input {

    UIManager::UIManager(Window::MainWindow &window)
        : VirtualData(window, 50)
        , mGuiHandlers(*this)
        , mGameHandlers(*this)
    {
        
    }

    UIManager::~UIManager()
    {
    }

    Threading::Task<bool> UIManager::init()
    {

        for (const std::unique_ptr<GuiHandlerBase> &handler : mGuiHandlers)
            co_await handler->callInit();

        for (const std::unique_ptr<GameHandlerBase> &handler : mGameHandlers)
            co_await handler->callInit();

        mWindow.taskQueue()->addRepeatedTask([this]() {
            update();
        });
        mWindow.taskQueue()->addRepeatedTask([this]() {
            fixedUpdate();
        },
            std::chrono::microseconds { 15000 });

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

    void UIManager::update()
    {
        std::chrono::microseconds timeSinceLastFrame = mFrameClock.tick<std::chrono::microseconds>();

        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            h->update(timeSinceLastFrame);
        }
    }

    void UIManager::fixedUpdate()
    {
        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            h->fixedUpdate(std::chrono::microseconds { 15000 });
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

}
}
