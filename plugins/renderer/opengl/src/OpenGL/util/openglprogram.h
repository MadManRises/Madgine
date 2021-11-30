#pragma once

#include "program.h"

#include "openglbuffer.h"

#include "openglssbobuffer.h"

#include "../openglshaderloader.h"

#include "openglvertexarrayobject.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLProgram : Program {

        OpenGLProgram() = default;
        OpenGLProgram(OpenGLProgram &&other);
        ~OpenGLProgram();

        OpenGLProgram &operator=(OpenGLProgram &&other);

        bool link(typename OpenGLShaderLoader::HandleType vertexShader, typename OpenGLShaderLoader::HandleType pixelShader, typename OpenGLShaderLoader::HandleType geometryShader = {});

        void reset();

        void bind(const OpenGLVertexArray *format) const;
        void unbind(const OpenGLVertexArray *format) const;

        void setParametersSize(size_t index, size_t size);
        WritableByteBuffer mapParameters(size_t index);

        void setInstanceDataSize(size_t size);
        void setInstanceData(const ByteBuffer &data);

        void setDynamicParameters(size_t index, const ByteBuffer &data);

        struct UniformBufferDescriptor {
            std::string mName;
            uint32_t mSlot;
            uint32_t mSize;
        };
        std::vector<UniformBufferDescriptor> uniformBuffers();
        void verify() const;

    private:
        OpenGLShaderLoader::HandleType mVertexShader;
        OpenGLShaderLoader::HandleType mPixelShader;
        OpenGLShaderLoader::HandleType mGeometryShader;
        mutable GLuint mHandle = 0;
        std::vector<OpenGLBuffer> mUniformBuffers;
#if !OPENGL_ES
        std::vector<OpenGLBuffer> mShaderStorageBuffers;
#else
        std::vector<OpenGLSSBOBuffer> mShaderStorageBuffers;
        mutable OpenGLBuffer mShaderStorageOffsetBuffer = { GL_UNIFORM_BUFFER };
#endif
        size_t mInstanceDataSize = 0;
        OpenGLBuffer mInstanceBuffer = GL_ARRAY_BUFFER;
    };

}
}