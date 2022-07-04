#pragma once

#include "Madgine/resources/virtualresourceloader.h"
#include "Modules/threading/workgroupstorage.h"

#include "Generic/bytebuffer.h"

#include "render/vertexformat.h"

#include "pipeline.h"
#include "pipelineinstance.h"

namespace Engine {
namespace Render {


    struct MADGINE_PIPELINELOADER_EXPORT PipelineLoader : Resources::VirtualResourceLoaderBase<PipelineLoader, Pipeline, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {

        using Base = Resources::VirtualResourceLoaderBase<PipelineLoader, Pipeline, std::list<Placeholder<0>>, Threading::WorkGroupStorage>;

        struct MADGINE_PIPELINELOADER_EXPORT Instance {

            Instance() = default;
            Instance(std::unique_ptr<PipelineInstance> ptr);

            Instance &operator=(std::unique_ptr<PipelineInstance> ptr);

            Threading::TaskFuture<bool> createStatic(PipelineConfiguration config, PipelineLoader *loader = &PipelineLoader::getSingleton());
            Threading::TaskFuture<bool> createDynamic(PipelineConfiguration config, PipelineLoader *loader = &PipelineLoader::getSingleton());

            WritableByteBuffer mapParameters(size_t index);

            void setInstanceData(const ByteBuffer &data);

            void setDynamicParameters(size_t index, const ByteBuffer &data);

            void reset();

            operator PipelineInstance *() const;

        private:
            std::unique_ptr<PipelineInstance> mPtr;
        };

        PipelineLoader();

        virtual Threading::Task<bool> create(Instance &instance, PipelineConfiguration config, bool dynamic) = 0;
        //virtual bool create(Instance &instance, const std::string &name, const CodeGen::ShaderFile &file) = 0;
    };

}
}

REGISTER_TYPE(Engine::Render::PipelineLoader)