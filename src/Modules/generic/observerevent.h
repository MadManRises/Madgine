#pragma once

namespace Engine {

	enum ObserverEvent {
    INSERT_ITEM = 0x1,
    REMOVE_ITEM = 0x2,
    REMOVE_RANGE = 0x3,
    RESET = 0x4,
	ACTIVATE_ITEM = 0x5,
	DEACTIVATE_ITEM = 0x6,

    MASK = 0x0F,

    BEFORE = 0x00,
    AFTER = 0x10,
    ABORTED = 0x20,

    FULL_MASK = 0x3F
};

}