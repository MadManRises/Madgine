#pragma once

namespace Engine {

namespace Render {
    struct OpenGLRenderWindow;

    struct OpenGLShader;
    struct OpenGLBuffer;
    struct OpenGLProgram;
    struct OpenGLVertexArray;

    struct OpenGLMeshData;

    struct OpenGLFontData;

	struct OpenGLRenderContext;

    enum ShaderType {
            VertexShader,
            PixelShader,
            GeometryShader
        };

    constexpr struct create_t {
    } create;
}

}
