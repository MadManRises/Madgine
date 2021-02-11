#pragma once

namespace Engine {

struct KeyValueTuplefyApplyer {
    template <typename A>
    void operator()(KeyValuePair& p, A&& accessor) {
        std::forward<A>(accessor)(p);
    }
};

template <typename T>
struct KeyValueTupleIterator {

    using AccessorF = void (*)(KeyValuePair &, T &);

    struct Accessor {
        void operator()(KeyValuePair& p) {
            mAccessor(p, mData);
        }
        AccessorF mAccessor;
        T &mData;
    };

    template <size_t... Is>
    Accessor helper(std::index_sequence<Is...>) const {
        static constexpr AccessorF accessors[] = {
            [](KeyValuePair &p, T &t) { 
                to_ValueType(KeyValuePair_key(p), Is); 
                to_ValueTypeRef(KeyValuePair_value(p), std::get<Is>(TupleUnpacker::toTuple(t)));
            }...
        };

        return { accessors[mIndex], mData };
    }

    Accessor operator*() const
    {
        static constexpr size_t size = std::tuple_size_v<decltype(TupleUnpacker::toTuple(mData))>;
        assert(mIndex < size);

        return helper(std::make_index_sequence<size> {});
    }

    void operator++()
    {
        ++mIndex;
    }

    bool operator==(const KeyValueTupleIterator<T> &other) const
    {
        assert(&mData == &other.mData);
        return mIndex == other.mIndex;
    }

    size_t mIndex;
    T &mData;
};

template <typename T>
struct KeyValueTuplefyer {

    KeyValueTuplefyer(T &&t)
        : mData(std::forward<T>(t))
    {
    }

    KeyValueTupleIterator<T> begin()
    {
        return { 0, mData };
    }

    KeyValueTupleIterator<T> end()
    {
        return { std::tuple_size_v<decltype(TupleUnpacker::toTuple(mData))>, mData };
    }

private:
    T mData;
};

template <typename T>
KeyValueTuplefyer(T &&) -> KeyValueTuplefyer<T>;

}