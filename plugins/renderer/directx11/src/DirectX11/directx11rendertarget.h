#pragma once

#include "Madgine/render/rendertarget.h"
#include "Meta/math/matrix4.h"
#include "util/directx11buffer.h"
#include "util/directx11program.h"
#include "util/directx11texture.h"

#include "render/renderpassflags.h"

#include "Madgine/render/rendertextureconfig.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderTarget : RenderTarget {

        DirectX11RenderTarget(DirectX11RenderContext *context);
        ~DirectX11RenderTarget();

        void setup(ID3D11RenderTargetView *targetView, const Vector2i &size, const RenderTextureConfig &config = {});
        void shutdown();

        virtual void beginFrame() override;

        virtual void setRenderSpace(const Rect2i &space) override;
        virtual void renderVertices(Program *program, size_t groupSize, std::vector<Vertex> vertices, std::vector<unsigned short> indices = {}) override;
        virtual void renderVertices(Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<unsigned short> indices = {}, TextureHandle texture = 0) override;
        virtual void renderMesh(GPUMeshData *mesh, Program *program) override;
        virtual void clearDepthBuffer() override;

        virtual void bindTexture(const std::vector<TextureHandle> &tex) override;

        //void setupProgram(RenderPassFlags flags = RenderPassFlags_None, unsigned int textureId = 0) override;

        ID3D11RenderTargetView *mTargetView = nullptr;
        ID3D11Texture2D *mDepthStencilBuffer = nullptr;        
        ID3D11DepthStencilView *mDepthStencilView = nullptr;
        ID3D11DepthStencilState *mDepthStencilState = nullptr;
        ID3D11RasterizerState *mRasterizerState = nullptr;
        ID3D11BlendState *mBlendState = nullptr;

        ID3D11SamplerState *mSamplers[2] = { nullptr, nullptr };
        
    };

}
}