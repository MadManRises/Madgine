#pragma once

#if defined(Im3D_EXPORTS)
#    define MADGINE_IM3D_EXPORT DLL_EXPORT
#else
#    define MADGINE_IM3D_EXPORT DLL_IMPORT
#endif

#include "Modules/math/matrix4.h"
#include "Modules/math/ray.h"
#include "Modules/math/vector2i.h"
#include "Modules/render/vertex.h"

namespace Engine {

enum Im3DMeshType {
    IM3D_POINTS,
    IM3D_LINES,
    IM3D_TRIANGLES,
    IM3D_MESHTYPE_COUNT
};

namespace Render {
    struct MeshData;
}
typedef Render::MeshData *Im3DNativeMesh;

typedef uint32_t Im3DID;
typedef unsigned int Im3DTextureId;
typedef int Im3DBoundingObjectFlags;

enum Im3DBoundingObjectFlags_ {
    Im3DBoundingObjectFlags_ShowOutline = 1 << 0,
    Im3DBoundingObjectFlags_ShowOnHover = 1 << 1
};

struct Im3DFont {
    Im3DTextureId mTexture;
    Vector2i mTextureSize;

    Render::Glyph *mGlyphs;
};

struct Im3DIO {
    Ray mNextFrameMouseRay;

    Im3DFont (*mFetchFont)(const char *) = nullptr;
    void (*mReleaseFont)(Im3DFont &) = nullptr;
};

namespace Im3D {

    struct Im3DContext;
    struct Im3DObject;

    MADGINE_IM3D_EXPORT void CreateContext();
    MADGINE_IM3D_EXPORT void DestroyContext();
    MADGINE_IM3D_EXPORT Im3DContext *GetCurrentContext();

    MADGINE_IM3D_EXPORT Im3DIO &GetIO();

    MADGINE_IM3D_EXPORT void NewFrame();

    MADGINE_IM3D_EXPORT void PushID(const void *ptr);
    MADGINE_IM3D_EXPORT void PopID();

    MADGINE_IM3D_EXPORT void Mesh(Im3DMeshType type, const Render::Vertex *vertices, size_t vertexCount, const Matrix4 &transform = Matrix4::IDENTITY, const unsigned short *indices = nullptr, size_t indexCount = 0);
    MADGINE_IM3D_EXPORT void Mesh(Im3DMeshType type, Render::RenderPassFlags flags, const Render::Vertex2 *vertices, size_t vertexCount, const Matrix4 &transform = Matrix4::IDENTITY, const unsigned short *indices = nullptr, size_t indexCount = 0, Im3DTextureId texId = 0);

    MADGINE_IM3D_EXPORT void NativeMesh(Im3DNativeMesh mesh, const AABB &bb, const Matrix4 &transform = Matrix4::IDENTITY);

    MADGINE_IM3D_EXPORT void Text(const char *text, const Matrix4 &transform, float fontSize = 24, bool facingX = true, bool facingY = true, const char *fontName = "OpenSans-Regular", Vector2 pivot = { 0.5f, 0.5f });

    MADGINE_IM3D_EXPORT bool BoundingSphere(const char *name, Im3DBoundingObjectFlags flags = 0, size_t priority = 1);
    MADGINE_IM3D_EXPORT bool BoundingSphere(Im3DID id, Im3DBoundingObjectFlags flags = 0, size_t priority = 1);
    MADGINE_IM3D_EXPORT bool BoundingSphere(const char *name, const AABB &bb, const Matrix4 &transform = Matrix4::IDENTITY, Im3DBoundingObjectFlags flags = 0, size_t priority = 1);
    MADGINE_IM3D_EXPORT bool BoundingSphere(Im3DID id, const AABB &bb, const Matrix4 &transform = Matrix4::IDENTITY, Im3DBoundingObjectFlags flags = 0, size_t priority = 1);

    MADGINE_IM3D_EXPORT bool BoundingBox(const char *name, Im3DBoundingObjectFlags flags = 0, size_t priority = 1);
    MADGINE_IM3D_EXPORT bool BoundingBox(Im3DID id, Im3DBoundingObjectFlags flags = 0, size_t priority = 1);
    MADGINE_IM3D_EXPORT bool BoundingBox(const char *name, const AABB &bb, const Matrix4 &transform = Matrix4::IDENTITY, Im3DBoundingObjectFlags flags = 0, size_t priority = 1);
    MADGINE_IM3D_EXPORT bool BoundingBox(Im3DID id, const AABB &bb, const Matrix4 &transform = Matrix4::IDENTITY, Im3DBoundingObjectFlags flags = 0, size_t priority = 1);

    MADGINE_IM3D_EXPORT bool BoundingObject(Im3DID id, float distance = 0.0f, Im3DBoundingObjectFlags flags = 0, size_t priority = 1);

    MADGINE_IM3D_EXPORT bool IsObjectHovered();
    MADGINE_IM3D_EXPORT bool IsAnyObjectHovered();
}

}