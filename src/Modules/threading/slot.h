#pragma once

#include "../signalslot/slot.h"
#include "defaulttaskqueue.h"

namespace Engine {
namespace Threading {

	template <auto f>
    struct Slot : SignalSlot::Slot<f> {
    
		template <typename T>
            Slot(T *t, SignalSlot::TaskQueue *queue = nullptr)
                : SignalSlot::Slot<f>(t, queue ? queue : DefaultTaskQueue::getSingletonPtr())
            {
            }

	};

}
}