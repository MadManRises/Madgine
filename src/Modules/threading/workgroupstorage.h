#pragma once

namespace Engine {
namespace Threading {

	
	struct MODULES_EXPORT WorkGroupStorage {
        template <typename T>
        struct container_type {
            T *operator->()
            {
                return &mData[&self()];
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