#pragma once

#include "Meta/math/vector4.h"
#include "Generic/compound.h"

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

    struct VertexBoneMappings {
        using ctor = std::pair<int[4], float[4]>;
        int mBoneIndices[4];
        float mBoneWeights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    };

    //

    using Vertex = Compound<
        VertexPos_3D,
        VertexColor,
        VertexNormal>;

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
        VertexColor,
        VertexNormal,
        VertexBoneMappings>;

}
}