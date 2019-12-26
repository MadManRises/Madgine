#pragma once

#include "global.h"

#if ENABLE_THREADING

#include "workgroup.h"

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT WorkGroupStorage {
        template <typename T>
        struct container_type {
            container_type() = default;
            container_type(const container_type &) = delete;

            T *operator->()
            {
                return &mData[&WorkGroup::self()];
            }

            T &operator*()
            {
                return mData[&WorkGroup::self()];
            }

        private:
            std::map<WorkGroup *, T> mData;
        };

        static int registerLocalBssVariable(std::function<Any()> ctor);
        static void unregisterLocalBssVariable(int index);
        static int registerLocalObjectVariable(std::function<Any()> ctor);
        static void unregisterLocalObjectVariable(int index);

        static const Any &localVariable(int index);

        static void init(bool bss);
        static void finalize(bool bss);
    };

}
}

#else

#include "globalstorage.h"

namespace Engine {
	namespace Threading {

		using WorkGroupStorage = GlobalStorage;

	}
}

#endif

namespace Engine {
namespace Threading {

	
    template <typename T>
    using WorkgroupLocal = Global<T, WorkGroupStorage>;


}
}