#pragma once

#include "path.h"

namespace Engine {
	namespace Filesystem {

		INTERFACES_EXPORT FileQueryState *createQueryState();
		INTERFACES_EXPORT void destroyQueryState(FileQueryState *state);
		
		INTERFACES_EXPORT bool isDir(FileQueryState &data);
		INTERFACES_EXPORT const char *filename(FileQueryState &data);

		struct INTERFACES_EXPORT FileQueryHandle {
			FileQueryHandle(Path p, FileQueryState &data);
			FileQueryHandle(FileQueryHandle &&other);
			~FileQueryHandle();

			bool advance(FileQueryState &data);
			operator bool();
			bool operator==(const FileQueryHandle &other) const;

			const Path &path() const;

			void close();

		private:			
			Path mPath;
			void *mHandle;
		};

		struct INTERFACES_EXPORT FileQueryIterator {
			FileQueryIterator(const FileQuery *query);
			FileQueryIterator(FileQueryIterator &&);
			~FileQueryIterator();

			Path operator *() const;
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
			std::unique_ptr<FileQueryState, decltype(&Filesystem::destroyQueryState)> mBuffer;
		};

		struct INTERFACES_EXPORT FileQuery {
			FileQuery(Path path, bool recursive);

			FileQueryIterator begin() const;
			FileQueryIterator end() const;

			bool isRecursive() const;

			const Path &path() const;

		private:
			Path mPath;
			bool mRecursive;
		};


	}
}