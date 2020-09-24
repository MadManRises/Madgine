#pragma once 

#if ENABLE_MEMTRACKING
#    include "Interfaces/debug/stacktrace.h"
#endif

#include "containerevent.h"

namespace Engine {

struct GenerationVectorIndex {

    static constexpr uint16_t INVALID_GENERATION = std::numeric_limits<uint16_t>::max();
    static constexpr uint32_t INVALID_INDEX = std::numeric_limits<uint32_t>::max();

    GenerationVectorIndex() = default;
    GenerationVectorIndex(const GenerationVectorIndex &) = delete;
    GenerationVectorIndex(GenerationVectorIndex &&other)
        : mIndex(std::exchange(other.mIndex, INVALID_INDEX))
        , mGeneration(std::exchange(other.mGeneration, INVALID_GENERATION))
#if ENABLE_MEMTRACKING
        , mDebugMarker(std::exchange(other.mDebugMarker, 0))
#endif
    {
    }

    ~GenerationVectorIndex()
    {
        assert(!*this);
#if ENABLE_MEMTRACKING
        assert(mDebugMarker == 0);
#endif
    }

    operator bool() const
    {
        if (mGeneration != INVALID_GENERATION) {
            return true;
        } else {
            assert(mIndex == INVALID_INDEX);
            return false;
        }
    }

    bool operator==(const GenerationVectorIndex &other) const
    {
        if (mGeneration == INVALID_GENERATION || other.mGeneration == INVALID_GENERATION)
            return mGeneration == other.mGeneration;
        assert(mGeneration == other.mGeneration);
        return mIndex == other.mIndex;
    }

    bool operator!=(const GenerationVectorIndex &other) const
    {
        if (mGeneration == INVALID_GENERATION || other.mGeneration == INVALID_GENERATION)
            return mGeneration != other.mGeneration;
        assert(mGeneration == other.mGeneration);
        return mIndex != other.mIndex;
    }

    void operator=(GenerationVectorIndex &&other)
    {
        std::swap(mIndex, other.mIndex);
        std::swap(mGeneration, other.mGeneration);
#if ENABLE_MEMTRACKING
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

    void reset()
    {
        mIndex = INVALID_INDEX;
        mGeneration = INVALID_GENERATION;
    }

    uint32_t mIndex = INVALID_INDEX;
    uint16_t mGeneration = INVALID_GENERATION;

#if ENABLE_MEMTRACKING
    mutable uint32_t mDebugMarker = 0;
#endif
};

struct GenerationVectorBase {
    GenerationVectorBase(uint16_t bufferSize = 64)
        : mHistory(bufferSize)
    {
        assert(bufferSize < GenerationVectorIndex::INVALID_GENERATION);
    }

    GenerationVectorBase(const GenerationVectorBase &) = delete;
    GenerationVectorBase(GenerationVectorBase &&) = delete;

    ~GenerationVectorBase()
    {
        for (Entry &entry : mHistory)
            assert(entry.mRefCount == 0);
    }

    GenerationVectorIndex generate(uint32_t index) const
    {
        uint16_t gen = mGeneration.load();
        incRef(gen);
        GenerationVectorIndex result { index, gen };
#if ENABLE_MEMTRACKING
        std::lock_guard lock { mDebugMutex };
        result.mDebugMarker = ++mDebugCounter;
        auto pib = mDebugTraces.try_emplace(result.mDebugMarker, std::make_pair(gen, Debug::StackTrace<8>::getCurrent(1)));
        assert(pib.second);
#endif

        return result;
    }

    void reset(GenerationVectorIndex &index) const
    {
        if (index) {
            decRef(index.mGeneration);
#if ENABLE_MEMTRACKING
            {
                std::lock_guard lock { mDebugMutex };
                auto count = mDebugTraces.erase(index.mDebugMarker);
                assert(count == 1);
                index.mDebugMarker = 0;
            }
#endif
            index.reset();
        }
    }

    GenerationVectorIndex copy(GenerationVectorIndex &other) const
    {
        update(other);
        if (!other)
            return {};
        return generate(other.mIndex);
    }

    void increment(GenerationVectorIndex &index, size_t size, ptrdiff_t diff = 1) const
    {
        update(index);
        assert(index || diff == 0);
        index.mIndex += diff;
        assert(index.mIndex <= size || index.mIndex == std::numeric_limits<uint32_t>::max());
        if (index.mIndex == size || index.mIndex == std::numeric_limits<uint32_t>::max())
            reset(index);
    }

