#pragma once

#include "util/openglbuffer.h"
#include "util/opengltexture.h"
#include "util/openglvertexarray.h"

#include "gpumeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshData : GPUMeshData {

        void reset()
        {
            mTexture.reset();
            mVertices.reset();
            mIndices.reset();
            mVAO.reset();
        }

        OpenGLTexture mTexture;
        OpenGLBuffer mVertices;
        OpenGLBuffer mIndices;
        size_t mGroupSize;
        size_t mElementCount;
        OpenGLVertexArray mVAO;
    };

}
}