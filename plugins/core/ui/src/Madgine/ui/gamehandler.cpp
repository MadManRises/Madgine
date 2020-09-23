#include "../uilib.h"
#include "gamehandler.h"
#include "Madgine/widgets/widget.h"
#include "uimanager.h"

#include "Modules/keyvalue/metatable_impl.h"


DEFINE_UNIQUE_COMPONENT(Engine::UI, GameHandler)

METATABLE_BEGIN_BASE(Engine::UI::GameHandlerBase, Engine::UI::Handler)
METATABLE_END(Engine::UI::GameHandlerBase)



namespace Engine {
namespace UI {
    const float GameHandlerBase::mDragStartThreshold = 100.0f;

    GameHandlerBase::GameHandlerBase(UIManager &ui, Threading::ContextMask context)
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
        onPointerDragAbort();
    }

    void GameHandlerBase::update(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask mask)
    {
        if (mContext & mask)
            update(timeSinceLastFrame);
    }

    void GameHandlerBase::fixedUpdate(std::chrono::microseconds timeStep, Threading::ContextMask mask)
    {
        if (mContext & mask)
            fixedUpdate(timeStep);
    }

    void GameHandlerBase::onPointerMove(const Input::PointerEventArgs &me)
    {
        Input::PointerEventArgs args = me;
        clampToWindow(args);
        if (mCurrentMouseButton != Input::MouseButton::NO_BUTTON) {
            args.button = mCurrentMouseButton;            
            if (!mDragging && mSingleClick && fabs(args.windowPosition.x - mDragStart.x) + fabs(args.windowPosition.y - mDragStart.y) > mDragStartThreshold) {
                mSingleClick = false;
                if (mPointerDragModes[mCurrentMouseButton] != MouseDragMode::DISABLED) {
                    mDragging = true;
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
            mDragStart = Vector2 { me.windowPosition };
        }
    }

    void GameHandlerBase::onPointerUp(const Input::PointerEventArgs &me)
    {
        Input::PointerEventArgs args = me;
        clampToWindow(args);
        if (args.button == mCurrentMouseButton) {
            if (mDragging) {
                mDragging = false;
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
        if (me.windowPosition.x < 0.0f)
            me.windowPosition.x = 0.0f;
        if (me.windowPosition.x > size.x)
            me.windowPosition.x = size.x;
        if (me.windowPosition.y < 0)
            me.windowPosition.y = 0;
        if (me.windowPosition.y > size.y)
            me.windowPosition.y = size.y;
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
