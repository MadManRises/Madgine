#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {
namespace Render {

    struct DirectX11GeometryShaderLoader : Resources::ResourceLoader<DirectX11GeometryShaderLoader, ReleasePtr<ID3D11GeometryShader>, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX11GeometryShaderLoader();

        struct Handle : Base::Handle {

            using Base::Handle::Handle;
            Handle(Base::Handle handle)
                : Base::Handle(std::move(handle))
            {
            }

            void create(const std::string &name, const CodeGen::ShaderFile &file, DirectX11GeometryShaderLoader *loader = &DirectX11GeometryShaderLoader::getSingleton());
        };


        bool loadImpl(ReleasePtr<ID3D11GeometryShader> &shader, ResourceDataInfo &info);
        void unloadImpl(ReleasePtr<ID3D11GeometryShader> &shader);

        bool create(ReleasePtr<ID3D11GeometryShader> &shader, Resource *res, const CodeGen::ShaderFile &file);

        bool loadFromSource(ReleasePtr<ID3D11GeometryShader> &shader, std::string_view name, std::string source);

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX11GeometryShaderLoader)