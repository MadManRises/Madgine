#pragma once

#include "Modules/math/matrix4.h"
#include "Modules/math/ray.h"

namespace Engine {

struct Im3DIO {
    Ray mNextFrameMouseRay;
};

enum Im3DMeshType {
    IM3D_POINTS,
    IM3D_LINES,
    IM3D_TRIANGLES,
    IM3D_MESHTYPE_COUNT
};

namespace Im3D {

    struct Im3DObject;

    typedef void *Im3DNativeMesh;

    typedef unsigned int Im3DID;

    MADGINE_APIS_EXPORT Im3DContext *GetCurrentContext();

    MADGINE_APIS_EXPORT Im3DIO &GetIO();

    MADGINE_APIS_EXPORT void NewFrame();

    MADGINE_APIS_EXPORT void Mesh(Im3DMeshType type, const Render::Vertex *vertices, size_t vertexCount, const Matrix4 &transform = Matrix4::IDENTITY, const unsigned int *indices = nullptr, size_t indexCount = 0);

    MADGINE_APIS_EXPORT void NativeMesh(Im3DNativeMesh mesh, const AABB &bb, const Matrix4 &transform = Matrix4::IDENTITY);

    MADGINE_APIS_EXPORT void BoundingSphere(const char *name, size_t priority = 1);
    MADGINE_APIS_EXPORT void BoundingSphere(Im3DID id, size_t priority = 1);
    MADGINE_APIS_EXPORT void BoundingSphere(const char *name, const AABB &bb, const Matrix4 &transform = Matrix4::IDENTITY, size_t priority = 1);
    MADGINE_APIS_EXPORT void BoundingSphere(Im3DID id, const AABB &bb, const Matrix4 &transform = Matrix4::IDENTITY, size_t priority = 1);

    MADGINE_APIS_EXPORT void BoundingObject(Im3DID id, float distance = 0.0f, size_t priority = 1);

    MADGINE_APIS_EXPORT bool IsObjectHovered();
}

}