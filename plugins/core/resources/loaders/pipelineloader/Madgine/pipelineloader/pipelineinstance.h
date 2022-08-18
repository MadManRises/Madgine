#pragma once

#include "Madgine/render/vertexformat.h"

#include "Generic/bytebuffer.h"

namespace Engine {
namespace Render {

    struct GPUMeshData;
    struct Material;
    struct TextureDescriptor;
    struct RenderTarget;

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
        virtual ~PipelineInstance() = default;

        virtual WritableByteBuffer mapParameters(size_t index) = 0;
        template <typename T>
        ByteBufferImpl<T> mapParameters(size_t index) {
            return mapParameters(index).cast<T>();
        }

        virtual void setInstanceData(const ByteBuffer &data) = 0;

        virtual void setDynamicParameters(size_t index, const ByteBuffer &data) = 0;

        virtual void renderMesh(const GPUMeshData *mesh, const Material *material = nullptr) const = 0;
        virtual void renderMeshInstanced(size_t count, const GPUMeshData *mesh, const Material *material = nullptr) const = 0;

        void renderQuad() const;

        virtual void bindTextures(const std::vector<TextureDescriptor> &tex, size_t offset = 0) const = 0;

        VertexFormat mFormat;
        size_t mInstanceDataSize = 0;
    };

}
}