#pragma once

#include "Modules/math/boundingbox.h"
#include "util/openglbuffer.h"
#include "util/openglvertexarray.h"
#include "util/opengltexture.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshData {
        OpenGLTexture mTexture = dont_create;
        OpenGLBuffer mVertices;
        OpenGLBuffer mIndices = dont_create;
        size_t mGroupSize;
        size_t mElementCount;
        OpenGLVertexArray mVAO;
        RenderPassFlags mFlags;
		AABB mAABB;
    };

}
}