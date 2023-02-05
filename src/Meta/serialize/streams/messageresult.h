#pragma once

#include "Generic/enum.h"

namespace Engine {
namespace Serialize {

    ENUM(MessageResult,
        OK,
        REJECTED,
        DATA_CORRUPTION,
        SERVER_ERROR)

}
}