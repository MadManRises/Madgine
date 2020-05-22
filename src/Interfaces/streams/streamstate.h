#pragma once

#include "../enum.h"

namespace Engine {

ENUM(StreamState,
    OK,
    UNKNOWN_ERROR,
    CLOSED_BY_USER,
    SEND_FAILURE,
    RECEIVE_FAILURE);

}