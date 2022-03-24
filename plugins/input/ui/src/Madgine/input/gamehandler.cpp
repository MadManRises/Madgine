#include "../uilib.h"
#include "gamehandler.h"
#include "Madgine/widgets/widget.h"
#include "uimanager.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentregistry.h"

DEFINE_UNIQUE_COMPONENT(Engine::Input, GameHandler)

METATABLE_BEGIN_BASE(Engine::Input::GameHandlerBase, Engine::Input::Handler)
METATABLE_END(Engine::Input::GameHandlerBase)

SERIALIZETABLE_INHERIT_BEGIN(Engine::Input::GameHandlerBase, Engine::Input::Handler)
SERIALIZETABLE_END(Engine::Input::GameHandlerBase)


namespace Engine {
namespace Input {

    GameHandlerBase::GameHandlerBase(UIManager &ui)
        : Handler(ui)
        , mCurrentMouseButton(MouseButton::NO_BUTTON)
        , mDragging(false)
        , mSingleClick(false)
    {
    }

    void GameHandlerBase::abortDrag()
    {
        mDragging = false;
        onPointerDragAbort();
    }

    void GameHandlerBase::onPointerMove(const PointerEventArgs &me)
    {
        PointerEventArgs args = me;
        clampToWindow(args);
        if (mCurrentMouseButton != MouseButton::NO_BUTTON) {
            args.button = mCurrentMouseButton;
            if (!mDragging && mSingleClick && fabs(args.windowPosition.x - mDragStart.x) + fabs(args.windowPosition.y - mDragStart.y) > mPointerDragModes[mCurrentMouseButton].mThreshold)
        {
                mSingleClick = false;
                if (mPointerDragModes[mCurrentMouseButton].mMode != MouseDragMode::DISABLED) {
                    mDragging = true;
                    if (mPointerDragModes[mCurrentMouseButton].mMode == MouseDragMode::ENABLED_HIDECURSOR) {
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

    void GameHandlerBase::onPointerDown(const PointerEventArgs &me)
    {
        if (mCurrentMouseButton == MouseButton::NO_BUTTON) {
            mCurrentMouseButton = me.button;
            mSingleClick = true;
            mDragStart = me.windowPosition;
        }
    }

    void GameHandlerBase::onPointerUp(const PointerEventArgs &me)
    {
        PointerEventArgs args = me;
        clampToWindow(args);
        if (args.button == mCurrentMouseButton) {
            if (mDragging) {
                mDragging = false;
                if (mPointerDragModes[mCurrentMouseButton].mMode == MouseDragMode::ENABLED_HIDECURSOR) {
                    mUI.showCursor();
                }
                onPointerDragEnd(args);
            } else if (mSingleClick) {
                mSingleClick = false;
                onPointerClick(args);
            }
            mCurrentMouseButton = MouseButton::NO_BUTTON;
        }
    }

    void GameHandlerBase::onPointerHover(const PointerEventArgs &evt)
    {
    }

    void GameHandlerBase::onPointerClick(const PointerEventArgs &evt)
    {
    }

    void GameHandlerBase::onPointerDragBegin(const PointerEventArgs &evt)
    {
    }

    void GameHandlerBase::onPointerDrag(const PointerEventArgs &evt)
    {
    }

    void GameHandlerBase::onPointerDragEnd(const PointerEventArgs &evt)
    {
    }

    void GameHandlerBase::onPointerDragAbort()
    {
    }

    void GameHandlerBase::clampToWindow(PointerEventArgs &me)
    {
        Engine::Vector3 size = mWidget->getAbsoluteSize();
        if (me.windowPosition.x < 0.0f)
            me.windowPosition.x = 0.0f;
        if (me.windowPosition.x > size.x)
            me.windowPosition.x = size.x;
        if (me.windowPosition.y < 0)
            me.windowPosition.y = 0;
        if (me.windowPosition.y > size.y)
            me.windowPosition.y = size.y;
    }

    void GameHandlerBase::setPointerDragMode(MouseButton::MouseButton button, MouseDragMode mode, float threshold)
    {
        mPointerDragModes[button] = { mode, threshold };
    }

    const InterfacesVector &GameHandlerBase::dragStart() const
    {
        return mDragStart;
    }

    void GameHandlerBase::update(std::chrono::microseconds timeSinceLastFrame)
    {
    }

    void GameHandlerBase::fixedUpdate(std::chrono::microseconds timeStep)
    {
    }

    bool GameHandlerBase::dragging() const {
        return mDragging;
    }

}
} // namespace UI
