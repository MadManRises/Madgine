#pragma once

#include "Madgine/resources/virtualresourceloader.h"
#include "Modules/threading/workgroupstorage.h"

#include "Generic/bytebuffer.h"

#include "Madgine/render/vertexformat.h"

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

            Threading::TaskFuture<bool> createGenerated(PipelineConfiguration config, CodeGen::ShaderFile file, PipelineLoader *loader = &PipelineLoader::getSingleton());
            
            WritableByteBuffer mapParameters(size_t index);
            template <typename T>
            auto mapParameters(size_t index) {
                return mapParameters(index).cast<T>();
            }

            void setInstanceData(const ByteBuffer &data);

            void setDynamicParameters(size_t index, const ByteBuffer &data);

            void reset();

            operator PipelineInstance *() const;

        private:
            std::unique_ptr<PipelineInstance> mPtr;
        };

        PipelineLoader();

        virtual Threading::Task<bool> create(Instance &instance, PipelineConfiguration config, bool dynamic) = 0;
        virtual Threading::Task<bool> create(Instance &instance, PipelineConfiguration config, CodeGen::ShaderFile file) = 0;
    };

}
}

REGISTER_TYPE(Engine::Render::PipelineLoader)