#pragma once

#include "Interfaces/util/enum.h"

namespace Engine {
namespace Serialize {

    ENUM(StreamState,
        OK,
        UNKNOWN_ERROR,
        CLOSED_BY_USER,
        SEND_FAILURE,
        RECEIVE_FAILURE);

}
}