#pragma once

#include "handler.h"

#include "Interfaces/input/inputevents.h"

#include "handlercollector.h"

namespace Engine {
namespace UI {
    struct MADGINE_UI_EXPORT GameHandlerBase : Handler {
        static constexpr float sDefaultDragStartThreshold = 80.0f;

        GameHandlerBase(UIManager &ui);

        void abortDrag();

        virtual void update(std::chrono::microseconds timeSinceLastFrame);
        virtual void fixedUpdate(std::chrono::microseconds timeStep);

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
    };

} // namespace UI

}

RegisterType(Engine::UI::GameHandlerBase);
