#pragma once

#include "Modules/math/matrix4.h"

namespace Engine {
namespace Apis {

    namespace Im3D {

        MADGINE_APIS_EXPORT Im3DContext *GetCurrentContext();

        MADGINE_APIS_EXPORT void NewFrame();

        MADGINE_APIS_EXPORT void Mesh(const Render::Vertex *vertices, size_t vertexCount, const Matrix4 &transform = Matrix4::IDENTITY, const unsigned int *indices = nullptr, size_t indexCount = 0);
    }

}
}