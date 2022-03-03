#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "util/directx12vertexshader.h"

namespace Engine {
namespace Render {

    struct DirectX12VertexShaderLoader : Resources::ResourceLoader<DirectX12VertexShaderLoader, DirectX12VertexShader, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX12VertexShaderLoader();

        struct HandleType : Base::HandleType {

            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(std::move(handle))
            {
            }

            void create(const std::string &name, const CodeGen::ShaderFile &file, DirectX12VertexShaderLoader *loader = nullptr);
        };

        bool loadImpl(DirectX12VertexShader &shader, ResourceDataInfo &info);
        void unloadImpl(DirectX12VertexShader &shader);

        bool create(DirectX12VertexShader &shader, ResourceType *res, const CodeGen::ShaderFile &file);
    };

}
}

RegisterType(Engine::Render::DirectX12VertexShaderLoader);