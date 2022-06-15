#pragma once

#include "Generic/compound.h"
#include "Meta/math/vector4.h"

namespace Engine {
namespace Render {

    struct VertexPos_3D {
        using ctor = Vector3;
        Vector3 mPos;
    };

    struct VertexPos_4D {
        using ctor = Vector4;
        Vector4 mPos;
    };

    struct VertexPos2 {
        using ctor = Vector2;
        Vector2 mPos2;
    };

    struct VertexNormal {
        using ctor = Vector3;
        Vector3 mNormal;
    };

    struct VertexColor {
        using ctor = Vector4;
        Vector4 mColor;
    };

    struct VertexUV {
        using ctor = Vector2;
        Vector2 mUV;
    };

    struct VertexBoneIndices {
        using ctor = int[4];
        int mBoneIndices[4];
    };

    struct VertexBoneWeights {
        using ctor = float[4];
        float mBoneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    };

    using VertexElements = type_pack<
        VertexPos_3D,
        VertexPos_4D,
        VertexPos2,
        VertexNormal,
        VertexColor,
        VertexUV,
        VertexBoneIndices,
        VertexBoneWeights>;

    static constexpr size_t sVertexElementSizes[] = {
        sizeof(VertexPos_3D),
        sizeof(VertexPos_4D),
        sizeof(VertexPos2),
        sizeof(VertexNormal),
        sizeof(VertexColor),
        sizeof(VertexUV),
        sizeof(VertexBoneIndices),
        sizeof(VertexBoneWeights)
    };

    using Vertex = Compound<
        VertexPos_3D,
        VertexNormal,
        VertexColor>;

    using Vertex2 = Compound<
        VertexPos_3D,
        VertexPos2,
        VertexColor,
        VertexUV>;

    using Vertex3 = Compound<
        VertexPos_3D,
        VertexNormal,
        VertexUV>;

    using Vertex4 = Compound<
        VertexPos_3D,
        VertexNormal,
        VertexColor,
        VertexBoneIndices,
        VertexBoneWeights>;

    using Vertex5 = Compound<
        VertexPos_3D,
        VertexUV,
        VertexNormal,
        VertexBoneIndices,
        VertexBoneWeights>;

}
}