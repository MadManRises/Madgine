#pragma once

namespace Engine {

namespace UI {
    class Handler;
    class GameHandlerBase;
    class GuiHandlerBase;
    struct UIManager;
}

namespace GUI {
    class GUISystem;
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
    class TopLevelWindow;
    class ToolWindow;

    enum class WidgetClass;

    struct WindowOverlay;

    struct Vertex;

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
    class RendererBase;
    class RenderWindow;
    struct RenderTarget;
    struct RenderPass;
}

}
