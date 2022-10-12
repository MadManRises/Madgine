#pragma once

#include "Madgine/render/vertexformat.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    struct GPUMeshData;
    struct TextureDescriptor;

    struct PipelineConfiguration {
        std::string_view vs;
        std::string_view ps;
        std::string_view gs;
        VertexFormat format;
        std::vector<size_t> bufferSizes;
        size_t instanceDataSize = 0;
    };

    struct MADGINE_PIPELINELOADER_EXPORT PipelineInstance {
        PipelineInstance(const PipelineConfiguration &config)
            : mFormat(config.format)
            , mInstanceDataSize(config.instanceDataSize)
        {
        }
        PipelineInstance(const PipelineInstance &) = delete;
        virtual ~PipelineInstance() = default;

        PipelineInstance &operator=(const PipelineInstance &) = delete;

        virtual WritableByteBuffer mapParameters(size_t index) = 0;
        template <typename T>
        ByteBufferImpl<T> mapParameters(size_t index)
        {
            return mapParameters(index).cast<T>();
        }

        virtual void setDynamicParameters(size_t index, const ByteBuffer &data) = 0;

        virtual void renderMesh(const GPUMeshData *mesh) const = 0;
        virtual void renderMeshInstanced(size_t count, const GPUMeshData *mesh, const ByteBuffer &instanceData) const = 0;
        template <typename T>
        void renderMeshInstanced(T &&instanceData, const GPUMeshData *mesh) const
        {
            ByteBufferImpl buffer { std::forward<T>(instanceData) };
            renderMeshInstanced(buffer.elementCount(), mesh, std::move(buffer).template cast<const void>());
        }

        void renderQuad() const;

        virtual void bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset = 0) const = 0;

        VertexFormat mFormat;
        size_t mInstanceDataSize = 0;
    };

}
}