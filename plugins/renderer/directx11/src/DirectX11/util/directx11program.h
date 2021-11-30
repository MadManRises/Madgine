#pragma once

#include "program.h"

#include "../directx11pixelshaderloader.h"
#include "../directx11vertexshaderloader.h"
#include "../directx11geometryshaderloader.h"
#include "directx11buffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11Program : Program {

        DirectX11Program() = default;
        DirectX11Program(DirectX11Program &&other);
        ~DirectX11Program();

        DirectX11Program &operator=(DirectX11Program &&other);

        bool link(typename DirectX11VertexShaderLoader::HandleType vertexShader, typename DirectX11PixelShaderLoader::HandleType pixelShader, typename DirectX11GeometryShaderLoader::HandleType geometryShader);

        void reset();

        void bind(const DirectX11VertexArray *format) const;

        void setParametersSize(size_t index, size_t size);
        WritableByteBuffer mapParameters(size_t index);

        void setInstanceDataSize(size_t size);
        void setInstanceData(const ByteBuffer &data);

        void setDynamicParameters(size_t index, const ByteBuffer &data);

    private:
        DirectX11VertexShaderLoader::HandleType mVertexShader;
        DirectX11PixelShaderLoader::HandleType mPixelShader;
        DirectX11GeometryShaderLoader::HandleType mGeometryShader;
        std::vector<DirectX11Buffer> mConstantBuffers;
        std::vector<DirectX11Buffer> mDynamicBuffers;
        size_t mInstanceDataSize = 0;
        DirectX11Buffer mInstanceBuffer = D3D11_BIND_VERTEX_BUFFER;
    };

}
}