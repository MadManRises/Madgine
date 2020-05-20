#pragma once

#include "../genericresult.h"

namespace Engine {

ENUM_BASE(StreamResult, GenericResult,
    WOULD_BLOCK,
    TIMEOUT,
    CONNECTION_REFUSED
);

}