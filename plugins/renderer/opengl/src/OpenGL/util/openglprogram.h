#pragma once

#include "program.h"

#include "openglbuffer.h"

#include "openglssbobuffer.h"

#include "../openglshaderloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLProgram : Program {

        OpenGLProgram() = default;
        OpenGLProgram(OpenGLProgram &&other);
        ~OpenGLProgram();

        OpenGLProgram &operator=(OpenGLProgram &&other);

        bool link(typename OpenGLShaderLoader::HandleType vertexShader, typename OpenGLShaderLoader::HandleType pixelShader);

        void reset();

        void bind();

        void setParameters(const ByteBuffer &data, size_t index);
        WritableByteBuffer mapParameters(size_t index);

        void setDynamicParameters(const ByteBuffer &data, size_t index);

        struct UniformBufferDescriptor {
            std::string mName;
            uint32_t mSlot;
            uint32_t mSize;
        };
        std::vector<UniformBufferDescriptor> uniformBuffers();
        void verify();

    private:
        OpenGLShaderLoader::HandleType mVertexShader;
        OpenGLShaderLoader::HandleType mPixelShader;
        GLuint mHandle = 0;
        std::vector<OpenGLBuffer> mUniformBuffers;
#if !OPENGL_ES
        std::vector<OpenGLBuffer> mShaderStorageBuffers;
#else
        std::vector<OpenGLSSBOBuffer> mShaderStorageBuffers;
        OpenGLBuffer mShaderStorageOffsetBuffer = GL_UNIFORM_BUFFER;
#endif
    };

}
}