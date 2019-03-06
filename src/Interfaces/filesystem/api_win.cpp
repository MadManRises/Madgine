#include "../interfaceslib.h"

#if WINDOWS

#include "api.h"
#include "path.h"

#define NOMINMAX
#include <Windows.h>


namespace Engine {
	namespace Filesystem {

		struct FileQueryState {
			WIN32_FIND_DATA mData;
		};

		static bool skipSymbolic(void *handle, FileQueryState &data)
		{
			while (strcmp(data.mData.cFileName, ".") == 0 ||
				strcmp(data.mData.cFileName, "..") == 0)
				if (!FindNextFile(handle, &data.mData))
					return false;
			return true;
		}

		FileQueryHandle::FileQueryHandle(Path p, FileQueryState & data) :
			mPath(std::move(p)),
			mHandle(FindFirstFile((mPath.str() + "/*").c_str(), &data.mData))
		{
			if (mHandle == INVALID_HANDLE_VALUE)
				mHandle = nullptr;			
			else if (!skipSymbolic(mHandle, data))			
				close();			
		}


		void FileQueryHandle::close()
		{
			if (mHandle)
			{
				FindClose(mHandle);
				mHandle = nullptr;
			}
		}

		bool FileQueryHandle::advance(FileQueryState &data)
		{
			if (!FindNextFile(mHandle, &data.mData))
				return false;
			return skipSymbolic(mHandle, data);
		}


		FileQueryState * createQueryState()
		{
			return new FileQueryState;
		}

		void destroyQueryState(FileQueryState * state)
		{
			delete state;
		}
		
		bool isDir(FileQueryState &data)
		{
			return (data.mData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		}

		const char *filename(FileQueryState &data)
		{
			return data.mData.cFileName;
		}


		Path configPath()
		{
			char buffer[512];

			auto result = GetModuleFileName(nullptr, buffer, sizeof(buffer));
			assert(result > 0);

			return Path(buffer).parentPath();
		}


		void createDirectory(const Path & p)
		{
			auto result = CreateDirectory(p.c_str(), NULL);
			assert(result);
		}

		bool exists(const Path & p)
		{
			return GetFileAttributes(p.c_str()) != INVALID_FILE_ATTRIBUTES;
		}

		bool remove(const Path & p)
		{
			return DeleteFile(p.c_str());
		}

		Path makeNormalized(const char * p)
		{
			char buffer[512];
			auto result = GetLongPathNameA(p, buffer, sizeof(buffer));
			assert(result > 0 && result < sizeof(buffer));
			buffer[0] = toupper(buffer[0]);
			for (size_t i = 0; i < result; ++i)
				if (buffer[i] == '\\')
					buffer[i] = '/';
			return { buffer };
		}

		bool isAbsolute(const Path &p)
		{
			auto colon = p.str().find(':');
			if (colon == std::string::npos)
				return false;
			auto sep = p.str().find('/');
			return sep == std::string::npos || colon < sep;
		}

		bool isSeparator(char c)
		{
			return c == '/' || c == '\\';
		}

	}
}

#endif