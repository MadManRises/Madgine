#pragma once

#include "audioinfo.h"
#include "Modules/generic/bytebuffer.h"

namespace Engine {
namespace Audio {

    struct AudioBuffer {
        AudioInfo mInfo;
        ByteBuffer mBuffer;
    };

}
}