#pragma once

#include "Meta/serialize/hierarchy/serializabledataunit.h"

namespace Engine {
namespace Widgets {

    struct RenderData : Serialize::SerializableDataUnit {

        static bool animationInterval(std::chrono::steady_clock::duration dur, std::chrono::steady_clock::duration on);

    };

}
}