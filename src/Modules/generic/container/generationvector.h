#pragma once

#if _DEBUG
#    include "Interfaces/debug/stacktrace.h"
#endif

namespace Engine {

struct GenerationVectorIndex {
    GenerationVectorIndex() = default;
    GenerationVectorIndex(const GenerationVectorIndex &) = delete;
    GenerationVectorIndex(GenerationVectorIndex &&other)
        : mIndex(std::exchange(other.mIndex, 0))
        , mGeneration(std::exchange(other.mGeneration, 0))
#if _DEBUG
        , mDebugMarker(std::exchange(other.mDebugMarker, 0))
#endif
    {
    }

    ~GenerationVectorIndex()
    {
        assert(!*this);
#if _DEBUG
        assert(mDebugMarker == 0);
#endif
    }

    operator bool() const
    {
        if (mGeneration != 0) {
            return true;
        } else {
            assert(mIndex == 0);
            return false;
        }
    }

    bool operator==(const GenerationVectorIndex &other) const
    {
        if (mGeneration == 0 || other.mGeneration == 0)
            return mGeneration == other.mGeneration;
        assert(mGeneration == other.mGeneration);
        return mIndex == other.mIndex;
    }

    bool operator!=(const GenerationVectorIndex &other) const
    {
        if (mGeneration == 0 || other.mGeneration == 0)
            return mGeneration != other.mGeneration;
        assert(mGeneration == other.mGeneration);
        return mIndex != other.mIndex;
    }

    void operator=(GenerationVectorIndex &&other)
    {
        std::swap(mIndex, other.mIndex);
        std::swap(mGeneration, other.mGeneration);
#if _DEBUG
        std::swap(mDebugMarker, other.mDebugMarker);
#endif
    }

private:
    friend struct GenerationVectorBase;

    GenerationVectorIndex(uint32_t index, uint32_t generation)
        : mIndex(index)
        , mGeneration(generation)
    {
    }

    void reset() const
    {
        mIndex = 0;
        mGeneration = 0;
    }

    mutable uint32_t mIndex = 0;
    mutable uint32_t mGeneration = 0;

#if _DEBUG
    mutable uint32_t mDebugMarker = 0;
#endif
};

struct GenerationVectorBase {

private:
    enum Entry {
        CLEAR = 1,
        ERASE = 2,
        SWAP_ERASE = 3
    };

public:
    GenerationVectorBase(size_t bufferSize = 64)
        : mHistory(bufferSize * 2)
    {
    }

    GenerationVectorBase(const GenerationVectorBase &) = delete;
    GenerationVectorBase(GenerationVectorBase &&) = delete;

    ~GenerationVectorBase()
    {
        assert(mGeneration == mLastKnownGeneration);
        assert(mCurrentIndex == mStartIndex);
        assert(mHistory[mCurrentIndex] == 0);
    }

    GenerationVectorIndex generate(uint32_t index) const
    {
        incRef();
        GenerationVectorIndex result { index, mGeneration };
#if _DEBUG
        result.mDebugMarker = ++mDebugCounter;
        auto pib = mDebugTraces.try_emplace(result.mDebugMarker, std::make_pair(mGeneration, Debug::StackTrace<5>::getCurrent(1).calculateReadable()));
        assert(pib.second);
#endif

        return result;
    }

    void reset(const GenerationVectorIndex &index) const
    {
        if (index) {
            if (index.mGeneration == mGeneration) {
                decRef(mCurrentIndex);
            } else {
                uint32_t historyIndex = mStartIndex;
                uint32_t gen = mLastKnownGeneration;
                while (gen != index.mGeneration) {
                    ++gen;
                    incIndex(historyIndex);
                }
                decRef(historyIndex);
            }
#if _DEBUG
            auto count = mDebugTraces.erase(index.mDebugMarker);
            assert(count == 1);
            index.mDebugMarker = 0;
#endif
            index.reset();
        }
    }

    GenerationVectorIndex copy(const GenerationVectorIndex &other) const
    {
        update(other);
        return generate(other.mIndex);
    }

    void increment(GenerationVectorIndex &index, ptrdiff_t diff = 1) const
    {
        update(index);
        index.mIndex += diff;
    }

