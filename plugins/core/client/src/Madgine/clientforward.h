#pragma once

namespace Engine {

namespace UI {
    class Handler;
    class GameHandlerBase;
    class GuiHandlerBase;
    struct UIManager;
}

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

namespace Resources {
    struct MeshData;
}

namespace Widgets {
    struct WidgetManager;

	class WidgetBase;
    class Bar;
    class Button;
    class SceneWindow;
    class Checkbox;
    class Combobox;
    class TabWidget;
    class Textbox;
    class Label;
    class Image;

    enum class WidgetClass;

}

}
