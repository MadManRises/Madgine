#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {
namespace Render {

    struct DirectX11PixelShaderLoader : Resources::ResourceLoader<DirectX11PixelShaderLoader, ReleasePtr<ID3D11PixelShader>, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX11PixelShaderLoader();

        struct Ptr : Base::Ptr {

            using Base::Ptr::Ptr;
            Ptr(Base::Ptr ptr)
                : Base::Ptr(std::move(ptr))
            {
            }

            Threading::TaskFuture<bool> create(const CodeGen::ShaderFile &file, DirectX11PixelShaderLoader *loader = &DirectX11PixelShaderLoader::getSingleton());
        };

        bool loadImpl(ReleasePtr<ID3D11PixelShader> &shader, ResourceDataInfo &info);
        void unloadImpl(ReleasePtr<ID3D11PixelShader> &shader);

        bool create(ReleasePtr<ID3D11PixelShader> &shader, const CodeGen::ShaderFile &file);

        bool loadFromSource(ReleasePtr<ID3D11PixelShader> &shader, std::string_view name, std::string source);

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX11PixelShaderLoader)