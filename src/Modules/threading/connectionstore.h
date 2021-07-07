#pragma once

#include "Generic/linkednode.h"

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT ConnectionStore {    
        template <typename Con, typename... Args>
        static std::shared_ptr<Con> create(Args &&... args)
        {

            return std::static_pointer_cast<Con>(
                make_shared_connection(
                    std::make_unique<Con>(std::forward<Args>(args)...)));
        }

        template <typename Con, typename... Args>
        std::weak_ptr<Con> emplace_front(Args &&... args)
        {
            std::shared_ptr<Con> ptr = create<Con>(std::forward<Args>(args)...);
            mBegin.link(ptr);
            return ptr;
        }

        ConnectionStore();

        void clear();

        static ConnectionStore &globalStore();

    private:
        static std::shared_ptr<ConnectionBase> make_shared_connection(std::unique_ptr<ConnectionBase> &&conn);

        DoublyLinkedStartNode<std::shared_ptr<ConnectionBase>> mBegin;
    };

}
}
