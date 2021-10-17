#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "util/directx11vertexshader.h"

namespace Engine {
namespace Render {

    struct DirectX11VertexShaderLoader : Resources::ResourceLoader<DirectX11VertexShaderLoader, DirectX11VertexShader, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX11VertexShaderLoader();

        struct HandleType : Base::HandleType {

            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(std::move(handle))
            {
            }

            void create(const std::string &name, const CodeGen::ShaderFile &file, DirectX11VertexShaderLoader *loader = nullptr);
        };

        bool loadImpl(DirectX11VertexShader &shader, ResourceDataInfo &info);
        void unloadImpl(DirectX11VertexShader &shader, ResourceDataInfo &info);

        bool create(DirectX11VertexShader &shader, ResourceType *res, const CodeGen::ShaderFile &file);

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };

}
}

RegisterType(Engine::Render::DirectX11VertexShaderLoader);