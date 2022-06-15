#pragma once

namespace Engine {
namespace Render {
    struct Pipeline;
    struct PipelineInstance;

    enum ShaderType {
        VertexShader,
        PixelShader,
        GeometryShader
    };
}
}