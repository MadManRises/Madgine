#pragma once

#include "program.h"

#include "openglbuffer.h"

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

    private:
        GLuint mHandle = 0;
        std::vector<OpenGLBuffer> mUniformBuffers;
    };

}
}