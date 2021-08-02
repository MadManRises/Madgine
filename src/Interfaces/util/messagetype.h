#pragma once

#include "Generic/enum.h"

namespace Engine {
namespace Util {

    ENUM(MessageType,
        DEBUG_TYPE,
        LOG_TYPE,
        WARNING_TYPE,
        ERROR_TYPE
    );

}
}
