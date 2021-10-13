#pragma once

namespace Engine {
namespace Render {
    namespace ShadingLanguage {

        constexpr size_t align_helper(size_t in) {
            if (in > 8)
                return 16;
            return in;
        }

    }
}
}

#define INCLUDE_SL_SHADER "Madgine/render/shadinglanguage/sl_include.h"
