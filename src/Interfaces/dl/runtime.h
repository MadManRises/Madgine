#pragma once

#include "../filesystem/path.h"

namespace Engine {
namespace Dl {
    struct end_tag {
    };

    using SharedLibraryQueryStatePtr = std::unique_ptr<SharedLibraryQueryState, void(*)(SharedLibraryQueryState *)>;

    struct INTERFACES_EXPORT SharedLibraryIterator {
        SharedLibraryIterator(const SharedLibraryQuery *query);
        SharedLibraryIterator(const SharedLibraryQuery *query, end_tag);
        SharedLibraryIterator(SharedLibraryIterator &&);
        ~SharedLibraryIterator();

        Filesystem::Path operator*() const;
        void operator++();

        bool operator!=(const SharedLibraryIterator &other) const;

        void close();

    private:
        const SharedLibraryQuery *mQuery;
        SharedLibraryQueryStatePtr mBuffer;
        void *mHandle;
    };

    struct INTERFACES_EXPORT SharedLibraryQuery {
        SharedLibraryQuery(Filesystem::Path p);

        SharedLibraryIterator begin() const;
        SharedLibraryIterator end() const;

        const Filesystem::Path &path() const;

    private:
        Filesystem::Path mPath;
    };

    INTERFACES_EXPORT SharedLibraryQuery listSharedLibraries();
    //INTERFACES_EXPORT std::set<std::string> listLoadedLibraries();
}
}