#pragma once

#include "directx11rendertarget.h"
#include "Modules/math/matrix4.h"
#include "util/directx11program.h"
#include "util/directx11buffer.h"
#include "util/directx11texture.h"

#include "Modules/render/renderpassflags.h"

namespace Engine {
namespace Render {

    struct MADGINE_DIRECTX11_EXPORT DirectX11RenderTexture : DirectX11RenderTarget {

        DirectX11RenderTexture(DirectX11RenderContext *context, const Vector2i &size);
        ~DirectX11RenderTexture();
        
        bool resize(const Vector2i &size) override;
        Vector2i size() const override;
        
        virtual void beginFrame() override;
        virtual void endFrame() override;

        virtual const DirectX11Texture *texture() const override;
        
    private:

        DirectX11Texture mTexture;

		Vector2i mSize;
    };

}
}