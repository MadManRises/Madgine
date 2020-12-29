#pragma once

#include "path.h"

namespace Engine {
namespace Filesystem {

    FileQueryState *createQueryState();
    void destroyQueryState(FileQueryState *state);
    struct FileQueryStateDeleter {
        void operator()(FileQueryState *state) { destroyQueryState(state); }
    };

    INTERFACES_EXPORT bool isDir(const FileQueryState &data);
    INTERFACES_EXPORT const char *filename(const FileQueryState &data);

    struct INTERFACES_EXPORT FileQueryHandle {
        FileQueryHandle(Path p, FileQueryState &data);
        FileQueryHandle(FileQueryHandle &&other);
        ~FileQueryHandle();

        bool advance(FileQueryState &data);
        explicit operator bool();
        bool operator==(const FileQueryHandle &other) const;

        const Path &path() const;

        void close();

    private:
        Path mPath;
        void *mHandle;
    };

    struct INTERFACES_EXPORT FileQueryResult {
        const FileQueryHandle *mHandle;
        const FileQueryState *mState;

        bool isDir() const;
        operator Path() const;
        Path path() const;
    };

    struct INTERFACES_EXPORT FileQueryIterator {
        FileQueryIterator(const FileQuery *query);
        FileQueryIterator(FileQueryIterator &&);
        ~FileQueryIterator();

        FileQueryResult operator*() const;
        void operator++();

        bool operator!=(const FileQueryIterator &other) const;

        void enterDir();
        void leaveDir();

        bool currentIsDir();

    protected:
        void verify();

    private:
        std::vector<FileQueryHandle> mHandles;
        const FileQuery *mQuery;
        std::unique_ptr<FileQueryState, FileQueryStateDeleter> mBuffer;
    };

    struct INTERFACES_EXPORT FileQuery {

        FileQueryIterator begin() const;
        FileQueryIterator end() const;

        Path mPath;
        bool mRecursive = false;
        bool mShowDirs = false;
        bool mShowFiles = true;
    };


}
}