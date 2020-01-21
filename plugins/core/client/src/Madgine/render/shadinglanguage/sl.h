#pragma once

namespace Engine {
namespace Render {
    namespace ShadingLanguage {

        constexpr size_t align_helper(size_t in) {
            if (in > 8)
                return 16;
            return in;
        }

        template <typename T, typename Name>
        struct alignas(align_helper(sizeof(T))) Named {
            T mValue;

            operator T& () {
                return mValue;
            }

            Named<T, Name>& operator=(const T& v) {
                mValue = v;
                return *this;
            }
        };

    }
}
}

#define INCLUDE_SL_SHADER "Madgine/render/shadinglanguage/sl_include.h"
