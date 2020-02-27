#pragma once

#include "program.h"

#include "openglbuffer.h"

#include "openglssbobuffer.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLProgram : Program {

        OpenGLProgram() = default;
        OpenGLProgram(OpenGLProgram &&other);
        ~OpenGLProgram();

        OpenGLProgram &operator=(OpenGLProgram &&other);

        bool link(OpenGLShader *vertexShader, OpenGLShader *pixelShader);

        void reset();

        void bind();

        void setParameters(const void *data, size_t size, size_t index);
        void setDynamicParameters(const void *data, size_t size, size_t index);

        struct UniformBufferDescriptor : ScopeBase {
            size_t mSlot;
            std::string mName;
            size_t mSize;
        };
        std::vector<UniformBufferDescriptor> uniformBuffers();
        void verify();

    private:
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