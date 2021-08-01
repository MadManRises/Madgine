#pragma once

#include "Madgine/render/rendertarget.h"
#include "Meta/math/matrix4.h"
#include "util/directx12buffer.h"
#include "util/directx12program.h"
#include "util/directx12texture.h"

#include "render/renderpassflags.h"

#include "Madgine/render/rendertextureconfig.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX12_EXPORT DirectX12RenderTarget : RenderTarget {

        DirectX12RenderTarget(DirectX12RenderContext *context);
        ~DirectX12RenderTarget();

        void setup(OffsetPtr targetView, const Vector2i &size, const RenderTextureConfig &config = {});
        void shutdown();

        virtual void beginFrame() override;

        virtual void setRenderSpace(const Rect2i &space) override;
        virtual void renderVertices(Program *program, size_t groupSize, std::vector<Vertex> vertices, std::vector<unsigned short> indices = {}) override;
        virtual void renderVertices(Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices = {}, TextureHandle texture = 0) override;
        virtual void renderMesh(GPUMeshData *mesh, Program *program) override;
        virtual void clearDepthBuffer() override;

        virtual void bindTextures(const std::vector<TextureHandle> &tex) override;

        //void setupProgram(RenderPassFlags flags = RenderPassFlags_None, unsigned int textureId = 0) override;

        OffsetPtr mTargetView;
        ID3D12Resource *mDepthStencilBuffer = nullptr;  
        OffsetPtr mDepthStencilView;
        D3D12_DEPTH_STENCIL_DESC mDepthStencilStateDesc;
        D3D12_RASTERIZER_DESC mRasterizerDesc;
        D3D12_BLEND_DESC mBlendState;

        //ID3D12SamplerState *mSamplers[2] = { nullptr, nullptr };
        
    };

}
}