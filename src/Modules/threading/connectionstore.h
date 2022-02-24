#pragma once

#include "Generic/linkednode.h"

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT ConnectionStore {    
        template <typename Con, typename... Args>
        std::weak_ptr<Con> emplace_front(Args &&... args)
        {
            std::shared_ptr<Con> ptr = std::make_shared<Con>(std::forward<Args>(args)...);
            mBegin.link(ptr);
            return ptr;
        }

        ConnectionStore();

        void clear();

        static ConnectionStore &globalStore();

    private:
        DoublyLinkedStartNode<std::shared_ptr<ConnectionBase>> mBegin;
    };

}
}
