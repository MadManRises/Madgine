#include "../apislib.h"

#include "im3d.h"

#include "Modules/threading/workgroup.h"

#include "Madgine/render/vertex.h"

#include "im3d_internal.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "Modules/math/geometry.h"

#include "Modules/math/boundingbox.h"

namespace Engine {

namespace Im3D {

    Threading::WorkgroupLocal<Im3DContext> sContext;

    Im3DContext *GetCurrentContext()
    {
        return &sContext;
    }

    Im3DIO &GetIO()
    {
        return sContext->mIO;
    }

    Im3DObject::Im3DObject(Im3DID id)
        : mID(id)
    {
    }

    void NewFrame()
    {
        Im3DContext &c = sContext;

        for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
            c.mIndices[i].clear();
            c.mVertices[i].clear();
            c.mVertexBase[i] = 0;
            c.mIndices2[i].clear();
            c.mVertices2[i].clear();
            c.mVertexBase2[i] = 0;
        }

        for (std::pair<const Im3DNativeMesh, std::vector<Matrix4>> &p : c.mNativeMeshes)
            p.second.clear();

        assert(c.mIDStack.empty());

        c.mHoveredObject = c.mNextHoveredObject;
        c.mNextHoveredObject = nullptr;
        c.mNextHoveredPriority = 0;
        c.mNextHoveredDistance = std::numeric_limits<float>::max();

        Im3DIO &io = c.mIO;
        c.mMouseRay = io.mNextFrameMouseRay;
        io.mNextFrameMouseRay = {};

        c.mTemp.mLastObject = nullptr;
        c.mTemp.mLastAABB = {};
        c.mTemp.mLastTransform = Matrix4::IDENTITY;
    }

    Im3DObject *FindObjectByID(Im3DID id)
    {
        Im3DContext &c = sContext;
        return (Im3DObject *)c.mObjectsById.GetVoidPtr(id);
    }

    Im3DObject *FindObjectByName(const char *name)
    {
        ImGuiID id = ImHashStr(name);
        return FindObjectByID(id);
    }

    Im3DID GetID(const char *name)
    {
        Im3DContext &c = sContext;
        Im3DID seed = c.mIDStack.empty() ? 0 : c.mIDStack.back();
        Im3DID id = ImHashStr(name, 0, seed);
        return id;
    }

    Im3DID GetID(const void *ptr)
    {
        Im3DContext &c = sContext;
        ImGuiID seed = c.mIDStack.empty() ? 0 : c.mIDStack.back();
        return ImHashData(&ptr, sizeof(void *), seed);
    }

    void PushID(const void *ptr)
    {
        Im3DContext &c = sContext;
        c.mIDStack.push_back(GetID(ptr));
    }

    void PopID()
    {
        Im3DContext &c = sContext;
        c.mIDStack.pop_back();
    }

    Im3DObject *CreateNewObject(Im3DID id)
    {
        Im3DContext &c = sContext;

        Im3DObject *o = c.mObjects.emplace_back(std::make_unique<Im3DObject>(id)).get();

        c.mObjectsById.SetVoidPtr(id, o);
        return o;
    }

