#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {
namespace Render {

    struct DirectX11PixelShaderLoader : Resources::ResourceLoader<DirectX11PixelShaderLoader, ReleasePtr<ID3D11PixelShader>, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX11PixelShaderLoader();

        struct HandleType : Base::HandleType {

            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(std::move(handle))
            {
            }

            void create(const std::string &name, const CodeGen::ShaderFile &file, DirectX11PixelShaderLoader *loader = nullptr);
        };

        bool loadImpl(ReleasePtr<ID3D11PixelShader> &shader, ResourceDataInfo &info);
        void unloadImpl(ReleasePtr<ID3D11PixelShader> &shader);

        bool create(ReleasePtr<ID3D11PixelShader> &shader, ResourceType *res, const CodeGen::ShaderFile &file);

        bool loadFromSource(ReleasePtr<ID3D11PixelShader> &shader, std::string_view name, std::string source);

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX11PixelShaderLoader)