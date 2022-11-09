#pragma once

#include "Madgine/render/rendercontext.h"
#include "Madgine/render/rendercontextcollector.h"

#include "Modules/threading/threadlocal.h"

#include "Madgine/render/vertexformat.h"

#include "util/directx11buffer.h"

namespace Engine {
namespace Render {

    extern THREADLOCAL(ID3D11DeviceContext *) sDeviceContext;
    extern THREADLOCAL(ID3D11Device *) sDevice;

    MADGINE_DIRECTX11_EXPORT ID3D11DeviceContext *GetDeviceContext();
    MADGINE_DIRECTX11_EXPORT ID3D11Device *GetDevice();

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderContext : public RenderContextComponent<DirectX11RenderContext> {
        DirectX11RenderContext(Threading::TaskQueue *queue);
        DirectX11RenderContext(const DirectX11RenderContext &) = delete;
        ~DirectX11RenderContext();

        DirectX11RenderContext &operator=(const DirectX11RenderContext &) = delete;

        static DirectX11RenderContext &getSingleton();

        virtual std::unique_ptr<RenderTarget> createRenderWindow(Window::OSWindow *w, size_t samples) override;
        virtual std::unique_ptr<RenderTarget> createRenderTexture(const Vector2i &size = { 1, 1 }, const RenderTextureConfig &config = {}) override;

        virtual void unloadAllResources() override;

        virtual bool supportsMultisampling() const override;

        virtual void pushAnnotation(const char *tag) override;
        virtual void popAnnotation() override;

        void bindFormat(VertexFormat format, size_t instanceDataSize, ID3D10Blob *blob);

    private:
        std::array<std::map<size_t, ReleasePtr<ID3D11InputLayout>>, 256> mInputLayouts;

        DirectX11Buffer mConstantBuffer = D3D11_BIND_VERTEX_BUFFER;

        ID3DUserDefinedAnnotation *mAnnotator = nullptr;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX11RenderContext)