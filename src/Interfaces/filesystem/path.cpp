#include "../interfaceslib.h"
#include "path.h"
#include "api.h"

namespace Engine {
	namespace Filesystem {

		Path::Path(const std::string &s) : Path(s.c_str()) {}

		Path::Path(const char * s)
		{
			size_t size = strlen(s);
			mPath.reserve(size);
			bool hadSeparator = false;
			for (size_t i = 0; i < size; ++i) {
				if (isSeparator(s[i])) 
				{
					if (!hadSeparator)
					{
						hadSeparator = true;
						mPath.push_back('/');
					}
				}
				else
				{
					hadSeparator = false;
					mPath.push_back(s[i]);
				}
			}
			if (hadSeparator && mPath.size() > 1)
			{
				mPath.pop_back();
			}
		}

		Path &Path::operator/=(const Path &other)
		{
			assert(other.isRelative());
			mPath.reserve(mPath.size() + other.mPath.size() + 1);
			mPath.push_back('/');
			mPath.append(other.mPath);
			return *this;
		}

		bool Path::operator==(const Path &other) const
		{
			return mPath == other.mPath;
		}

		Path Path::parentPath() const
		{
			auto sep = mPath.rfind('/');
			return sep == std::string::npos ? std::string{} : mPath.substr(0, sep);
		}

		Path Path::relative(const Path & base) const
		{
			size_t baseCount = base.mPath.size();;
			size_t count = mPath.size();

			if (baseCount >= count)
				return {};

			if (mPath.substr(0, baseCount) != base.mPath)
				return {};

			if (baseCount == count)
				return ".";

			if (mPath[baseCount] != '/')
				return {};

			return mPath.substr(baseCount + 1);
		}

		Path Path::filename() const
		{			
			auto sep = mPath.rfind('/');
			return sep == std::string::npos ? mPath : mPath.substr(sep + 1);
		}

		std::string Path::stem() const
		{
			std::string name = filename().mPath;
			auto pos = name.rfind('.');
			return pos == std::string::npos ? name : name.substr(0, pos);
		}

		std::string Path::extension() const
		{
			std::string name = filename().mPath;
			auto pos = name.rfind('.');
			return pos == std::string::npos ? std::string{} : name.substr(pos);
		}

		bool Path::isAbsolute() const
		{
			return Filesystem::isAbsolute(mPath);
		}

		bool Path::isRelative() const
		{
			return !isAbsolute();
		}

		bool Path::empty() const
		{
			return mPath.empty();
		}

		const std::string &Path::str() const
		{
			return mPath;
		}

		const char *Path::c_str() const
		{
			return mPath.c_str();
		}

	}
}