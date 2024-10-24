#pragma once

#include "util/openglbuffer.h"
#include "util/opengltexture.h"

#include "Madgine/meshloader/gpumeshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshData : GPUMeshData {


        void reset()
        {
            mMaterials.clear();
            mVertices.reset();
            mIndices.reset();
        }

        OpenGLBuffer mVertices = GL_ARRAY_BUFFER;
        OpenGLBuffer mIndices = GL_ELEMENT_ARRAY_BUFFER;
    };

}
}