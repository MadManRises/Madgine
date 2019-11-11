#pragma once

#include "util/openglbuffer.h"
#include "util/openglvertexarray.h"
#include "util/opengltexture.h"

#include "meshdata.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshData : Resources::MeshData {
        OpenGLTexture mTexture = dont_create;
        OpenGLBuffer mVertices;
        OpenGLBuffer mIndices = dont_create;
        size_t mGroupSize;
        size_t mElementCount;
        OpenGLVertexArray mVAO;
    };

}
}