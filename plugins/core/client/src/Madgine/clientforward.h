#pragma once

namespace Engine {

namespace Window {

    struct MainWindow;
    struct ToolWindow;

	struct MainWindowComponentBase;
    struct MainWindowComponentComparator;
}

namespace Threading {
    enum class ContextMask : unsigned int;
}

namespace Input {
    struct InputHandler;
    struct InputListener;

    struct KeyEventArgs;
    struct PointerEventArgs;

    namespace MouseButton {
        enum MouseButton : unsigned char;
    }

    enum Key : uint8_t;
}

namespace Render {
    struct RendererBase;
    struct RenderContext;
    struct RenderTarget;
    struct RenderPass;
    struct Camera;
}

}
