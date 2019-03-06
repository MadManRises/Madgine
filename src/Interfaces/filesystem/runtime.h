#pragma once

#include "path.h"

namespace Engine
{
	namespace Filesystem
	{
		struct end_tag {};


		INTERFACES_EXPORT SharedLibraryQueryState *createSharedLibraryQueryState();
		INTERFACES_EXPORT void destroySharedLibraryQueryState(SharedLibraryQueryState *state);

		INTERFACES_EXPORT const char *filename(SharedLibraryQueryState &data);

		struct SharedLibraryIterator
		{
			SharedLibraryIterator(const SharedLibraryQuery* query);
			SharedLibraryIterator(const SharedLibraryQuery* query, end_tag);
			SharedLibraryIterator(SharedLibraryIterator &&);
			~SharedLibraryIterator();

			Path operator *() const;
			void operator++();

			bool operator!=(const SharedLibraryIterator &other) const;

			void close();

		private:
			const SharedLibraryQuery *mQuery;
			std::unique_ptr<SharedLibraryQueryState, decltype(&Filesystem::destroySharedLibraryQueryState)> mBuffer;
			void *mHandle;			
		};

		struct INTERFACES_EXPORT SharedLibraryQuery {
			SharedLibraryQuery(Path p);
			
			SharedLibraryIterator begin() const;
			SharedLibraryIterator end() const;

			const Path &path() const;

		private:
			Path mPath;			
		};


		INTERFACES_EXPORT SharedLibraryQuery listSharedLibraries();

	}
}