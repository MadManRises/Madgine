#pragma once

namespace Engine {

namespace Render {
    struct OpenGLRenderWindow;

    struct OpenGLShader;

    struct OpenGLMeshData;

    struct OpenGLFontData;

	struct OpenGLRenderContext;

    enum ShaderType {
            VertexShader,
            PixelShader
        };

    constexpr struct create_t {
    } create;
}

}
