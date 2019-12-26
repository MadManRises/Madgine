#pragma once

#if ENABLE_THREADING

namespace Engine {
namespace Threading {

    struct MODULES_EXPORT ThreadStorage {

        template <typename T>
        struct container_type {
            T *operator->()
            {
                return &mData[std::this_thread::get_id()];
            }

            T &operator*()
            {
                return mData[std::this_thread::get_id()];
            }

        private:
            std::map<std::thread::id, T> mData;
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

#endif