#pragma once

namespace Engine {

struct MADGINE_ULAM_EXPORT Ulam {

    struct Triple {
        Triple(int value)
            : mValue(value)
        {
        }

        operator int() const {
            return mValue;
        }

        size_t mCount3 = 0, mCount2 = 0;
        int mValue;
    };

    struct MADGINE_ULAM_EXPORT iterator {

        iterator(int value);

        void operator++();
        bool operator!=(const iterator &) const;
        const Triple &operator*() const;

        Triple mValue;
    };

    iterator begin();
    iterator end();

    int mValue;
};

}