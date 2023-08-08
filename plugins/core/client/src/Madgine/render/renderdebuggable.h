#pragma once

#include "Generic/lambda.h"

namespace Engine {
namespace Render {

    struct RenderDebuggable {
        virtual void debugFrustums(Lambda<void(const Frustum &, std::string_view)> handler) const { }
        virtual void debugCameras(Lambda<void(const Camera &, std::string_view)> handler) const { }
    };

}
}