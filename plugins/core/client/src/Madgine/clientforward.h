#pragma once

namespace Engine {

namespace GUI {

    struct TopLevelWindow;
    struct ToolWindow;

	struct TopLevelWindowComponentBase;
    struct TopLevelWindowComponentComparator;

    struct WindowOverlay;
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
}

namespace Render {
    struct RendererBase;
    struct RenderContext;
    struct RenderTarget;
    struct RenderPass;
    struct Camera;
}

}
