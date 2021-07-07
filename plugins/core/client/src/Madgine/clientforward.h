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



namespace Render {
    struct RendererBase;
    struct RenderContext;
    struct RenderTarget;
    struct RenderTextureConfig;
    struct RenderPass;
    struct Camera;
    struct TextureDescriptor;
    
}

}
