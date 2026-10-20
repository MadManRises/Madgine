#pragma once

namespace Engine {
namespace Widgets {

    struct PointerClickEvent {
        mutable Platform::PlatformVector mWindowPosition;
        Platform::PlatformVector mScreenPosition;
        Platform::Input::MouseButton::MouseButton mButton;
        Platform::Input::ControlKeyState mControlKeys;
    };

    struct DragBeginEvent {
        mutable Platform::PlatformVector mWindowPosition;
        Platform::PlatformVector mScreenPosition;
        Platform::Input::MouseButton::MouseButton mButton;
        Platform::Input::ControlKeyState mControlKeys;
    };

    struct DragMoveEvent {
        mutable Platform::PlatformVector mWindowPosition;
        Platform::PlatformVector mScreenPosition;
        Platform::Input::MouseButton::MouseButton mButton;
        Platform::PlatformVector mMoveDelta;
        Platform::Input::ControlKeyState mControlKeys;
    };

    struct DragEndEvent {
        mutable Platform::PlatformVector mWindowPosition;
        Platform::PlatformVector mScreenPosition;
        Platform::Input::MouseButton::MouseButton mButton;
        Platform::Input::ControlKeyState mControlKeys;
    };

}
}