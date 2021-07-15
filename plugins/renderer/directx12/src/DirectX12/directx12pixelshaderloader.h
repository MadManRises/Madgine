#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "util/directx12pixelshader.h"

namespace Engine {
namespace Render {

    struct DirectX12PixelShaderLoader : Resources::ResourceLoader<DirectX12PixelShaderLoader, DirectX12PixelShader, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX12PixelShaderLoader();

        struct HandleType : Base::HandleType {

            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(std::move(handle))
            {
            }

            void create(const std::string &name, const CodeGen::ShaderFile &file, DirectX12PixelShaderLoader *loader = nullptr);
        };


        bool loadImpl(DirectX12PixelShader &shader, ResourceDataInfo &info);
        void unloadImpl(DirectX12PixelShader &shader, ResourceDataInfo &info);

        bool create(DirectX12PixelShader &shader, ResourceType *res, const CodeGen::ShaderFile &file);

        bool loadFromSource(DirectX12PixelShader &shader, std::string_view name, std::string_view source);
    };

}
}

RegisterType(Engine::Render::DirectX12PixelShaderLoader);