    uint32_t update(GenerationVectorIndex &index) const
    {
        uint32_t oldIndex = index.mIndex;
        if (index && index.mGeneration != mGeneration) {

            uint32_t oldGen = index.mGeneration;

            //Apply History
            uint32_t wrap = mHistory.size();
            while (index.mGeneration != mGeneration) {

                const Entry &entry = mHistory[index.mGeneration];

                if (entry.mOp == RESET) {
                    index.reset();
#if ENABLE_MEMTRACKING
                    std::lock_guard lock { mDebugMutex };
                    auto count = mDebugTraces.erase(index.mDebugMarker);
                    assert(count == 1);
                    index.mDebugMarker = 0;
#endif
                    break;
                }

                assert(entry.mOp == ERASE || entry.mOp == SWAP_ERASE);
                if (index.mIndex == entry.mArg1) {
                    index.reset();
#if ENABLE_MEMTRACKING
                    std::lock_guard lock { mDebugMutex };
                    auto count = mDebugTraces.erase(index.mDebugMarker);
                    assert(count == 1);
                    index.mDebugMarker = 0;
#endif
                    break;
                }
                if (entry.mOp == SWAP_ERASE) {
                    if (index.mIndex == entry.mArg2) {
                        index.mIndex = entry.mArg1;
                    }
                } else {
                    assert(entry.mOp == ERASE);
                }
                index.mGeneration = (index.mGeneration + 1) % wrap;
            }

            if (index)
                incRef(index.mGeneration);
            decRef(oldGen);

#if ENABLE_MEMTRACKING
            if (index) {
                std::lock_guard lock { mDebugMutex };
                mDebugTraces.at(index.mDebugMarker).first = mGeneration;
            }
#endif
        }
        return oldIndex;
    }

protected:
    void onSwapErase(uint32_t index, uint32_t swapped)
    {
        uint16_t gen = mGeneration.load();
        uint16_t nextGen = (gen + 1) % mHistory.size();
        if (mHistory[nextGen].mRefCount != 0)
            throw "Ring buffer is full!";
        mHistory[gen].mOp = SWAP_ERASE;
        mHistory[gen].mArg1 = index;
        mHistory[gen].mArg2 = swapped;
        if (!mGeneration.compare_exchange_strong(gen, nextGen))
            throw "Concurrent writes!!";
    }

    void onErase(uint32_t index)
    {
        uint16_t gen = mGeneration.load();
        uint16_t nextGen = (gen + 1) % mHistory.size();
        if (mHistory[nextGen].mRefCount != 0)
            throw "Ring buffer is full!";
        mHistory[gen].mOp = ERASE;
        mHistory[gen].mArg1 = index;
        if (!mGeneration.compare_exchange_strong(gen, nextGen))
            throw "Concurrent writes!!";
    }

    void onClear()
    {

        uint16_t gen = mGeneration.load();
        uint16_t nextGen = (gen + 1) % mHistory.size();
        if (mHistory[nextGen].mRefCount != 0)
            throw "Ring buffer is full!";
        mHistory[gen].mOp = RESET;
        if (!mGeneration.compare_exchange_strong(gen, nextGen))
            throw "Concurrent writes!!";
    }

    uint32_t get(const GenerationVectorIndex &index) const
    {
        assert(index);
        return index.mIndex;
    }

    uint32_t get(const GenerationVectorIndex &index, uint32_t end) const
    {
        return index ? index.mIndex : end;
    }

private:
    void incRef(uint16_t index) const
    {
        ++mHistory[index].mRefCount;
    }

    void decRef(uint16_t index) const
    {
        --mHistory[index].mRefCount;
    }

    struct Entry {
        mutable std::atomic<uint16_t> mRefCount = 0;
        ContainerEvent mOp;
        uint32_t mArg1, mArg2;
    };

    std::vector<Entry> mHistory;
    std::atomic<uint16_t> mGeneration = 0;

#if ENABLE_MEMTRACKING
    mutable uint32_t mDebugCounter = 0;
    mutable std::unordered_map<uint32_t, std::pair<uint16_t, Debug::StackTrace<8>>> mDebugTraces;
    mutable std::mutex mDebugMutex;
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
            : mIndex(other.mIndex ? other.mVector->copy(other.mIndex) : GenerationVectorIndex {})
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
            mVector->increment(mIndex, mVector->size());
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
            return mVector->get(mIndex, mVector->size()) - mVector->get(other.mIndex, mVector->size());
        }

