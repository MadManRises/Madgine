#include "../interfaceslib.h"

#include "runtime.h"

namespace Engine {
namespace Dl {

    extern const char *filename(SharedLibraryQueryState &data);

    SharedLibraryQuery::SharedLibraryQuery(Filesystem::Path p)
        : mPath(std::move(p))
    {
    }

    SharedLibraryIterator SharedLibraryQuery::begin() const
    {
        return SharedLibraryIterator(this);
    }

    SharedLibraryIterator SharedLibraryQuery::end() const
    {
        return SharedLibraryIterator(this, end_tag {});
    }

    const Filesystem::Path &SharedLibraryQuery::path() const
    {
        return mPath;
    }

    SharedLibraryIterator::SharedLibraryIterator(const SharedLibraryQuery *query, end_tag)
        : mQuery(query)
        , mBuffer(nullptr, nullptr)
        , mHandle(nullptr)
    {
    }

    SharedLibraryIterator::SharedLibraryIterator(SharedLibraryIterator &&other)
        : mQuery(other.mQuery)
        , mBuffer(std::move(other.mBuffer))
        , mHandle(std::exchange(other.mHandle, nullptr))
    {
    }

    SharedLibraryIterator::~SharedLibraryIterator()
    {
        close();
    }

    bool SharedLibraryIterator::operator!=(const SharedLibraryIterator &other) const
    {
        assert(mQuery == other.mQuery);
        return other.mHandle != mHandle;
    }

    Filesystem::Path SharedLibraryIterator::operator*() const
    {
        return Filesystem::Path(filename(*mBuffer));
    }

}
}