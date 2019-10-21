#pragma once

#include "Modules/math/boundingbox.h"
#include "Modules/math/sphere.h"

#include "Modules/render/renderpassflags.h"

namespace Engine {
namespace Im3D {

    constexpr size_t MAX_PRIORITY_COUNT = 3;

    struct Im3DObject {
        Im3DObject(Im3DID id);

        std::string mName;
        Im3DID mID;
    };

    struct Im3DObjectTempData {
        Im3DObject *mLastObject;
        AABB mLastAABB;
        Matrix4 mLastTransform;
    };

    struct Im3DContext {

        Im3DIO mIO;

        struct RenderData {
            std::vector<Render::Vertex> mVertices[IM3D_MESHTYPE_COUNT];
            std::vector<unsigned short> mIndices[IM3D_MESHTYPE_COUNT];
            size_t mVertexBase[IM3D_MESHTYPE_COUNT];
            std::vector<Render::Vertex2> mVertices2[IM3D_MESHTYPE_COUNT];
            std::vector<unsigned short> mIndices2[IM3D_MESHTYPE_COUNT];
            size_t mVertexBase2[IM3D_MESHTYPE_COUNT];
            Render::RenderPassFlags mFlags = Render::RenderPassFlags_NoLighting;
        };
        std::map<Im3DTextureId, RenderData> mRenderData;

        std::map<Im3DNativeMesh, std::vector<Matrix4>> mNativeMeshes;

        std::map<Im3DID, Im3DObject> mObjects;
        std::vector<Im3DID> mIDStack;

        Ray mMouseRay;
        Im3DObject *mHoveredObject;
        Im3DObject *mNextHoveredObject;
        float mNextHoveredDistance;
        size_t mNextHoveredPriority;

        Im3DObjectTempData mTemp;
    };

    MADGINE_IM3D_EXPORT Im3DObject *FindObjectByID(Im3DID id);

    MADGINE_IM3D_EXPORT Im3DObject *FindObjectByName(const char *name);

    MADGINE_IM3D_EXPORT Im3DID GetID(const char *name);
    MADGINE_IM3D_EXPORT Im3DID GetID(const void *ptr);
    
}
}