        iterator &operator+=(ptrdiff_t diff)
        {
            mVector->increment(mIndex, mVector->size(), diff);
            return *this;
        }

        iterator operator+(ptrdiff_t diff) const
        {
            GenerationVectorIndex index = mVector->copy(mIndex);
            mVector->increment(index, mVector->size(), diff);
            return { std::move(index), mVector };
        }

        void update() const
        {
            if (mVector)
                mVector->update(mIndex);
        }

        GenerationVectorIndex copyIndex() const
        {
            return mVector->copy(mIndex);
        }

        uint32_t index() const
        {
            return mVector->get(mIndex);
        }

        bool valid() const
        {
            update();
            return mIndex;
        }

        mutable GenerationVectorIndex mIndex;
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
            mVector->increment(mIndex, mVector->size());
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
            return mVector->get(mIndex, mVector->size()) - mVector->get(other.mIndex, mVector->size());
        }

        const_iterator operator+(ptrdiff_t diff) const
        {
            GenerationVectorIndex index = mVector->copy(mIndex);
            mVector->increment(index, mVector->size(), diff);
            return { std::move(index), mVector };
        }

        GenerationVectorIndex copyIndex() const
        {
            return mVector->copy(mIndex);
        }

        mutable GenerationVectorIndex mIndex;
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
            mVector->increment(mIndex, mVector->size(), -1);
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
            return mVector->get(other.mIndex, std::numeric_limits<uint32_t>::max()) - mVector->get(mIndex, std::numeric_limits<uint32_t>::max());
        }

        GenerationVectorIndex copyIndex() const
        {
            return mVector->copy(mIndex);
        }

        mutable GenerationVectorIndex mIndex;
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
            mVector->increment(mIndex, mVector->size(), -1);
            return *this;
        }

        bool operator!=(const const_reverse_iterator &other)
        {
            assert(mVector == other.mVector);
            mVector->update(mIndex);
            mVector->update(other.mIndex);
            return mIndex != other.mIndex;
        }

        GenerationVectorIndex copyIndex() const
        {
            return mVector->copy(mIndex);
        }

        mutable GenerationVectorIndex mIndex;
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
        return erase(it.mIndex);
    }

    iterator erase(GenerationVectorIndex &it)
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

    bool empty() const
    {
        return mData.empty();
    }

    T &front()
    {
        return mData.front();
    }

    T& operator[](size_t i) {
        return mData[i];
    }

    const T &operator[](size_t i) const
    {
        return mData[i];
    }

    T &operator[](GenerationVectorIndex &index)
    {
        update(index);
        assert(index);
        return mData[get(index)];
    }

    const T &operator[](GenerationVectorIndex &index) const
    {
        update(index);
        assert(index);
        return mData[get(index)];
    }

    T &at(size_t i)
    {
        return mData.at(i);
    }

    const T &at(size_t i) const
    {
        return mData.at(i);
    }

    T &at(GenerationVectorIndex &index)
    {
        update(index);
        assert(index);
        return mData.at(get(index));
    }

    const T &at(GenerationVectorIndex &index) const
    {
        update(index);
        assert(index);
        return mData.at(get(index));
    }

    iterator begin()
    {
        if (mData.empty())
            return end();
        return { generate(0), this };
    }

    iterator end()
    {
        return { {}, this };
    }

    const_iterator begin() const
    {
        if (mData.empty())
            return end();
        return { generate(0), this };
    }

    const_iterator end() const
    {
        return { {}, this };
    }

    reverse_iterator rbegin()
    {
        if (mData.empty())
            return rend();
        return { generate(size() - 1), this };
    }

    reverse_iterator rend()
    {
        return { {}, this };
    }

    const_reverse_iterator rbegin() const
    {
        if (mData.empty())
            return rend();
        return { generate(size() - 1), this };
    }

    const_reverse_iterator rend() const
    {
        return { {}, this };
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
        if (it == c.end())
            return {};
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
    }

    static void revalidateHandleAfterRemove(position_handle &handle, const container &c, const const_iterator &it, size_t count = 1)
    {
        throw "TODO";
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

    using C::at;
    using C::operator[];
    using C::copy;
    using C::reset;
    using C::update;
};

}