    void update(const GenerationVectorIndex &index) const
    {
        if (index && index.mGeneration != mGeneration) {
            uint32_t historyIndex = mStartIndex;
            uint32_t gen = mLastKnownGeneration;
            while (gen != index.mGeneration) {
                ++gen;
                incIndex(historyIndex);
            }
            decRef(historyIndex);


            //Apply History
            uint32_t wrap = mHistory.size();
            while (index.mGeneration != mGeneration) {
                ++index.mGeneration;

                if (mHistory[(historyIndex + 1) % wrap] == CLEAR) {
                    index.reset();
#if _DEBUG
                    auto count = mDebugTraces.erase(index.mDebugMarker);
                    assert(count == 1);
                    index.mDebugMarker = 0;
#endif
                    return;
                }

                if (index.mIndex == mHistory[(historyIndex + 2) % wrap]) {
                    index.reset();
#if _DEBUG
                    auto count = mDebugTraces.erase(index.mDebugMarker);
                    assert(count == 1);
                    index.mDebugMarker = 0;
#endif
                    return;
                }
                if (mHistory[(historyIndex + 1) % wrap] == SWAP_ERASE) {
                    if (index.mIndex == mHistory[(historyIndex + 3) % wrap]) {
                        index.mIndex = mHistory[(historyIndex + 2) % wrap];
                    }
                } else {
                    assert(mHistory[(historyIndex + 1) % wrap] == ERASE);
                }
                incIndex(historyIndex);
            }

            
            incRef();

#if _DEBUG
            mDebugTraces.at(index.mDebugMarker).first = mGeneration;
#endif
        }
    }

protected:
    void onSwapErase(uint32_t index, uint32_t swapped)
    {
        if (mHistory[mCurrentIndex] == 0 && mGeneration == mLastKnownGeneration)
            return;
        uint32_t wrap = mHistory.size();
        assert((mStartIndex + wrap - 1 - mCurrentIndex) % wrap >= 4);
        mHistory[(mCurrentIndex + 1) % wrap] = SWAP_ERASE;
        mHistory[(mCurrentIndex + 2) % wrap] = index;
        mHistory[(mCurrentIndex + 3) % wrap] = swapped;
        mCurrentIndex = (mCurrentIndex + 4) % wrap;
        ++mGeneration;
        mHistory[mCurrentIndex] = 0;
    }

    void onErase(uint32_t index)
    {
        if (mHistory[mCurrentIndex] == 0 && mGeneration == mLastKnownGeneration)
            return;
        uint32_t wrap = mHistory.size();
        assert((mStartIndex + wrap - 1 - mCurrentIndex) % wrap >= 3);
        mHistory[(mCurrentIndex + 1) % wrap] = ERASE;
        mHistory[(mCurrentIndex + 2) % wrap] = index;
        mCurrentIndex = (mCurrentIndex + 3) % wrap;
        ++mGeneration;
        mHistory[mCurrentIndex] = 0;
    }

    void onClear()
    {
        if (mHistory[mCurrentIndex] == 0 && mGeneration == mLastKnownGeneration)
            return;
        uint32_t wrap = mHistory.size();
        assert((mStartIndex + wrap - 1 - mCurrentIndex) % wrap >= 2);
        mHistory[(mCurrentIndex + 1) % wrap] = CLEAR;
        mCurrentIndex = (mCurrentIndex + 2) % wrap;
        ++mGeneration;
        mHistory[mCurrentIndex] = 0;
    }

    uint32_t get(const GenerationVectorIndex &index) const
    {
        return index.mIndex;
    }

private:
    void incRef() const
    {
        ++mHistory[mCurrentIndex];
    }

    void decRef(uint32_t index) const
    {
        --mHistory[index];
        if (index == mStartIndex) {
            while (mHistory[mStartIndex] == 0 && mGeneration != mLastKnownGeneration) {
                ++mLastKnownGeneration;
                incIndex(mStartIndex);
            }
        }
    }

    void incIndex(uint32_t &index) const
    {
        uint32_t wrap = mHistory.size();
        index = (index + 1 + mHistory[(index + 1) % wrap]) % wrap;
    }

    mutable std::vector<uint32_t> mHistory;
    uint32_t mGeneration = 1;
    uint32_t mCurrentIndex = 0;
    mutable uint32_t mLastKnownGeneration = 1;
    mutable uint32_t mStartIndex = 0;

#if _DEBUG
    mutable uint32_t mDebugCounter = 0;
    mutable std::unordered_map<uint32_t, std::pair<uint32_t, Debug::FullStackTrace>> mDebugTraces;
#endif
};

template <typename T>
struct GenerationVector : GenerationVectorBase {

