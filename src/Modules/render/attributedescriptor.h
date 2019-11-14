#pragma once

#include "Modules/math/vector4.h"

namespace Engine {
namespace Render {

	enum AttributeType {
        ATTRIBUTE_FLOAT
    };

    struct AttributeDescriptor {
        template <typename T, typename M, typename BaseType = T>
        AttributeDescriptor(M T::*m, type_holder_t<BaseType> = {})
            : mStride(sizeof(BaseType))
            , mOffset(reinterpret_cast<size_t>(&(reinterpret_cast<BaseType *>(0x1)->*m))-1)
        {
            if constexpr (std::is_same_v<M, Vector2>) {
                mType = ATTRIBUTE_FLOAT;
                mArraySize = 2;
            } else if constexpr (std::is_same_v<M, Vector3>) {
                mType = ATTRIBUTE_FLOAT;
                mArraySize = 3;
            } else if constexpr (std::is_same_v<M, Vector4>) {
                mType = ATTRIBUTE_FLOAT;
                mArraySize = 4;
            } else {
                static_assert(dependent_bool<M, false>::value, "Unsupported Attribute type!");
            }
        }

        size_t mArraySize;
        AttributeType mType;
        size_t mStride;
        size_t mOffset;
    };

}
}