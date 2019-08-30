#include "../apislib.h"

#include "im3d.h"

#include "Modules/threading/workgroup.h"

#include "Madgine/render/vertex.h"

#include "im3d_internal.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "Modules/math/geometry.h"

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

        Im3DObject::Im3DObject(const char *name)
            : mName(name)
            , mID(ImHashStr(name))
        {
        }

        void NewFrame()
        {
            Im3DContext &c = sContext;

            c.mTriangleIndices.clear();
            c.mTriangleVertices.clear();

            c.mHoveredObject = c.mNextHoveredObject;
            c.mNextHoveredObject = nullptr;
            c.mNextHoveredDistance = std::numeric_limits<float>::max();

            Im3DIO &io = c.mIO;
            c.mMouseRay = io.mNextFrameMouseRay;
            io.mNextFrameMouseRay = {};

			c.mTemp.mLastObject = nullptr;
        }

        Im3DObject *FindObjectByID(ImGuiID id)
        {
            Im3DContext &c = sContext;
            return (Im3DObject *)c.mObjectsById.GetVoidPtr(id);
        }

        Im3DObject *FindObjectByName(const char *name)
        {
            ImGuiID id = ImHashStr(name);
            return FindObjectByID(id);
        }

        Im3DObject *CreateNewObject(const char *name)
        {
            Im3DContext &c = sContext;

            Im3DObject *o = c.mObjects.emplace_back(std::make_unique<Im3DObject>(name)).get();

            ImGuiID id = ImHashStr(name);
            c.mObjectsById.SetVoidPtr(id, o);
            return o;
        }

        void Mesh(const char *name, const Render::Vertex *vertices, size_t vertexCount, const Matrix4 &transform, const unsigned int *indices, size_t indexCount)
        {
            Im3DContext &c = sContext;

            Im3DObject *object = FindObjectByName(name);
            const bool object_just_created = (object == NULL);
            if (object_just_created) {
                object = CreateNewObject(name);
            }

            object->mTransform = transform;
            object->mInverseTransform = transform.Inverse();

            unsigned int indexBase = c.mTriangleVertices.size();

            std::transform(vertices, vertices + vertexCount, std::back_inserter(c.mTriangleVertices), [&](const Render::Vertex &v) {
                Render::Vertex result = v;
                result.mPos = (transform * Vector4 { v.mPos, 1.0f }).xyz();

                if (c.mHoveredObject == object)
                    result.mColor *= 2.f;

                return result;
            });

            if (indices) {
                assert(indexCount % 3 == 0);
                std::transform(indices, indices + indexCount, std::back_inserter(c.mTriangleIndices), [&](unsigned int i) { return i + indexBase; });
            } else {
                assert(vertexCount % 3 == 0);
                size_t oldIndexCount = c.mTriangleIndices.size();
                c.mTriangleIndices.resize(oldIndexCount + vertexCount);
                std::iota(c.mTriangleIndices.begin() + oldIndexCount, c.mTriangleIndices.end(), indexBase);
            }

            //Update Bounds
            if (object_just_created ||true) {
                Vector3 min { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
                Vector3 max { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };
                for (size_t index = indexBase; index < indexBase + vertexCount; ++index) {
                    Vector3 &v = c.mTriangleVertices[index].mPos;
                    min.x = std::min(v.x, min.x);
                    min.y = std::min(v.y, min.y);
                    min.z = std::min(v.z, min.z);
                    max.x = std::max(v.x, max.x);
                    max.y = std::max(v.y, max.y);
                    max.z = std::max(v.z, max.z);
                }

                object->mBounds = { 0.5f * (min + max),
                    0.4f * (max - min).length() };
            } else {
                object->mBounds = transform * (object->mInverseTransform * object->mBounds);
            }

            object->mTransform = transform;
            object->mInverseTransform = transform.Inverse();

            //Check if Hovered
            float rayParam;
            if (!c.mMouseRay.mDir.isZeroLength()) {
                if (Intersect(c.mMouseRay, object->mBounds, &rayParam)) {
                    if (rayParam > 0.0f && rayParam < c.mNextHoveredDistance) {
                        c.mNextHoveredObject = object;
                        c.mNextHoveredDistance = rayParam;
                    }
                }
            }

			c.mTemp.mLastObject = object;
        }

		MADGINE_APIS_EXPORT bool IsObjectHovered()
        {
            Im3DContext &c = sContext;
            return c.mTemp.mLastObject && c.mTemp.mLastObject == c.mHoveredObject;
        }

    }

}