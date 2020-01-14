#pragma once

#include "../threading/taskqueue.h"

namespace Engine {
namespace Threading {

	struct MODULES_EXPORT DefaultTaskQueue : Threading::TaskQueue {
    public:
        DefaultTaskQueue();
        DefaultTaskQueue(const DefaultTaskQueue &) = delete;
        ~DefaultTaskQueue();

        DefaultTaskQueue &operator=(const DefaultTaskQueue &) = delete;

        static DefaultTaskQueue &getSingleton();
        static DefaultTaskQueue *getSingletonPtr();
    };

}
}