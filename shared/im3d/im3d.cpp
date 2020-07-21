#include "Modules/moduleslib.h"

#include "im3d.h"

#include "Modules/threading/workgroup.h"

#include "render/vertex.h"

#include "im3d_internal.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "Modules/math/geometry3.h"

#include "Modules/math/boundingbox.h"

#include "glyph.h"

#include <numeric>

namespace Engine {

namespace Im3D {

    Threading::WorkgroupLocal<Im3DContext *> sContext;

    void CreateContext()
    {
        assert(sContext == nullptr);
        sContext = new Im3DContext;
    }

    void DestroyContext()
    {
        assert(sContext != nullptr);
        delete sContext;
        sContext = nullptr;
    }

    Im3DContext *GetCurrentContext()
    {
        return sContext;
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
        Im3DContext &c = *sContext;

        for (std::pair<const Im3DTextureId, Im3DContext::RenderData> &p : c.mRenderData) {
            for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
                p.second.mIndices[i].clear();
                p.second.mVertices[i].clear();
                p.second.mVertexBase[i] = 0;
                p.second.mIndices2[i].clear();
                p.second.mVertices2[i].clear();
                p.second.mVertexBase2[i] = 0;
            }
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
        Im3DContext &c = *sContext;
        auto it = c.mObjects.find(id);
        if (it == c.mObjects.end())
            return nullptr;
        return &it->second;
    }

    Im3DObject *FindObjectByName(const char *name)
    {
        ImGuiID id = ImHashStr(name);
        return FindObjectByID(id);
    }

    Im3DID GetID(const char *name)
    {
        Im3DContext &c = *sContext;
        Im3DID seed = c.mIDStack.empty() ? 0 : c.mIDStack.back();
        Im3DID id = ImHashStr(name, 0, seed);
        return id;
    }

    Im3DID GetID(const void *ptr)
    {
        Im3DContext &c = *sContext;
        ImGuiID seed = c.mIDStack.empty() ? 0 : c.mIDStack.back();
        return ImHashData(&ptr, sizeof(void *), seed);
    }

    void PushID(const void *ptr)
    {
        Im3DContext &c = *sContext;
        c.mIDStack.push_back(GetID(ptr));
    }

    void PopID()
    {
        Im3DContext &c = *sContext;
        c.mIDStack.pop_back();
    }

    Im3DObject *CreateObject(Im3DID id)
    {
        Im3DContext &c = *sContext;

        Im3DObject *o = &c.mObjects.try_emplace(id, id).first->second;

        return o;
    }

    static void ResetLastObject()
    {
        Im3DContext &c = *sContext;

        for (std::pair<const Im3DTextureId, Im3DContext::RenderData> &p : c.mRenderData) {
            for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
                p.second.mVertexBase[i] = p.second.mVertices[i].size();
            }
            for (size_t i = 0; i < IM3D_MESHTYPE_COUNT; ++i) {
                p.second.mVertexBase2[i] = p.second.mVertices2[i].size();
            }
        }
    }

