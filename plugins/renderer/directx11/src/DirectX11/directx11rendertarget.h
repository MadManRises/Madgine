#pragma once

#include "Madgine/render/rendertarget.h"
#include "Meta/math/matrix4.h"
#include "util/directx11buffer.h"
#include "util/directx11program.h"
#include "util/directx11texture.h"

#include "Madgine/render/rendertextureconfig.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderTarget : RenderTarget {

        DirectX11RenderTarget(DirectX11RenderContext *context, bool global, std::string name, size_t iterations = 1);
        ~DirectX11RenderTarget();

        void setup(std::vector<ReleasePtr<ID3D11RenderTargetView>> targetViews, const Vector2i &size, TextureType type, size_t samples = 1);
        void shutdown();

        virtual void beginIteration(size_t iteration) const override;
        virtual void endIteration(size_t iteration) const override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        virtual void setRenderSpace(const Rect2i &space) override;
        virtual void renderVertices(const Program *program, size_t groupSize, std::vector<Vertex> vertices, std::vector<uint32_t> indices = {}) override;
        virtual void renderVertices(const Program *program, size_t groupSize, std::vector<Vertex2> vertices, std::vector<uint32_t> indices = {}, const Material *material = nullptr) override;
        virtual void renderMesh(const GPUMeshData *mesh, const Program *program, const Material *material = nullptr) override;
        virtual void renderMeshInstanced(size_t count, const GPUMeshData *mesh, const Program *program, const Material *material = nullptr) override;
        virtual void clearDepthBuffer() override;

        virtual void bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset = 0) const override;

        //void setupProgram(RenderPassFlags flags = RenderPassFlags_None, unsigned int textureId = 0) override;

        std::vector<ReleasePtr<ID3D11RenderTargetView>> mTargetViews;
        DirectX11Texture mDepthBuffer;
        ReleasePtr<ID3D11DepthStencilView> mDepthStencilView;
        ReleasePtr<ID3D11DepthStencilState> mDepthStencilState;
        ReleasePtr<ID3D11RasterizerState> mRasterizerState;
        ReleasePtr<ID3D11BlendState> mBlendState;

        ReleasePtr<ID3D11SamplerState> mSamplers[2];
    };

}
}