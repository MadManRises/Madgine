#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "util/directx11vertexshaderlist.h"

namespace Engine {
namespace Render {

    struct DirectX11VertexShaderLoader : Resources::ResourceLoader<DirectX11VertexShaderLoader, DirectX11VertexShaderList, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX11VertexShaderLoader();

        struct Handle : Base::Handle {

            using Base::Handle::Handle;
            Handle(Base::Handle handle)
                : Base::Handle(std::move(handle))
            {
            }

            Threading::TaskFuture<bool> create(std::string_view name, const CodeGen::ShaderFile &file, DirectX11VertexShaderLoader *loader = &DirectX11VertexShaderLoader::getSingleton());
        };

        bool loadImpl(DirectX11VertexShaderList &list, ResourceDataInfo &info);
        void unloadImpl(DirectX11VertexShaderList &list);

        bool create(DirectX11VertexShaderList &shader, Resource *res, const CodeGen::ShaderFile &file);

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX11VertexShaderLoader)