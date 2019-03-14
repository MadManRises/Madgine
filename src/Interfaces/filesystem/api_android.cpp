#include "../interfaceslib.h"

#if ANDROID

#include "api.h"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../util/stringutil.h"

#include <android/asset_manager.h> 

namespace Engine {
	namespace Filesystem {

		DLL_EXPORT AAssetManager *sAssetManager = nullptr;

		static const char sAssetPrefix[] = "assets:";

		static bool isAssetPath(const Path &p)
		{
			return StringUtil::startsWith(p.str(), sAssetPrefix);
		}

		static const char *assetDir(const Path &p)
		{
			assert(isAssetPath(p));
			const char *dir = p.c_str() + sizeof(sAssetPrefix);
			if (isSeparator(*dir))
				++dir;
			return dir;
		}

		Path configPath()
		{
			//TODO
			char buffer[512];

			auto result = readlink("/proc/self/exe", buffer, sizeof(buffer));
			assert(result > 0);

			return Path(buffer).parentPath();
		}

		struct FileQueryState {
			union
			{
				dirent *mData;
				const char *mAssetData;
			};
			Path mPath;
		};

		static bool isAssetQuery(const FileQueryState &data)
		{
			return isAssetPath(data.mPath);
		}

		static bool skipSymbolic(void *handle, FileQueryState &data)
		{
			while (strcmp(data.mData->d_name, ".") == 0 ||
				strcmp(data.mData->d_name, "..") == 0)
			{
				data.mData = readdir((DIR*)handle);
				if (!data.mData)
					return false;
			}

			return true;
		}


		FileQueryHandle::FileQueryHandle(Path p, FileQueryState & data) :
			mPath(std::move(p)),
			mHandle(isAssetPath(mPath) ? (void*)AAssetManager_openDir(sAssetManager, assetDir(mPath)) : (void*)opendir(mPath.c_str()))
		{
			if (mHandle)
			{
				if (!advance(data))
				{
					close();
				}
			}
		}

		void FileQueryHandle::close()
		{
			if (mHandle)
			{
				if (isAssetPath(mPath))
					AAssetDir_close((AAssetDir*)mHandle);
				else
					closedir((DIR*)mHandle);
				mHandle = nullptr;
			}
		}

		bool FileQueryHandle::advance(FileQueryState &data)
		{
			data.mPath = mPath;
			if (isAssetPath(mPath))
			{
				data.mAssetData = AAssetDir_getNextFileName((AAssetDir*)mHandle);
				return data.mAssetData != nullptr;
			}
			else
			{
				data.mData = readdir((DIR*)mHandle);

				if (!data.mData)
					return false;
				return skipSymbolic(mHandle, data);
			}
		}

		FileQueryState *createQueryState()
		{
			return new FileQueryState;
		}

		void destroyQueryState(FileQueryState * state)
		{
			delete state;
		}

		bool isDir(FileQueryState &data)
		{
			if (isAssetPath(data.mPath))
				return false;

			struct stat statbuffer;
			auto result = stat((data.mPath / data.mData->d_name).c_str(), &statbuffer);
			assert(result != -1);
			return (statbuffer.st_mode & S_IFMT) == S_IFDIR;
		}

		const char *filename(FileQueryState &data)
		{
			if (isAssetPath(data.mPath))
				return data.mAssetData;
			else
				return data.mData->d_name;
		}


		void createDirectory(const Path & p)
		{
			assert(!isAssetPath(p));
			auto result = mkdir(p.c_str(), 0700);
			assert(result == 0);
		}

		bool exists(const Path & p)
		{
			if (isAssetPath(p))
			{
				AAssetDir *dir = AAssetManager_openDir(sAssetManager, assetDir(p));
				bool result = AAssetDir_getNextFileName(dir) != nullptr;
				AAssetDir_close(dir);
				return result;
			}
			else
			{
				return access(p.c_str(), F_OK) != -1;
			}
		}

		bool remove(const Path & p)
		{
			assert(!isAssetPath(p));
			return ::remove(p.c_str());
		}

		Path makeNormalized(const char * p)
		{
			return p;
		}

		bool isAbsolute(const Path &p)
		{
			return isAssetPath(p) || p.c_str()[0] == '/';
		}

		bool isSeparator(char c)
		{
			return c == '/';
		}

		std::vector<char> readFile(const Path & p)
		{
			if (isAssetPath(p))
			{
				AAsset *asset = AAssetManager_open(sAssetManager, assetDir(p), AASSET_MODE_STREAMING);
				if (!asset)
					return {};
				size_t len = AAsset_getLength64(asset);
				std::vector<char> buffer(len);
				int read = AAsset_read(asset, buffer.data(), len);
				assert(read == len);
				return buffer;
			}
			else
			{
				std::ifstream ifs(p.str());
				if (!ifs)
					return {};
				return std::vector<char>(std::istreambuf_iterator<char>(ifs),
					std::istreambuf_iterator<char>());
			}
		}


	}
}

#endif