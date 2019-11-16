#pragma once

namespace Engine {

namespace GUI {

    class TopLevelWindow;
    class ToolWindow;

    struct WindowOverlay;
}

namespace Threading {
    enum class ContextMask : unsigned int;
}

namespace Input {
    class InputHandler;
    class InputListener;

    struct KeyEventArgs;
    struct PointerEventArgs;

    namespace MouseButton {
        enum MouseButton : unsigned char;
    }
}

namespace Render {
    struct RendererBase;
    struct RenderContext;
    struct RenderTarget;
    struct RenderPass;
    struct Camera;
}

}
