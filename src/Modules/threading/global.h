#pragma once

#include "../generic/any.h"
#include "../generic/tupleunpacker.h"

namespace Engine {
namespace Threading {

    template <typename T, typename Storage>
    struct Global {

        template <typename... Args>
        Global(Args &&... args)
        {
            std::tuple<Args...> tuple {
                std::forward<Args>(args)...
            };
            auto lambda = [=]() { return TupleUnpacker::constructExpand<Any>(Any::inplace<T>, tuple); };

            if constexpr (std::is_trivially_copyable_v<T>) {
                mIndex = Storage::registerLocalBssVariable(lambda);
            } else {
                mIndex = Storage::registerLocalObjectVariable(lambda);
            }
        }

        ~Global()
        {
            if constexpr (std::is_trivially_copyable_v<T>) {
                Storage::unregisterLocalBssVariable(mIndex);
            } else {
                Storage::unregisterLocalObjectVariable(mIndex);
            }
        }

        T *operator->() { return &data(); }

        T &operator*() { return data(); }

        operator T &() { return data(); }

        T *operator&() { return &data(); }

        Global<T, Storage> &operator=(const T &t)
        {
            data() = t;
            return *this;
        }

        T &data() { return Storage::localVariable(mIndex).template as<T>(); }

    private:
        int mIndex;
    };

    template <typename T, typename Storage>
    struct Global<T *, Storage> {

        Global(T *initial = nullptr)
        {
            mIndex = Storage::registerLocalBssVariable([=]() { return Any { initial }; });
        }

        ~Global()
        {
            Storage::unregisterLocalBssVariable(mIndex);
        }

        T *operator->() { return data(); }

        T &operator*() { return *data(); }

        operator T *() { return data(); }

        T *operator&() { return data(); }

        void operator=(T *t) { data() = t; }

        T *&data() { return Storage::localVariable(mIndex).template as<T *>(); }

    private:
        int mIndex;
    };

} // namespace Threading
} // namespace Engine