    void Mesh(Im3DMeshType type, const Render::Vertex *vertices, size_t vertexCount, const Matrix4 &transform, const unsigned short *indices, size_t indexCount)
    {
        Im3DContext &c = *sContext;

        ResetLastObject();

        Vector3 minP { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
        Vector3 maxP { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

        std::transform(vertices, vertices + vertexCount, std::back_inserter(c.mRenderData[0].mVertices[type]), [&](const Render::Vertex &v) {
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
            std::transform(indices, indices + indexCount, std::back_inserter(c.mRenderData[0].mIndices[type]), [&](unsigned int i) { return i + c.mRenderData[0].mVertexBase[type]; });
        } else {
            assert(vertexCount % groupSize == 0);
            size_t oldIndexCount = c.mRenderData[0].mIndices[type].size();
            c.mRenderData[0].mIndices[type].resize(oldIndexCount + vertexCount);
            std::iota(c.mRenderData[0].mIndices[type].begin() + oldIndexCount, c.mRenderData[0].mIndices[type].end(), c.mRenderData[0].mVertexBase[type]);
        }
    }

    void Mesh(Im3DMeshType type, Render::RenderPassFlags flags, const Render::Vertex2 *vertices, size_t vertexCount, const Matrix4 &transform, const unsigned short *indices, size_t indexCount, Im3DTextureId texId)
    {
        Im3DContext &c = *sContext;

        ResetLastObject();

        Vector3 minP { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
        Vector3 maxP { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

        std::transform(vertices, vertices + vertexCount, std::back_inserter(c.mRenderData[texId].mVertices2[type]), [&](const Render::Vertex2 &v) {
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
            std::transform(indices, indices + indexCount, std::back_inserter(c.mRenderData[texId].mIndices2[type]), [&](unsigned int i) { return i + c.mRenderData[texId].mVertexBase2[type]; });
        } else {
            assert(vertexCount % groupSize == 0);
            size_t oldIndexCount = c.mRenderData[texId].mIndices2[type].size();
            c.mRenderData[texId].mIndices2[type].resize(oldIndexCount + vertexCount);
            std::iota(c.mRenderData[texId].mIndices2[type].begin() + oldIndexCount, c.mRenderData[texId].mIndices2[type].end(), c.mRenderData[texId].mVertexBase2[type]);
        }
        c.mRenderData[texId].mFlags = flags;
    }

    void NativeMesh(Im3DNativeMesh mesh, const AABB &bb, const Matrix4 &transform)
    {
        Im3DContext &c = *sContext;

        ResetLastObject();

        c.mNativeMeshes[mesh].push_back(transform);

        c.mTemp.mLastAABB = bb;
        c.mTemp.mLastTransform = transform;
    }

    void Text(const char *text, const Matrix4 &transform, float fontSize, bool facingX, bool facingY, const char *fontName, Vector2 pivot)
    {
        Im3DFont font = GetIO().mFetchFont(fontName);

        size_t textLen = strlen(text);

        if (textLen == 0)
            return;

        float scale = fontSize / 5000.0f;

        const float padding = 1.0f * scale;

        float fullWidth = padding * (textLen - 1);
        float minY = 0.0f;
        float maxY = 0.0f;

        for (size_t i = 0; i < textLen; ++i) {
            Render::Glyph &g = font.mGlyphs[static_cast<uint8_t>(text[i])];

            fullWidth += g.mSize.x * scale;
            maxY = max(maxY, g.mBearingY * scale);
            minY = min(minY, (g.mBearingY - g.mSize.y) * scale);
        }

        float fullHeight = maxY - minY;

        float xLeft = -fullWidth * pivot.x;
        float yTop = fullHeight * pivot.y;

        std::unique_ptr<Render::Vertex2[]> vertices = std::make_unique<Render::Vertex2[]>(4 * textLen);
        std::unique_ptr<unsigned short[]> indices = std::make_unique<unsigned short[]>(6 * textLen);

        float cursorX = xLeft;

        for (size_t i = 0; i < textLen; ++i) {
            Render::Glyph &g = font.mGlyphs[static_cast<uint8_t>(text[i])];

            float width = g.mSize.x * scale;
            float height = g.mSize.y * scale;

            float vPosX1 = cursorX;
            float vPosX2 = cursorX + width;
            float vPosY1 = yTop - fullHeight + g.mBearingY * scale;
            float vPosY2 = vPosY1 - height;

            Vector3 v11 = { 0, 0, 0 }, v12 = { 0, 0, 0 }, v21 = { 0, 0, 0 }, v22 = { 0, 0, 0 };
            Vector2 v211 = { 0, 0 }, v212 = { 0, 0 }, v221 = { 0, 0 }, v222 = { 0, 0 };

            if (facingX) {
                v211.x = vPosX1;
                v212.x = vPosX2;
                v221.x = vPosX1;
                v222.x = vPosX2;
            } else {
                v11.x = vPosX1;
                v12.x = vPosX2;
                v21.x = vPosX1;
                v22.x = vPosX2;
            }

            if (facingY) {
                v211.y = vPosY1;
                v212.y = vPosY1;
                v221.y = vPosY2;
                v222.y = vPosY2;
            } else {
                v11.y = vPosY1;
                v12.y = vPosY1;
                v21.y = vPosY2;
                v22.y = vPosY2;
            }

            int uvWidth = g.mSize.x;
            int uvHeight = g.mSize.y;

            if (g.mFlipped)
                std::swap(uvWidth, uvHeight);

            Vector2 uvTopLeft = { float(g.mUV.x) / font.mTextureSize.x, float(g.mUV.y) / font.mTextureSize.y };
            Vector2 uvBottomRight = { float(g.mUV.x + uvWidth) / font.mTextureSize.x,
                float(g.mUV.y + uvHeight) / font.mTextureSize.y };

            Vector2 uvTopRight = { uvBottomRight.x, uvTopLeft.y };
            Vector2 uvBottomLeft = { uvTopLeft.x, uvBottomRight.y };

            if (g.mFlipped)
                std::swap(uvTopRight, uvBottomLeft);

            vertices[4 * i] = { v11, v211, { 1, 1, 1, 1 }, uvTopLeft };
            vertices[4 * i + 1] = { v12, v212, { 1, 1, 1, 1 }, uvTopRight };
            vertices[4 * i + 2] = { v21, v221, { 1, 1, 1, 1 }, uvBottomLeft };
            vertices[4 * i + 3] = { v22, v222, { 1, 1, 1, 1 }, uvBottomRight };

            indices[6 * i] = 4 * i;
            indices[6 * i + 1] = 4 * i + 1;
            indices[6 * i + 2] = 4 * i + 2;
            indices[6 * i + 3] = 4 * i + 2;
            indices[6 * i + 4] = 4 * i + 1;
            indices[6 * i + 5] = 4 * i + 3;

            cursorX += g.mAdvance / 64.0f * scale;
        }

        Mesh(IM3D_TRIANGLES, Render::RenderPassFlags_NoLighting | Render::RenderPassFlags_DistanceField, vertices.get(), 4 * textLen, transform, indices.get(), 6 * textLen, *font.mTexture);

        if (GetIO().mReleaseFont)
            GetIO().mReleaseFont(font);
    }

    void Arrow(Im3DMeshType type, float radius, float length, const Matrix4 &transform, const Vector4 &color)
    {
        const Render::Vertex vertices[]
            = { { { 0, 0, 0 }, color, { 0, -1, 0 } },
                  { { radius, radius, -radius }, color, { 1, 0, -1 } },
                  { { radius, radius, radius }, color, { 1, 0, 1 } },
                  { { -radius, radius, radius }, color, { -1, 0, 1 } },
                  { { -radius, radius, -radius }, color, { -1, 0, -1 } },
                  { { 0, length, 0 }, color, { 0, 1, 0 } } };

        switch (type) {
        case IM3D_TRIANGLES: {
            constexpr unsigned short indices[] = {
                0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1, 1, 2, 5, 2, 3, 5, 3, 4, 5, 4, 1, 5
            };

            Im3D::Mesh(IM3D_TRIANGLES, vertices, 6, transform, indices, 24);
            break;
        }
        case IM3D_LINES: {
            constexpr unsigned short indices[] = {
                0, 1, 0, 2, 0, 3, 0, 4, 1, 5, 2, 5, 3, 5, 4, 5, 1, 2, 2, 3, 3, 4, 4, 1
            };

            Im3D::Mesh(IM3D_LINES, vertices, 6, transform, indices, 24);
            break;
        }
        default:
            std::terminate();
        }
    }

    bool BoundingSphere(const char *name, Im3DBoundingObjectFlags flags, size_t priority)
    {
        return BoundingSphere(GetID(name), flags, priority);
    }

    bool BoundingSphere(Im3DID id, Im3DBoundingObjectFlags flags, size_t priority)
    {
        Im3DContext &c = *sContext;
        return BoundingSphere(id, c.mTemp.mLastAABB, c.mTemp.mLastTransform, flags, priority);
    }

    bool BoundingSphere(const char *name, const AABB &bb, const Matrix4 &transform, Im3DBoundingObjectFlags flags, size_t priority)
    {
        return BoundingSphere(GetID(name), bb, transform, flags, priority);
    }

    bool BoundingSphere(Im3DID id, const AABB &bb, const Matrix4 &transform, Im3DBoundingObjectFlags flags, size_t priority)
    {
        Im3DContext &c = *sContext;

        Sphere bounds = { bb.center(),
            0.4f * bb.diameter() };

        //Check if Hovered
        float distance = 0.0f;
        if (auto intersection = Intersect(c.mMouseRay, transform * bounds))
            distance = intersection[0];

        return BoundingObject(id, distance, flags, priority);
    }

    bool BoundingBox(const char *name, Im3DBoundingObjectFlags flags, size_t priority)
    {
        return BoundingBox(GetID(name), flags, priority);
    }

    bool BoundingBox(Im3DID id, Im3DBoundingObjectFlags flags, size_t priority)
    {
        Im3DContext &c = *sContext;
        return BoundingBox(id, c.mTemp.mLastAABB, c.mTemp.mLastTransform, flags, priority);
    }

    bool BoundingBox(const char *name, const AABB &bb, const Matrix4 &transform, Im3DBoundingObjectFlags flags, size_t priority)
    {
        return BoundingBox(GetID(name), bb, transform, flags, priority);
    }

    bool BoundingBox(Im3DID id, const AABB &bb, const Matrix4 &transform, Im3DBoundingObjectFlags flags, size_t priority)
    {
        Im3DContext &c = *sContext;

        //Check if Hovered
        float distance = 0.0f;
        if (auto intersection = Intersect(c.mMouseRay, transform * bb))
            distance = intersection[0];

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
            unsigned short indices[] = {
                0, 1, 0, 2, 0, 4, 1, 3, 1, 5, 2, 3, 2, 6, 3, 7, 4, 5, 4, 6, 5, 7, 6, 7
            };
            Mesh(IM3D_LINES, vertices, 8, transform, indices, 24);
        }

        return hovered;
    }

    bool BoundingObject(Im3DID id, float distance, Im3DBoundingObjectFlags flags, size_t priority)
    {
        Im3DContext &c = *sContext;

        Im3DObject *object = CreateObject(id);

        if (distance > 0.0f && ((priority == c.mNextHoveredPriority && distance < c.mNextHoveredDistance) || priority > c.mNextHoveredPriority)) {
            c.mNextHoveredObject = object;
            c.mNextHoveredDistance = distance;
            c.mNextHoveredPriority = priority;
        }

        if (c.mHoveredObject == object) {
            for (std::pair<const Im3DTextureId, Im3DContext::RenderData> &p : c.mRenderData) {
                for (size_t t = 0; t < IM3D_MESHTYPE_COUNT; ++t) {
                    for (size_t i = p.second.mVertexBase[t]; i < p.second.mVertices[t].size(); ++i) {
                        p.second.mVertices[t][i].mColor *= 2.f;
                    }
                }
            }
        }

        c.mTemp.mLastObject = object;

        return object == c.mHoveredObject;
    }

    bool IsObjectHovered()
    {
        Im3DContext &c = *sContext;
        return c.mTemp.mLastObject && c.mTemp.mLastObject == c.mHoveredObject;
    }

    bool IsAnyObjectHovered()
    {
        Im3DContext &c = *sContext;
        return c.mHoveredObject || c.mNextHoveredObject;
    }

}

}