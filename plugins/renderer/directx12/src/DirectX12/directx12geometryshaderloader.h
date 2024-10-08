#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {
namespace Render {

    struct DirectX12GeometryShaderLoader : Resources::ResourceLoader<DirectX12GeometryShaderLoader, ReleasePtr<IDxcBlob>, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX12GeometryShaderLoader();

        struct Handle : Base::Handle {

            using Base::Handle::Handle;
            Handle(Base::Handle handle)
                : Base::Handle(std::move(handle))
            {
            }

            //void create(const std::string &name, const CodeGen::ShaderFile &file, DirectX12PixelShaderLoader *loader = nullptr);
        };

        bool loadImpl(ReleasePtr<IDxcBlob> &shader, ResourceDataInfo &info);
        void unloadImpl(ReleasePtr<IDxcBlob> &shader);

        //bool create(ReleasePtr<ID3DBlob> &shader, Resource *res, const CodeGen::ShaderFile &file);

        bool loadFromSource(ReleasePtr<IDxcBlob> &shader, std::string_view name, std::string source);

        virtual Threading::TaskQueue *loadingTaskQueue() const override;

    private:
        ReleasePtr<IDxcLibrary> mLibrary;
        ReleasePtr<IDxcCompiler3> mCompiler;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX12GeometryShaderLoader)