    void Mesh(Im3DMeshType type, const Render::Vertex *vertices, size_t vertexCount, const Matrix4 &transform, const unsigned int *indices, size_t indexCount)
    {
        Im3DContext &c = sContext;

        for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
            c.mVertexBase[i] = c.mVertices[i].size();
        }
        for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
            c.mVertexBase2[i] = c.mVertices2[i].size();
        }

        Vector3 minP { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
        Vector3 maxP { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

        std::transform(vertices, vertices + vertexCount, std::back_inserter(c.mVertices[type]), [&](const Render::Vertex &v) {
            Render::Vertex result = v;

            minP = min(v.mPos, minP);
            maxP = max(v.mPos, maxP);

            result.mPos = (transform * Vector4 { v.mPos, 1.0f }).xyz();

            /*if (c.mHoveredObject == object)
                result.mColor *= 2.f;*/

            return result;
        });

        c.mTemp.mLastAABB = { minP, maxP };
        c.mTemp.mLastTransform = transform;

        size_t groupSize = (size_t)type + 1;

        if (indices) {
            assert(indexCount % groupSize == 0);
            std::transform(indices, indices + indexCount, std::back_inserter(c.mIndices[type]), [&](unsigned int i) { return i + c.mVertexBase[type]; });
        } else {
            assert(vertexCount % groupSize == 0);
            size_t oldIndexCount = c.mIndices[type].size();
            c.mIndices[type].resize(oldIndexCount + vertexCount);
            std::iota(c.mIndices[type].begin() + oldIndexCount, c.mIndices[type].end(), c.mVertexBase[type]);
        }
    }

	void Mesh(Im3DMeshType type, const Render::Vertex2 *vertices, size_t vertexCount, const Matrix4 &transform, const unsigned int *indices, size_t indexCount)
    {
        Im3DContext &c = sContext;

        for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
            c.mVertexBase[i] = c.mVertices[i].size();
        }
        for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
            c.mVertexBase2[i] = c.mVertices2[i].size();
        }

        Vector3 minP { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
        Vector3 maxP { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

        std::transform(vertices, vertices + vertexCount, std::back_inserter(c.mVertices2[type]), [&](const Render::Vertex2 &v) {
            Render::Vertex2 result = v;

            minP = min(v.mPos, minP);
            maxP = max(v.mPos, maxP);

            result.mPos = (transform * Vector4 { v.mPos, 1.0f }).xyz();

            /*if (c.mHoveredObject == object)
                result.mColor *= 2.f;*/

            return result;
        });

        c.mTemp.mLastAABB = { minP, maxP };
        c.mTemp.mLastTransform = transform;

        size_t groupSize = (size_t)type + 1;

        if (indices) {
            assert(indexCount % groupSize == 0);
            std::transform(indices, indices + indexCount, std::back_inserter(c.mIndices2[type]), [&](unsigned int i) { return i + c.mVertexBase2[type]; });
        } else {
            assert(vertexCount % groupSize == 0);
            size_t oldIndexCount = c.mIndices2[type].size();
            c.mIndices2[type].resize(oldIndexCount + vertexCount);
            std::iota(c.mIndices2[type].begin() + oldIndexCount, c.mIndices2[type].end(), c.mVertexBase2[type]);
        }
    }

    void NativeMesh(Im3DNativeMesh mesh, const AABB &bb, const Matrix4 &transform)
    {
        Im3DContext &c = sContext;

        for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
            c.mVertexBase[i] = c.mVertices[i].size();
        }

        c.mNativeMeshes[mesh].push_back(transform);

        c.mTemp.mLastAABB = bb;
        c.mTemp.mLastTransform = transform;
    }

    bool BoundingSphere(const char *name, Im3DBoundingObjectFlags flags, size_t priority)
    {
        return BoundingSphere(GetID(name), flags, priority);
    }

    bool BoundingSphere(Im3DID id, Im3DBoundingObjectFlags flags, size_t priority)
    {
        Im3DContext &c = sContext;
        return BoundingSphere(id, c.mTemp.mLastAABB, c.mTemp.mLastTransform, flags, priority);
    }

    bool BoundingSphere(const char *name, const AABB &bb, const Matrix4 &transform, Im3DBoundingObjectFlags flags, size_t priority)
    {
        return BoundingSphere(GetID(name), bb, transform, flags, priority);
    }

    bool BoundingSphere(Im3DID id, const AABB &bb, const Matrix4 &transform, Im3DBoundingObjectFlags flags, size_t priority)
    {
        Im3DContext &c = sContext;

        Sphere bounds = { bb.center(),
            0.4f * bb.diameter() };

        //Check if Hovered
        float distance = 0.0f;
        Intersect(c.mMouseRay, transform * bounds, &distance);

        return BoundingObject(id, distance, flags, priority);
    }

    bool BoundingBox(const char *name, Im3DBoundingObjectFlags flags, size_t priority)
    {
        return BoundingBox(GetID(name), flags, priority);
    }

    bool BoundingBox(Im3DID id, Im3DBoundingObjectFlags flags, size_t priority)
    {
        Im3DContext &c = sContext;
        return BoundingBox(id, c.mTemp.mLastAABB, c.mTemp.mLastTransform, flags, priority);
    }

    bool BoundingBox(const char *name, const AABB &bb, const Matrix4 &transform, Im3DBoundingObjectFlags flags, size_t priority)
    {
        return BoundingBox(GetID(name), bb, transform, flags, priority);
    }

    bool BoundingBox(Im3DID id, const AABB &bb, const Matrix4 &transform, Im3DBoundingObjectFlags flags, size_t priority)
    {
        Im3DContext &c = sContext;

        //Check if Hovered
        float distance = 0.0f;
        Intersect(c.mMouseRay, transform * bb, &distance);

        bool hovered = BoundingObject(id, distance, flags, priority);

        if ((flags & Im3DBoundingObjectFlags_ShowOutline) || (hovered && (flags & Im3DBoundingObjectFlags_ShowOnHover))) {
            std::array<Vector3, 8> corners = bb.corners();
            Render::Vertex vertices[] = {
                { corners[0], { 1, 1, 1, 1 }, { 0, 0, 0 } },
                { corners[1], { 1, 1, 1, 1 }, { 0, 0, 0 } },
                { corners[2], { 1, 1, 1, 1 }, { 0, 0, 0 } },
                { corners[3], { 1, 1, 1, 1 }, { 0, 0, 0 } },
                { corners[4], { 1, 1, 1, 1 }, { 0, 0, 0 } },
                { corners[5], { 1, 1, 1, 1 }, { 0, 0, 0 } },
                { corners[6], { 1, 1, 1, 1 }, { 0, 0, 0 } },
                { corners[7], { 1, 1, 1, 1 }, { 0, 0, 0 } },
            };
            unsigned int indices[] = {
                0, 1, 0, 2, 0, 4, 1, 3, 1, 5, 2, 3, 2, 6, 3, 7, 4, 5, 4, 6, 5, 7, 6, 7
            };
            Mesh(IM3D_LINES, vertices, 8, transform, indices, 24);
        }

        return hovered;
    }

    bool BoundingObject(Im3DID id, float distance, Im3DBoundingObjectFlags flags, size_t priority)
    {
        Im3DContext &c = sContext;

        Im3DObject *object = FindObjectByID(id);
        const bool object_just_created = (object == nullptr);
        if (object_just_created) {
            object = CreateNewObject(id);
        }

        if (distance > 0.0f && ((priority == c.mNextHoveredPriority && distance < c.mNextHoveredDistance) || priority > c.mNextHoveredPriority)) {
            c.mNextHoveredObject = object;
            c.mNextHoveredDistance = distance;
            c.mNextHoveredPriority = priority;
        }

        if (c.mHoveredObject == object) {
            for (size_t t = 0; t < IM3D_MESHTYPE_COUNT; ++t) {
                for (size_t i = c.mVertexBase[t]; i < c.mVertices[t].size(); ++i) {
                    c.mVertices[t][i].mColor *= 2.f;
                }
            }
        }

        c.mTemp.mLastObject = object;

        return object == c.mHoveredObject;
    }

    bool IsObjectHovered()
    {
        Im3DContext &c = sContext;
        return c.mTemp.mLastObject && c.mTemp.mLastObject == c.mHoveredObject;
    }

    bool IsAnyObjectHovered()
    {
        Im3DContext &c = sContext;
        return c.mHoveredObject || c.mNextHoveredObject;
    }

}

}