    using value_type = T;

    using GenerationVectorBase::GenerationVectorBase;

    struct iterator {

        using iterator_category = typename std::vector<T>::iterator::iterator_category;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;

        iterator() = default;

        iterator(GenerationVectorIndex index, GenerationVector<T> *vector)
            : mIndex(std::move(index))
            , mVector(vector)
        {
        }

        iterator(const iterator &other)
            : mIndex(other.mVector->copy(other.mIndex))
            , mVector(other.mVector)
        {
        }

        iterator(iterator &&) = default;

        ~iterator()
        {
            mVector->reset(mIndex);
        }

        T &operator*() const
        {
            return (*mVector)[mIndex];
        }

        T *operator->() const
        {
            return &(*mVector)[mIndex];
        }

        iterator &operator=(const iterator &other)
        {
            if (mVector)
                mVector->reset(mIndex);
            mVector = other.mVector;
            if (mVector)
                mIndex = mVector->copy(other.mIndex);
            return *this;
        }

        iterator &operator++()
        {
            mVector->increment(mIndex);
            return *this;
        }

        bool operator==(const iterator &other) const
        {
            assert(mVector == other.mVector || !mVector || !other.mVector);
            if (mVector)
                mVector->update(mIndex);
            if (other.mVector)
                other.mVector->update(other.mIndex);
            return mIndex == other.mIndex;
        }

        bool operator!=(const iterator &other) const
        {
            assert(mVector == other.mVector || !mVector || !other.mVector);
            if (mVector)
                mVector->update(mIndex);
            if (other.mVector)
                other.mVector->update(other.mIndex);
            return mIndex != other.mIndex;
        }

        ptrdiff_t operator-(const iterator &other) const
        {
            assert(mVector == other.mVector);
            mVector->update(mIndex);
            mVector->update(other.mIndex);
            return mVector->get(mIndex) - mVector->get(other.mIndex);
        }

        iterator &operator+=(ptrdiff_t diff)
        {
            mVector->increment(mIndex, diff);
            return *this;
        }

        iterator operator+(ptrdiff_t diff) const
        {
            GenerationVectorIndex index = mVector->copy(mIndex);
            mVector->increment(index, diff);
            return { std::move(index), mVector };
        }

        const GenerationVectorIndex &index() const &
        {
            if (mVector)
                mVector->update(mIndex);
            return mIndex;
        }

        operator GenerationVectorIndex() const
        {
            return mVector->copy(mIndex);
        }

        GenerationVectorIndex mIndex;
        GenerationVector<T> *mVector = nullptr;
    };

    struct const_iterator {

        using iterator_category = typename std::vector<T>::const_iterator::iterator_category;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T *;
        using reference = const T &;

        const_iterator() = default;

        const_iterator(const const_iterator &other)
            : mIndex(other.mVector->copy(other.mIndex))
            , mVector(other.mVector)
        {
        }

        const_iterator(const iterator &other)
            : mIndex(other.mVector->copy(other.mIndex))
            , mVector(other.mVector)
        {
        }

        const_iterator(const_iterator &&) = default;

        const_iterator(GenerationVectorIndex index, const GenerationVector<T> *vector)
            : mIndex(std::move(index))
            , mVector(vector)
        {
        }

        ~const_iterator()
        {
            mVector->reset(mIndex);
        }

        const T &operator*() const
        {
            return (*mVector)[mIndex];
        }

        const T *operator->() const
        {
            return &(*mVector)[mIndex];
        }

        const_iterator &operator++()
        {
            mVector->increment(mIndex);
            return *this;
        }

        bool operator!=(const const_iterator &other)
        {
            assert(mVector == other.mVector);
            mVector->update(mIndex);
            mVector->update(other.mIndex);
            return mIndex != other.mIndex;
        }

        ptrdiff_t operator-(const const_iterator &other) const
        {
            assert(mVector == other.mVector);
            mVector->update(mIndex);
            mVector->update(other.mIndex);
            return mVector->get(mIndex) - mVector->get(other.mIndex);
        }

        const_iterator operator+(ptrdiff_t diff) const
        {
            GenerationVectorIndex index = mVector->copy(mIndex);
            mVector->increment(index, diff);
            return { std::move(index), mVector };
        }

