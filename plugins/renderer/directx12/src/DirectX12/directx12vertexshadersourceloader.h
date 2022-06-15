#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {
namespace Render {

    struct DirectX12VertexShaderSourceLoader : Resources::ResourceLoader<DirectX12VertexShaderSourceLoader, std::string, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        DirectX12VertexShaderSourceLoader();

        struct HandleType : Base::HandleType {

            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(std::move(handle))
            {
            }

            //void create(const std::string &name, const CodeGen::ShaderFile &file, DirectX12VertexShaderLoader *loader = nullptr);
        };

        bool loadImpl(std::string &shader, ResourceDataInfo &info);
        void unloadImpl(std::string &shader);

        //bool create(DirectX12VertexShader &shader, ResourceType *res, const CodeGen::ShaderFile &file);

        virtual Threading::TaskQueue *loadingTaskQueue() const override;
    };

}
}

REGISTER_TYPE(Engine::Render::DirectX12VertexShaderSourceLoader)