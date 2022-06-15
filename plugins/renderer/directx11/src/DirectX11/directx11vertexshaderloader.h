#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "util/directx11vertexshaderlist.h"

namespace Engine {
namespace Render {

    struct DirectX11VertexShaderLoader : Resources::ResourceLoader<DirectX11VertexShaderLoader, DirectX11VertexShaderList, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX11VertexShaderLoader();

        struct HandleType : Base::HandleType {

            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(std::move(handle))
            {
            }

            //void create(const std::string &name, const CodeGen::ShaderFile &file, DirectX11VertexShaderLoader *loader = nullptr);
        };

        bool loadImpl(DirectX11VertexShaderList &list, ResourceDataInfo &info);
        void unloadImpl(DirectX11VertexShaderList &list);

        bool create(DirectX11VertexShaderList &shader, ResourceType *res, const CodeGen::ShaderFile &file);

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX11VertexShaderLoader)