#pragma once

namespace Engine {
namespace Execution {

    template <typename... V>
    struct ValueStorageImpl {
        template <typename... U>
        ValueStorageImpl(U &&...u)
            : mValues(std::forward<U>(u)...)
        {
        }

        void reproduce(auto &rec)
        {
            TupleUnpacker::invokeExpand(LIFT(rec.set_value, &), std::move(mValues));
        }

        std::tuple<V...> mValues;
    };

    template <typename V>
    struct ValueStorageImpl<V> {
        template <typename U>
        ValueStorageImpl(U &&u)
            : mValue(std::forward<U>(u))
        {
        }

        void reproduce(auto &rec)
        {
            rec.set_value(std::forward<V>(mValue));
        }

        operator V()
        {
            return std::forward<V>(mValue);
        }

        V mValue;
    };

    template <typename R>
    struct ErrorStorageImpl {
        template <typename V>
        ErrorStorageImpl(V &&e)
            : mError(std::forward<V>(e))
        {
        }

        void reproduce(auto &rec)
        {
            rec.set_error(std::forward<R>(mError));
        }

        R mError;
    };

    template <>
    struct ErrorStorageImpl<void> {
        ErrorStorageImpl() = delete;

        void reproduce(auto &rec)
        {
            throw 0;
        }
    };

    template <typename Sender>
    using ValueStorage = typename Sender::template value_types<ValueStorageImpl>;

    template <typename Sender>
    using ErrorStorage = ErrorStorageImpl<typename Sender::result_type>;

    struct DoneStorage {
        void reproduce(auto &rec)
        {
            rec.set_done();
        }
    };

    struct NullStorage {
        void reproduce(auto &rec)
        {
            throw 0;
        }
    };

    template <typename Sender>
    struct ResultStorage {
        void reproduce(auto &rec)
        {
            std::visit([&](auto &storage) {
                storage.reproduce(rec);
            },
                mState);
        }

        void reproduce_error(auto &rec)
        {
            std::get<ErrorStorage<Sender>>(mState).reproduce(rec);
        }

        bool is_null() const
        {
            return std::holds_alternative<NullStorage>(mState);
        }

        bool is_value() const
        {
            return std::holds_alternative<ValueStorage<Sender>>(mState);
        }

        bool is_error() const
        {
            return std::holds_alternative<ErrorStorage<Sender>>(mState);
        }

        bool is_done() const
        {
            return std::holds_alternative<DoneStorage>(mState);
        }

        template <typename... V>
        void set_value(V &&...v)
        {
            assert(std::holds_alternative<NullStorage>(mState));
            mState.emplace<ValueStorage<Sender>>(std::forward<V>(v)...);
        }

        template <typename... R>
        void set_error(R &&...r)
        {
            assert(std::holds_alternative<NullStorage>(mState));
            mState.emplace<ErrorStorage<Sender>>(std::forward<R>(r)...);
        }

        void set_done()
        {
            assert(std::holds_alternative<NullStorage>(mState));
            mState.emplace<DoneStorage>();
        }

        ValueStorage<Sender> value() &&
        {
            return std::move(std::get<ValueStorage<Sender>>(mState));
        }

        std::variant<NullStorage, ValueStorage<Sender>, ErrorStorage<Sender>, DoneStorage> mState;
    };

}
}