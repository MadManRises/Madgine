#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {
namespace Render {

    struct DirectX11VertexShader {
        ReleasePtr<ID3D11VertexShader> mShader;
        ReleasePtr<ID3D10Blob> mBlob;
    };

    struct DirectX11VertexShaderLoader : Resources::ResourceLoader<DirectX11VertexShaderLoader, DirectX11VertexShader, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX11VertexShaderLoader();

        struct Ptr : Base::Ptr {

            using Base::Ptr::Ptr;
            Ptr(Base::Ptr ptr)
                : Base::Ptr(std::move(ptr))
            {
            }

            Threading::TaskFuture<bool> create(const CodeGen::ShaderFile &file, DirectX11VertexShaderLoader *loader = &DirectX11VertexShaderLoader::getSingleton());
        };

        bool loadImpl(DirectX11VertexShader &shader, ResourceDataInfo &info);
        void unloadImpl(DirectX11VertexShader &shader);

        bool create(DirectX11VertexShader &shader, const CodeGen::ShaderFile &file);

        bool loadFromSource(DirectX11VertexShader &shader, std::string_view name, std::string source);

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX11VertexShaderLoader)