        operator GenerationVectorIndex() const
        {
            return mVector->copy(mIndex);
        }

        GenerationVectorIndex mIndex;
        const GenerationVector<T> *mVector;
    };

    struct reverse_iterator {

        ~reverse_iterator()
        {
            mVector->reset(mIndex);
        }

        reverse_iterator(const reverse_iterator &other)
            : mIndex(other.mVector->copy(other.mIndex))
            , mVector(other.mVector)
        {
        }

        reverse_iterator(reverse_iterator &&) = default;

        reverse_iterator(GenerationVectorIndex index, GenerationVector<T> *vector)
            : mIndex(std::move(index))
            , mVector(vector)
        {
        }

        T &operator*() const
        {
            return (*mVector)[mIndex];
        }

        T *operator->() const
        {
            return &(*mVector)[mIndex];
        }

        reverse_iterator &operator++()
        {
            mVector->increment(mIndex, -1);
            return *this;
        }

        bool operator!=(const reverse_iterator &other)
        {
            assert(mVector == other.mVector);
            mVector->update(mIndex);
            mVector->update(other.mIndex);
            return mIndex != other.mIndex;
        }

        ptrdiff_t operator-(const reverse_iterator &other)
        {
            assert(mVector == other.mVector);
            mVector->update(mIndex);
            mVector->update(other.mIndex);
            return mIndex.mIndex - other.mIndex.mIndex;
        }

        operator GenerationVectorIndex() const
        {
            return mVector->copy(mIndex);
        }

        GenerationVectorIndex mIndex;
        GenerationVector<T> *mVector;
    };

    struct const_reverse_iterator {
        ~const_reverse_iterator()
        {
            mVector->reset(mIndex);
        }

        const_reverse_iterator(const const_reverse_iterator &other)
            : mIndex(other.mVector->copy(other.mIndex))
            , mVector(other.mVector)
        {
        }

        const_reverse_iterator(const_reverse_iterator &&) = default;

        const_reverse_iterator(GenerationVectorIndex index, const GenerationVector<T> *vector)
            : mIndex(std::move(index))
            , mVector(vector)
        {
        }

        const T &operator*() const
        {
            return (*mVector)[mIndex];
        }

        const T *operator->() const
        {
            return &(*mVector)[mIndex];
        }

        const_reverse_iterator &operator++()
        {
            mVector->increment(mIndex, -1);
            return *this;
        }

        bool operator!=(const const_reverse_iterator &other)
        {
            assert(mVector == other.mVector);
            mVector->update(mIndex);
            mVector->update(other.mIndex);
            return mIndex != other.mIndex;
        }

        operator GenerationVectorIndex() const
        {
            return mVector->copy(mIndex);
        }

        GenerationVectorIndex mIndex;
        const GenerationVector<T> *mVector;
    };

    template <typename... Args>
    iterator emplace(Args &&... args)
    {
        T *t = &mData.emplace_back(std::forward<Args>(args)...);
        uint32_t index = t - &mData.front();
        return { generate(index), this };
    }

    iterator erase(const iterator &it)
    {
        return erase(it.index());
    }

    iterator erase(const GenerationVectorIndex &it)
    {
        update(it);

        uint32_t size = mData.size();
        uint32_t index = get(it);

        assert(index < size);

        if (index != size - 1) {
            using std::swap;
            swap(mData[index], mData[size - 1]);
            onSwapErase(index, size - 1);
        } else {
            onErase(index);
        }
        mData.pop_back();
        reset(it);
        return iterator { generate(index), this };
    }

    void clear()
    {
        onClear();
        mData.clear();
    }

    size_t size() const
    {
        return mData.size();
    }

    T &front()
    {
        return mData.front();
    }

    T &operator[](const GenerationVectorIndex &index)
    {
        update(index);
        assert(index);
        return mData[get(index)];
    }

    const T &operator[](const GenerationVectorIndex &index) const
    {
        update(index);
        assert(index);
        return mData[get(index)];
    }

    iterator begin()
    {
        return { generate(0), this };
    }

    iterator end()
    {
        return { generate(size()), this };
    }

    const_iterator begin() const
    {
        return { generate(0), this };
    }

    const_iterator end() const
    {
        return { generate(size()), this };
    }

    reverse_iterator rbegin()
    {
        return { generate(size() - 1), this };
    }

