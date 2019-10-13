#include "../clientlib.h"

#include "uimanager.h"

#include "../gui/widgets/toplevelwindow.h"

#include "../gui/widgets/widget.h"

#include "Madgine/app/application.h"
#include "Madgine/app/appsettings.h"

#include "Modules/debug/profiler/profiler.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/serialize/serializetable_impl.h"
#include "Modules/reflection/classname.h"

UNIQUECOMPONENT(Engine::UI::UIManager)

METATABLE_BEGIN(Engine::UI::UIManager)
MEMBER(mGuiHandlers)
MEMBER(mGameHandlers)
METATABLE_END(Engine::UI::UIManager)

SERIALIZETABLE_BEGIN(Engine::UI::UIManager)
FIELD(mGuiHandlers)
FIELD(mGameHandlers)
SERIALIZETABLE_END(Engine::UI::UIManager)

RegisterType(Engine::UI::UIManager);

namespace Engine {

namespace UI {

    UIManager::UIManager(GUI::TopLevelWindow &window)
        : SerializableUnit(window)
        , mGuiHandlers(*this)
        , mGameHandlers(*this)
    {
    }

    UIManager::~UIManager()
    {
    }

    UIManager &UIManager::getSelf(bool init)
    {
        if (init) {
            checkDependency();
        }
        return *this;
    }

    bool UIManager::init()
    {
        mWindow.addFrameListener(this);

        markInitialized();

        if (App::Application::getSingleton().settings().mRunMain) {
            /*std::optional<Scripting::ArgumentList> res = app(false).callMethodIfAvailable("afterViewInit", {});
				if (res && !res->empty() && (!res->front().is<bool>() || !res->front().as<bool>()))
					return false;*/
        }

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



    bool UIManager::frameRenderingQueued(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
    {
        PROFILE();
        for (const std::unique_ptr<GameHandlerBase> &h : mGameHandlers) {
            h->update(timeSinceLastFrame, context);
        }
        return true;
    }

    bool UIManager::frameFixedUpdate(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask context)
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

    GUI::TopLevelWindow &UIManager::window(bool init) const
    {
        if (init) {
            checkInitState();
        }
        return mWindow;
    }

    const char *UIManager::key() const
    {
        return "UI";
    }

    GameHandlerBase &UIManager::getGameHandler(size_t i, bool init)
    {
        GameHandlerBase &handler = mGameHandlers.get(i);
        if (init) {
            checkInitState();
            handler.callInit();
        }
        return handler.getSelf(init);
    }

    GuiHandlerBase &UIManager::getGuiHandler(size_t i, bool init)
    {
        GuiHandlerBase &handler = mGuiHandlers.get(i);
        if (init) {
            checkInitState();
            handler.callInit();
        }
        return handler.getSelf(init);
    }

}
}
