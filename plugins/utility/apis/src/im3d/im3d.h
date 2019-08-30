#pragma once

#include "Modules/math/matrix4.h"
#include "Modules/math/ray.h"

namespace Engine {

struct Im3DIO {
    Ray mNextFrameMouseRay;
};

namespace Im3D {

    struct Im3DObject;

    MADGINE_APIS_EXPORT Im3DContext *GetCurrentContext();

    MADGINE_APIS_EXPORT Im3DIO &GetIO();

    MADGINE_APIS_EXPORT void NewFrame();

    MADGINE_APIS_EXPORT void Mesh(const char *name, const Render::Vertex *vertices, size_t vertexCount, const Matrix4 &transform = Matrix4::IDENTITY, const unsigned int *indices = nullptr, size_t indexCount = 0);

    MADGINE_APIS_EXPORT bool IsObjectHovered();
}

}