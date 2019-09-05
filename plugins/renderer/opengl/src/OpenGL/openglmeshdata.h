#pragma once

#include "Modules/math/boundingbox.h"
#include "util/openglbuffer.h"
#include "util/openglvertexarray.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLMeshData {
        OpenGLBuffer mVertices;
        OpenGLBuffer mIndices = dont_create_buffer;
        size_t mGroupSize;
        size_t mElementCount;
        OpenGLVertexArray mVAO;
		AABB mAABB;
    };

}
}