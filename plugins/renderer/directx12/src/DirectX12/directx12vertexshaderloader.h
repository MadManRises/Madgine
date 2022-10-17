#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "Madgine/render/vertexformat.h"

namespace Engine {
namespace Render {

    struct DirectX12VertexShaderLoader : Resources::ResourceLoader<DirectX12VertexShaderLoader, ReleasePtr<IDxcBlob>, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX12VertexShaderLoader();

        struct Handle : Base::Handle {

            using Base::Handle::Handle;
            Handle(Base::Handle handle)
                : Base::Handle(std::move(handle))
            {
            }

            Threading::TaskFuture<bool> create(std::string_view name, const CodeGen::ShaderFile &file, DirectX12VertexShaderLoader *loader = &DirectX12VertexShaderLoader::getSingleton());
        };


        bool loadImpl(ReleasePtr<IDxcBlob> &shader, ResourceDataInfo &info);
        void unloadImpl(ReleasePtr<IDxcBlob> &shader);

        bool create(ReleasePtr<IDxcBlob> &shader, Resource *res, const CodeGen::ShaderFile &file);

        bool loadFromSource(ReleasePtr<IDxcBlob> &shader, std::string_view name, std::string source);

        virtual Threading::TaskQueue *loadingTaskQueue() const override;

    private:
        ReleasePtr<IDxcLibrary> mLibrary;
        ReleasePtr<IDxcCompiler3> mCompiler;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX12VertexShaderLoader)