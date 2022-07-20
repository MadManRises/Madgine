#pragma once

#include "Madgine/render/vertexformat.h"


namespace Engine {
namespace Render {


    struct PipelineConfiguration {
        std::string_view vs;
        std::string_view ps;
        std::string_view gs;
        VertexFormat format;
        std::vector<size_t> bufferSizes;
        size_t instanceDataSize = 0;
    };

    struct PipelineInstance {
        PipelineInstance(const PipelineConfiguration &config)
            : mFormat(config.format)
            , mInstanceDataSize(config.instanceDataSize)
        {
        }
        virtual ~PipelineInstance() = default;

        virtual WritableByteBuffer mapParameters(size_t index) = 0;

        virtual void setInstanceData(const ByteBuffer &data) = 0;

        virtual void setDynamicParameters(size_t index, const ByteBuffer &data) = 0;

        VertexFormat mFormat;
        size_t mInstanceDataSize = 0;
    };

}
}