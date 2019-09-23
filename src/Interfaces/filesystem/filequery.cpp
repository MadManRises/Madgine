#include "../interfaceslib.h"

#include "api.h"
#include "filequery.h"

namespace Engine {
namespace Filesystem {

    FileQueryHandle::FileQueryHandle(FileQueryHandle &&other)
        : mPath(std::move(other.mPath))
        , mHandle(std::exchange(other.mHandle, nullptr))
    {
    }

    FileQueryHandle::~FileQueryHandle()
    {
        close();
    }

    const Path &FileQueryHandle::path() const
    {
        return mPath;
    }

    FileQueryHandle::operator bool()
    {
        return mHandle != nullptr;
    }

    bool FileQueryHandle::operator==(const FileQueryHandle &other) const
    {
        return mHandle == other.mHandle && mPath == other.mPath;
    }

    FileQueryIterator::FileQueryIterator(const FileQuery *query)
        : mQuery(query)
        , mBuffer(createQueryState(), &destroyQueryState)
    {
    }

    FileQueryIterator::FileQueryIterator(FileQueryIterator &&other)
        : mHandles(std::move(other.mHandles))
        , mQuery(other.mQuery)
        , mBuffer(std::move(other.mBuffer))
    {
    }

    FileQueryIterator::~FileQueryIterator()
    {
    }

    FileQueryResult FileQueryIterator::operator*() const
    {
        return { &mHandles.back(), mBuffer.get() };
    }

    void FileQueryIterator::operator++()
    {
        if (mHandles.back().advance(*mBuffer))
            verify();
        else
            leaveDir();
    }

    bool FileQueryIterator::operator!=(const FileQueryIterator &other) const
    {
        assert(mQuery == other.mQuery);
        return mHandles != other.mHandles;
    }

    void FileQueryIterator::enterDir()
    {
        Path p = mHandles.empty() ? mQuery->mPath : **this;

        FileQueryHandle handle(p, *mBuffer);
        if (handle) {
            mHandles.emplace_back(std::move(handle));
            verify();
        }
    }

    void FileQueryIterator::leaveDir()
    {
        mHandles.pop_back();
        if (!mHandles.empty())
            ++(*this);
    }

    void FileQueryIterator::verify()
    {
        if (!mHandles.empty()) {
            if (currentIsDir()) {
                if (mQuery->mRecursive) {
                    enterDir();
                } else if (!mQuery->mShowFolders) {
                    ++(*this);
                }
            } else {
                if (!mQuery->mShowFiles) {
                    ++(*this);
                }
			}
        }
    }

    bool FileQueryIterator::currentIsDir()
    {
        return isDir(*mBuffer);
    }

    FileQueryIterator FileQuery::begin() const
    {
        FileQueryIterator it { this };
        it.enterDir();
        return it;
    }

    FileQueryIterator FileQuery::end() const
    {
        return FileQueryIterator { this };
    }

    bool FileQueryResult::isDir() const
    {
        return Filesystem::isDir(*mState);
    }

    FileQueryResult::operator Path() const
    {
        return path();
    }

    Path FileQueryResult::path() const
    {
        return mHandle->path() / filename(*mState);
    }

}
}