#pragma once

#include "Madgine/threading/contextmasks.h"
#include "handler.h"

#include "Interfaces/input/inputevents.h"

#include "handlercollector.h"

namespace Engine {
namespace UI {
    struct MADGINE_UI_EXPORT GameHandlerBase : Handler {
        static constexpr float sDefaultDragStartThreshold = 80.0f;

        GameHandlerBase(UIManager &ui, Threading::ContextMask context = Threading::ContextMask::SceneContext);

        void abortDrag();

        void update(std::chrono::microseconds timeSinceLastFrame, Threading::ContextMask mask);
        void fixedUpdate(std::chrono::microseconds timeStep, Threading::ContextMask mask);

    protected:
        void onPointerMove(const Input::PointerEventArgs &me) override;

        void onPointerDown(const Input::PointerEventArgs &me) override;

        void onPointerUp(const Input::PointerEventArgs &me) override;

        virtual void onPointerHover(const Input::PointerEventArgs &evt);

        virtual void onPointerClick(const Input::PointerEventArgs &evt);

        virtual void onPointerDragBegin(const Input::PointerEventArgs &evt);

        virtual void onPointerDrag(const Input::PointerEventArgs &evt);

        virtual void onPointerDragEnd(const Input::PointerEventArgs &evt);

        virtual void onPointerDragAbort();

        virtual void update(std::chrono::microseconds timeSinceLastFrame);
        virtual void fixedUpdate(std::chrono::microseconds timeStep);

        void clampToWindow(Input::PointerEventArgs &me);

        enum class MouseDragMode {
            DISABLED = 0,
            ENABLED,
            ENABLED_HIDECURSOR
        };

        void setPointerDragMode(Input::MouseButton::MouseButton button, MouseDragMode mode, float threshold = sDefaultDragStartThreshold);

        const InterfacesVector &dragStart() const;

        bool dragging() const;

    private:


        struct MouseDragInfo {
            MouseDragMode mMode;
            float mThreshold = sDefaultDragStartThreshold;
        };

        std::array<MouseDragInfo, Input::MouseButton::BUTTON_COUNT> mPointerDragModes;

        Input::MouseButton::MouseButton mCurrentMouseButton;

        bool mDragging, mSingleClick;

        InterfacesVector mDragStart;

        Threading::ContextMask mContext;
    };

} // namespace UI

}

RegisterType(Engine::UI::GameHandlerBase);