    reverse_iterator rend()
    {
        return { generate(-1), this };
    }

    const_reverse_iterator rbegin() const
    {
        return { generate(size() - 1), this };
    }

    const_reverse_iterator rend() const
    {
        return { generate(-1), this };
    }

private:
    std::vector<T> mData;
};

template <typename T>
struct container_traits<GenerationVector<T>, void> {
    static constexpr const bool sorted = true;
    static constexpr const bool has_dependent_handle = true;
    static constexpr const bool remove_invalidates_handles = true;
    static constexpr const bool is_fixed_size = false;

    typedef GenerationVector<T> container;
    typedef typename container::iterator iterator;
    typedef typename container::const_iterator const_iterator;
    typedef typename container::reverse_iterator reverse_iterator;
    typedef typename container::const_reverse_iterator const_reverse_iterator;

    struct handle_t {
        handle_t(size_t index = std::numeric_limits<size_t>::max())
            : mIndex(index)
        {
        }

        operator size_t() const { return mIndex; }

        void operator++() { ++mIndex; }
        void operator--() { --mIndex; }
        handle_t &operator-=(size_t s)
        {
            mIndex -= s;
            return *this;
        }

        size_t mIndex;
    };

    typedef handle_t handle;
    typedef handle_t const_handle;
    typedef handle_t position_handle;
    typedef handle_t const_position_handle;
    typedef typename container::value_type value_type;

    typedef iterator emplace_return;

    template <template <typename> typename M>
    using rebind = container_traits<GenerationVector<M<T>>>;

    template <typename... _Ty>
    static emplace_return emplace(container &c, const const_iterator &where, _Ty &&... args)
    {
        return c.emplace(std::forward<_Ty>(args)...);
    }

    static bool was_emplace_successful(const emplace_return &)
    {
        return true;
    }

    static position_handle toPositionHandle(container &c, const iterator &it)
    {
        return std::distance(c.begin(), it);
    }

    static handle toHandle(container &c, const position_handle &handle)
    {
        return handle;
    }

    static handle toHandle(container &c, const iterator &it)
    {
        if (it == c.end())
            return {};
        return std::distance(c.begin(), it);
    }

    static void revalidateHandleAfterInsert(position_handle &handle, const container &c, const const_iterator &it)
    {
        size_t item = std::distance(c.begin(), it);
        if (item <= handle)
            ++handle;
    }

    static void revalidateHandleAfterRemove(position_handle &handle, const container &c, const const_iterator &it, size_t count = 1)
    {
        size_t pivot = std::distance(c.begin(), it);
        assert(handle < pivot || handle >= pivot + count);
        if (handle > pivot)
            handle -= count;
    }

    static iterator toIterator(container &c, const position_handle &handle)
    {
        return c.begin() + handle;
    }

    static const_iterator toIterator(const container &c, const const_position_handle &handle)
    {
        return c.begin() + handle;
    }

    static position_handle next(const position_handle &handle)
    {
        return handle + 1;
    }
};

template <typename C, typename... Ty>
struct container_api_impl<C, GenerationVector<Ty...>> : C {

    using C::C;

    using C::operator=;

    using value_type = typename C::value_type;

    /*void resize(size_t size)
            {
                C::resize(size);
            }*/

    /*void remove(const value_type &item)
    {
        for (typename C::const_iterator it = this->begin(); it != this->end();) {
            if (*it == item) {
                it = erase(it);
            } else {
                ++it;
            }
        }
    }*/

    /*decltype(auto) push_back(const value_type &item)
    {
        return then(emplace_back(item), [](auto &&it) -> decltype(auto) {
            return *it;
        });
    }*/

    /*decltype(auto) push_back(value_type &&item)
    {
        return then(emplace_back(std::move(item)), [](auto &&it) -> decltype(auto) {
            return *it;
        });
    }*/

    /*template <typename... _Ty>
    decltype(auto) emplace_back(_Ty &&... args)
    {
        return this->emplace(this->end(), std::forward<_Ty>(args)...);
    }*/

    /*value_type &at(size_t i)
    {
        return C::at(i);
    }*/

    /*const value_type &at(size_t i) const
    {
        return C::at(i);
    }*/

    /*value_type &operator[](size_t i)
    {
        return C::operator[](i);
    }*/

    /*const value_type &operator[](size_t i) const
    {
        return C::operator[](i);
    }*/
};

}