#include "../interfaceslib.h"

#include "runtime.h"

namespace Engine 
{
	namespace Filesystem
	{



		SharedLibraryQuery::SharedLibraryQuery(Path p) :
			mPath(std::move(p))
		{
		}

		SharedLibraryIterator SharedLibraryQuery::begin() const
		{
			return SharedLibraryIterator(this);
		}

		SharedLibraryIterator SharedLibraryQuery::end() const
		{
			return SharedLibraryIterator(this, end_tag{});
		}

		const Path & SharedLibraryQuery::path() const
		{
			return mPath;
		}		

		SharedLibraryIterator::SharedLibraryIterator(const SharedLibraryQuery * query, end_tag) :
			mQuery(query),
			mBuffer(nullptr, destroySharedLibraryQueryState),
			mHandle(nullptr)
		{
		}

		SharedLibraryIterator::SharedLibraryIterator(SharedLibraryIterator &&other) :
			mQuery(other.mQuery),
			mBuffer(std::move(other.mBuffer)),
			mHandle(std::exchange(other.mHandle, nullptr))
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

		Path SharedLibraryIterator::operator*() const
		{
			return Path(filename(*mBuffer));
		}


	}
}