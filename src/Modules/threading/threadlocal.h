#pragma once

#include "../generic/any.h"
#include "../generic/tupleunpacker.h"

namespace Engine {
namespace Threading {

#if ENABLE_THREADING

    struct MODULES_EXPORT ThreadLocalStorage {

        static int registerLocalBssVariable(std::function<Any()> ctor);
        static void unregisterLocalBssVariable(int index);
        static int registerLocalObjectVariable(std::function<Any()> ctor);
        static void unregisterLocalObjectVariable(int index);

        static const Any &localVariable(int index);

        static void init(bool bss);
        static void finalize(bool bss);
    };

    template <typename T, typename G = ThreadLocalStorage>
    struct ThreadLocal {

        template <typename... Args>
        ThreadLocal(Args &&... args)
        {
            std::tuple<Args...> tuple {
                std::forward<Args>(args)...
            };
            auto lambda = [=]() { return TupleUnpacker::constructExpand<Any>(Any::inplace<T>, tuple); };

            if constexpr (std::is_trivially_copyable_v<T>) {
                mIndex = G::registerLocalBssVariable(lambda);
            } else {
                mIndex = G::registerLocalObjectVariable(lambda);
            }
        }

        ~ThreadLocal()
        {
            if constexpr (std::is_trivially_copyable_v<T>) {
                G::unregisterLocalBssVariable(mIndex);
            } else {
                G::unregisterLocalObjectVariable(mIndex);
            }
        }

        T *operator->() { return &data(); }

        T &operator*() { return data(); }

        operator T &() { return data(); }

        T *operator&() { return &data(); }

        void operator=(const T &t) { data() = t; }

        T &data() { return G::localVariable(mIndex).template as<T>(); }

    private:
        int mIndex;
    };

    template <typename T, typename G>
    struct ThreadLocal<T *, G> {

        ThreadLocal(T *initial = nullptr)
        {
            mIndex = G::registerLocalBssVariable([=]() { return Any { initial }; });
        }

        ~ThreadLocal()
        {
            G::unregisterLocalBssVariable(mIndex);
		}

        T *operator->() { return data(); }

        T &operator*() { return *data(); }

        operator T *() { return data(); }

        T *operator&() { return data(); }

        void operator=(T *t) { data() = t; }

        T *&data() { return G::localVariable(mIndex).template as<T *>(); }

    private:
        int mIndex;
    };

#endif

    template <typename T>
    struct Proxy {

        template <typename... Args>
        Proxy(Args... args)
            : mData(std::forward<Args>(args)...)
        {
        }

        T *operator->() { return &mData; }

        T &operator*() { return mData; }

        T *operator&() { return &mData; }

        operator T &() { return mData; }

        void operator=(const T &t) { mData = t; }

    private:
        T mData;
    };

    template <typename T>
    struct Proxy<T *> {

        Proxy(T *t = nullptr)
            : mPtr(t)
        {
        }

        T *operator->() { return mPtr; }

        T &operator*() { return *mPtr; }

        T *operator&() { return mPtr; }

        operator T *() { return mPtr; }

        void operator=(T *t) { mPtr = t; }

    private:
        T *mPtr;
    };

} // namespace Threading
} // namespace Engine

#if ENABLE_THREADING
#if USE_CUSTOM_THREADLOCAL
#define THREADLOCAL(T) ::Engine::Threading::ThreadLocal<T>
#else
#define THREADLOCAL(T) thread_local ::Engine::Threading::Proxy<T>
#endif
#else
#define THREADLOCAL(T) ::Engine::Threading::Proxy<T>
#endif

#if EMSCRIPTEN
#define thread_local thread_local_not_supported_on_emscripten provoke_syntax_error
#endif
