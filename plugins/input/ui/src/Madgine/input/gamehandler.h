#pragma once

#include "handler.h"

#include "Interfaces/input/inputevents.h"

#include "handlercollector.h"

namespace Engine {
namespace Input {
    struct MADGINE_UI_EXPORT GameHandlerBase : Handler {
        static constexpr float sDefaultDragStartThreshold = 80.0f;

        GameHandlerBase(UIManager &ui, std::string_view widgetName);

        void abortDrag();

        virtual void updateRender(std::chrono::microseconds timeSinceLastFrame);
        virtual void fixedUpdateRender(std::chrono::microseconds timeStep);
        virtual void updateApp(std::chrono::microseconds timeSinceLastFrame);

    protected:
        void onPointerMove(const PointerEventArgs &me) override;

        void onPointerDown(const PointerEventArgs &me) override;

        void onPointerUp(const PointerEventArgs &me) override;

        virtual void onPointerHover(const PointerEventArgs &evt);

        virtual void onPointerClick(const PointerEventArgs &evt);

        virtual void onPointerDragBegin(const PointerEventArgs &evt);

        virtual void onPointerDrag(const PointerEventArgs &evt);

        virtual void onPointerDragEnd(const PointerEventArgs &evt);

        virtual void onPointerDragAbort();


        void clampToWindow(PointerEventArgs &me);

        enum class MouseDragMode {
            DISABLED = 0,
            ENABLED,
            ENABLED_HIDECURSOR
        };

        void setPointerDragMode(MouseButton::MouseButton button, MouseDragMode mode, float threshold = sDefaultDragStartThreshold);

        const InterfacesVector &dragStart() const;

        bool dragging() const;

    private:


        struct MouseDragInfo {
            MouseDragMode mMode;
            float mThreshold = sDefaultDragStartThreshold;
        };

        std::array<MouseDragInfo, MouseButton::BUTTON_COUNT> mPointerDragModes;

        MouseButton::MouseButton mCurrentMouseButton;

        bool mDragging, mSingleClick;

        InterfacesVector mDragStart;
    };

} // namespace UI

}

RegisterType(Engine::Input::GameHandlerBase);
