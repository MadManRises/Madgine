#include "../clientlib.h"
#include "gamehandler.h"
#include "../gui/guisystem.h"
#include "../gui/widgets/widget.h"
#include "uimanager.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"


DEFINE_UNIQUE_COMPONENT(Engine::UI, GameHandler)

METATABLE_BEGIN_BASE(Engine::UI::GameHandlerBase, Engine::UI::Handler)
METATABLE_END(Engine::UI::GameHandlerBase)

RegisterType(Engine::UI::GameHandlerBase);

namespace Engine {
namespace UI {
    const float GameHandlerBase::mDragStartThreshold = 0.01f;

    GameHandlerBase::GameHandlerBase(UIManager &ui, Scene::ContextMask context)
        : Handler(ui)
        , mCurrentMouseButton(Input::MouseButton::NO_BUTTON)
        , mDragging(false)
        , mSingleClick(false)
        , mContext(context)
    {
    }

    void GameHandlerBase::abortDrag()
    {
        mDragging = false;
        mWidget->releaseInput();
        onPointerDragAbort();
    }

    void GameHandlerBase::update(std::chrono::microseconds timeSinceLastFrame, Scene::ContextMask mask)
    {
        if (mContext & mask)
            update(timeSinceLastFrame);
    }

    void GameHandlerBase::fixedUpdate(std::chrono::microseconds timeStep, Scene::ContextMask mask)
    {
        if (mContext & mask)
            fixedUpdate(timeStep);
    }

    Scene::SceneManager &GameHandlerBase::sceneMgr(bool init) const
    {
        if (init) {
            checkInitState();
        }
        return mUI.sceneMgr(init);
    }

    GameHandlerBase &GameHandlerBase::getSelf(bool init)
    {
        if (init) {
            checkDependency();
        }
        return *this;
    }

    void GameHandlerBase::onPointerMove(const Input::PointerEventArgs &me)
    {
        Input::PointerEventArgs args = me;
        clampToWindow(args);
        if (mCurrentMouseButton != Input::MouseButton::NO_BUTTON) {
            args.button = mCurrentMouseButton;
            if (!mDragging && mSingleClick && fabs(args.position[0] - mDragStart[0]) + fabs(args.position[1] - mDragStart[1]) > mDragStartThreshold) {
                mSingleClick = false;
                if (mPointerDragModes[mCurrentMouseButton] != MouseDragMode::DISABLED) {
                    mDragging = true;
                    mWidget->captureInput();
                    if (mPointerDragModes[mCurrentMouseButton] == MouseDragMode::ENABLED_HIDECURSOR) {
                        mUI.hideCursor();
                    }
                    onPointerDragBegin(args);
                }
            }
        }
        if (mDragging) {
            onPointerDrag(args);
        } else if (mUI.isCursorVisible()) {
            onPointerHover(args);
        }
    }

    void GameHandlerBase::onPointerDown(const Input::PointerEventArgs &me)
    {
        if (mCurrentMouseButton == Input::MouseButton::NO_BUTTON) {
            mCurrentMouseButton = me.button;
            mSingleClick = true;
            mDragStart = me.position;
        }
    }

    void GameHandlerBase::onPointerUp(const Input::PointerEventArgs &me)
    {
        Input::PointerEventArgs args = me;
        clampToWindow(args);
        if (args.button == mCurrentMouseButton) {
            if (mDragging) {
                mDragging = false;
                mWidget->releaseInput();
                if (mPointerDragModes[mCurrentMouseButton] == MouseDragMode::ENABLED_HIDECURSOR) {
                    mUI.showCursor();
                }
                onPointerDragEnd(args);
            } else if (mSingleClick) {
                mSingleClick = false;
                onPointerClick(args);
            }
            mCurrentMouseButton = Input::MouseButton::NO_BUTTON;
        }
    }

    void GameHandlerBase::onPointerHover(const Input::PointerEventArgs &evt)
    {
    }

    void GameHandlerBase::onPointerClick(const Input::PointerEventArgs &evt)
    {
    }

    void GameHandlerBase::onPointerDragBegin(const Input::PointerEventArgs &evt)
    {
    }

    void GameHandlerBase::onPointerDrag(const Input::PointerEventArgs &evt)
    {
    }

    void GameHandlerBase::onPointerDragEnd(const Input::PointerEventArgs &evt)
    {
    }

    void GameHandlerBase::onPointerDragAbort()
    {
    }

    void GameHandlerBase::clampToWindow(Input::PointerEventArgs &me)
    {
        Engine::Vector3 size = mWidget->getActualSize();
        Engine::Vector3 pos = mWidget->getActualPosition();
        if (me.position.x < pos.x)
            me.position.x = pos.x;
        if (me.position.x > pos.x + size.x)
            me.position.x = pos.x + size.x;
        if (me.position.y < pos.y)
            me.position.y = pos.y;
        if (me.position.y > pos.y + size.y)
            me.position.y = pos.y + size.y;
    }

    void GameHandlerBase::setPointerDragMode(Input::MouseButton::MouseButton button, MouseDragMode mode)
    {
        mPointerDragModes[button] = mode;
    }

    const Vector2 &GameHandlerBase::dragStart() const
    {
        return mDragStart;
    }

    void GameHandlerBase::update(std::chrono::microseconds timeSinceLastFrame)
    {
    }

    void GameHandlerBase::fixedUpdate(std::chrono::microseconds timeStep)
    {
    }
}
} // namespace UI
