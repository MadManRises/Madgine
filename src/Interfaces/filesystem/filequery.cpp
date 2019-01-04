#include "../interfaceslib.h"

#include "filequery.h"
#include "api.h"


namespace Engine {
	namespace Filesystem {

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

		FileQueryIterator::FileQueryIterator(const FileQuery * query) :
			mQuery(query),
			mBuffer(createQueryState(), &destroyQueryState)
		{
		}

		FileQueryIterator::FileQueryIterator(FileQueryIterator &&other) :
			mHandles(std::move(other.mHandles)),
			mQuery(other.mQuery),
			mBuffer(std::move(other.mBuffer))
		{
		}

		FileQueryIterator::~FileQueryIterator()
		{
		}

		Path FileQueryIterator::operator*() const
		{
			return mHandles.back().path() / filename(*mBuffer);
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
			Path p = mHandles.empty() ? mQuery->path() : **this;

			FileQueryHandle handle(p, *mBuffer);
			if (!handle)
				throw 0;

			mHandles.emplace_back(std::move(handle));
			verify();
		}

		void FileQueryIterator::leaveDir()
		{
			mHandles.pop_back();
			if (!mHandles.empty())
				++(*this);
		}

		void FileQueryIterator::verify()
		{
			if (!mHandles.empty())
			{
				if (currentIsDir())
				{
					if (mQuery->isRecursive())
					{
						enterDir();
					}
					else
					{
						++(*this);
					}
				}
			}
		}

		bool FileQueryIterator::currentIsDir()
		{
			return isDir(*mBuffer);
		}

		FileQuery::FileQuery(Path path, bool recursive) :
			mPath(std::move(path)),
			mRecursive(recursive)
		{
		}


		FileQueryIterator FileQuery::begin() const
		{
			FileQueryIterator it{ this };
			it.enterDir();
			return it;
		}

		FileQueryIterator FileQuery::end() const
		{
			return FileQueryIterator{ this };
		}

		bool FileQuery::isRecursive() const
		{
			return mRecursive;
		}

		const Path & FileQuery::path() const
		{
			return mPath;
		}


}
}