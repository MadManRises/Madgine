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
            mMaterials.clear();
            mVertices.reset();
            mIndices.reset();
            mVAO.reset();
        }

        OpenGLBuffer mVertices = GL_ARRAY_BUFFER;
        OpenGLBuffer mIndices = GL_ELEMENT_ARRAY_BUFFER;
        size_t mGroupSize;
        size_t mElementCount;
        OpenGLVertexArray mVAO;
    };

}
}