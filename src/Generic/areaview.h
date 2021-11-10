#pragma once

namespace Engine {

template <typename T, size_t Dim, size_t Storage = Dim>
struct AreaView {

    AreaView(T *buffer, const std::array<size_t, Storage> &sizes)
        : mBuffer(buffer)
        , mSizes(sizes)
        , mFullSizes(sizes)
    {
        mFlipped.fill(false);
        for (size_t i = 0; i < Dim; ++i)
            mAxisMapping[i] = i;
    }

    AreaView(T *buffer, const std::array<size_t, Storage> &sizes, const std::array<size_t, Storage> &fullSizes, const std::array<bool, Storage> &flipped, const std::array<size_t, Storage> &axisMapping)
        : mBuffer(buffer)
        , mSizes(sizes)
        , mFullSizes(fullSizes)
        , mFlipped(flipped)
        , mAxisMapping(axisMapping)
    {
    }

    AreaView<T, Dim, Storage> subArea(const std::array<size_t, Storage> &topLeft, const std::array<size_t, Storage> &sizes)
    {
        size_t acc = 0;
        for (int i = Dim - 1; i >= 0; --i) {
            size_t axis = mAxisMapping[i];
            assert(topLeft[i] < mSizes[axis]);
            acc *= mFullSizes[axis];
            if (!mFlipped[axis]) {
                acc += topLeft[i];
            } else {
                acc += (mSizes[axis] - topLeft[i] - sizes[i]);
            }
        }
        return { mBuffer + acc, sizes, mFullSizes, mFlipped, mAxisMapping };
    }

    decltype(auto) operator[](size_t i) const
    {
        size_t axis = mAxisMapping[Dim - 1];
        assert(i < mSizes[axis]);
        if (mFlipped[axis])
            i = mSizes[axis] - 1 - i;
        size_t fullSize = 1;
        for (size_t j = 0; j < axis; ++j) {
            fullSize *= mFullSizes[j];
        }
        T *ptr = mBuffer + i * fullSize;
        if constexpr (Dim == 1) {
            return *ptr;
        } else {            
            return AreaView<T, Dim - 1, Storage> { ptr, mSizes, mFullSizes, mFlipped, mAxisMapping};
        }
    }

    T &at(const std::array<size_t, Storage> &indices)
    {
        return mBuffer[index(indices)];
    }

    const T &at(const std::array<size_t, Storage> &indices) const
    {
        return mBuffer[index(indices)];
    }

    void flip(size_t dim)
    {
        mFlipped[mAxisMapping[dim]] = !mFlipped[mAxisMapping[dim]];
    }

    void swapAxis(size_t a1, size_t a2)
    {
        std::swap(mAxisMapping[a1], mAxisMapping[a2]);
    }

    size_t index(const std::array<size_t, Storage> &indices) const
    {
        size_t acc = 0;
        for (int i = Dim - 1; i >= 0; --i) {
            size_t reverseAxis = std::find(mAxisMapping.begin(), mAxisMapping.end(), i) - mAxisMapping.begin();            
            assert(indices[reverseAxis] < mSizes[i]);
            acc *= mFullSizes[i];
            if (!mFlipped[i]) {
                acc += indices[reverseAxis];
            } else {
                acc += (mSizes[i] - 1 - indices[reverseAxis]);
            }
        }
        return acc;
    }

    struct iterator {

        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = void;
        using reference = T &;

        iterator(AreaView<T, Dim> *view)
            : mView(view)
        {
            mIndices.fill(0);
        }

        iterator(AreaView<T, Dim> *view, const std::array<size_t, Dim> &indices)
            : mView(view)
            , mIndices(indices)
        {
        }

        T &operator*()
        {
            return mView->at(mIndices);
        }

        void operator++()
        {
            ++mIndices[0];
            validate();
        }

        bool operator!=(const iterator &other)
        {
            return mView != other.mView || mIndices != other.mIndices;
        }

    private:
        void validate()
        {
            assert(mIndices[Dim - 1] < mView->mSizes[mView->mAxisMapping[Dim - 1]]);
            for (size_t i = 0; i < Dim - 1; ++i) {
                size_t axis = mView->mAxisMapping[i];
                assert(mIndices[i] <= mView->mSizes[axis]);
                if (mIndices[i] == mView->mSizes[axis]) {
                    mIndices[i] = 0;
                    ++mIndices[i + 1];
                } else {
                    break;
                }
            }
        }

    private:
        AreaView<T, Dim, Storage> *mView;
        std::array<size_t, Dim> mIndices;
    };

    struct const_iterator {

        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using difference_type = ptrdiff_t;
        using pointer = void;
        using reference = const T &;

        const_iterator(const AreaView<T, Dim> *view)
            : mView(view)
        {
            mIndices.fill(0);
        }

        const_iterator(const AreaView<T, Dim> *view, const std::array<size_t, Dim> &indices)
            : mView(view)
            , mIndices(indices)
        {
        }

        const T &operator*()
        {
            return mView->at(mIndices);
        }

        void operator++()
        {
            ++mIndices[0];
            validate();
        }

        bool operator!=(const const_iterator &other)
        {
            return mView != other.mView || mIndices != other.mIndices;
        }

    private:
        void validate()
        {
            assert(mIndices[Dim - 1] < mView->mSizes[mView->mAxisMapping[Dim - 1]]);
            for (size_t i = 0; i < Dim - 1; ++i) {
                size_t axis = mView->mAxisMapping[i];
                assert(mIndices[i] <= mView->mSizes[axis]);
                if (mIndices[i] == mView->mSizes[axis]) {
                    mIndices[i] = 0;
                    ++mIndices[i + 1];
                } else {
                    break;
                }
            }
        }

    private:
        const AreaView<T, Dim, Storage> *mView;
        std::array<size_t, Dim> mIndices;
    };

    iterator begin()
    {
        return { this };
    }

    iterator end()
    {
        std::array<size_t, Dim> endSizes;
        endSizes.fill(0);
        endSizes[Dim - 1] = mSizes[mAxisMapping[Dim - 1]];
        return { this, endSizes };
    }

    const_iterator begin() const
    {
        return { this };
    }

    const_iterator end() const
    {
        std::array<size_t, Dim> endSizes;
        endSizes.fill(0);
        endSizes[Dim - 1] = mSizes[mAxisMapping[Dim - 1]];
        return { this, endSizes };
    }

private:
    T *mBuffer;
    std::array<size_t, Storage> mSizes;
    std::array<size_t, Storage> mFullSizes;
    std::array<bool, Storage> mFlipped;
    std::array<size_t, Storage> mAxisMapping;
};

}