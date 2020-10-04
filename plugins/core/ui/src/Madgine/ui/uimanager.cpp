#include "../uilib.h"

#include "uimanager.h"

#include "Madgine/window/mainwindow.h"

#include "Madgine/widgets/widget.h"

#include "Modules/debug/profiler/profile.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"

#include "Modules/serialize/container/controlledconfig.h"

#include "guihandler.h"
#include "gamehandler.h"

UNIQUECOMPONENT(Engine::UI::UIManager)

METATABLE_BEGIN(Engine::UI::UIManager)
MEMBER(mGuiHandlers)
MEMBER(mGameHandlers)
METATABLE_END(Engine::UI::UIManager)

SERIALIZETABLE_BEGIN(Engine::UI::UIManager)
FIELD(mGuiHandlers, Serialize::ControlledConfig<KeyCompare<std::unique_ptr<Engine::UI::GuiHandlerBase>>>)
FIELD(mGameHandlers, Serialize::ControlledConfig<KeyCompare<std::unique_ptr<Engine::UI::GameHandlerBase>>>)
SERIALIZETABLE_END(Engine::UI::UIManager)



namespace Engine {

namespace UI {

    UIManager::UIManager(Window::MainWindow &window)
        : VirtualUnit(window, 50)
        , mGuiHandlers(*this)
        , mGameHandlers(*this)
    {
    }

    UIManager::~UIManager()
    {
    }

    bool UIManager::init()
    {
        mWindow.addFrameListener(this);

        markInitialized();

        for (const std::unique_ptr<GuiHandlerBase> &handler : mGuiHandlers)
            if (!handler->callInit())
                return false;

        for (const std::unique_ptr<GameHandlerBase> &handler : mGameHandlers) {
            if (!handler->callInit())
                return false;
        }

        return true;
    }

    void UIManager::finalize()
    {
        for (const std::unique_ptr<GameHandlerBase> &handler : mGameHandlers) {
            handler->callFinalize();
        }

        for (const std::unique_ptr<GuiHandlerBase> &handler : mGuiHandlers)
            handler->callFinalize();

        mWindow.removeFrameListener(this);
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

    bool UIManager::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context)
    {
        PROFILE();
        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            h->update(timeSinceLastFrame, context);
        }
        return true;
    }

    bool UIManager::frameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask context)
    {
        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            h->fixedUpdate(timeSinceLastFrame, context);
        }
        return true;
    }

    /*Scene::ContextMask UIManager::currentContext()
    {
        return mModalWindowList.empty()
            ? (mCurrentRoot ? mCurrentRoot->context() : Scene::ContextMask::NoContext)
            : mModalWindowList.top()->context();
    }*/

    Window::MainWindow &UIManager::window(bool init) const
    {
        if (init) {
            checkInitState();
        }
        return mWindow;
    }

    std::string_view UIManager::key() const
    {
        return "UI";
    }

    GameHandlerBase &UIManager::getGameHandler(size_t i, bool init)
    {
        return getChild(mGameHandlers.get(i), init);
    }

    GuiHandlerBase &UIManager::getGuiHandler(size_t i, bool init)
    {
        return getChild(mGuiHandlers.get(i), init);
    }

}
}
