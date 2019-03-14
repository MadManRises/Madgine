#include "../baselib.h"

#include "resourcebase.h"

#include "Interfaces/filesystem/api.h"

namespace Engine {
	namespace Resources {
		ResourceBase::ResourceBase(Filesystem::Path path) :
		mPath(std::move(path)),
		mIsPersistent(false)
		{
		}

		void ResourceBase::setPersistent(bool b)
		{
			mIsPersistent = b;
		}

		bool ResourceBase::isPersistent() const
		{
			return mIsPersistent;
		}

		const Filesystem::Path& ResourceBase::path()
		{
			return mPath;
		}

		std::string ResourceBase::extension()
		{
			return mPath.extension();
		}

		std::string ResourceBase::name()
		{
			return mPath.stem();
		}

		std::string ResourceBase::readAsText()
		{
			std::vector<char> buffer = Filesystem::readFile(mPath);
			return std::string{ buffer.begin(), buffer.end() };
			/*std::string result(buffer.size(), ' ');
			auto it = std::copy_if(buffer.begin(), buffer.end(), result.begin(), [](char c) {return c != '\r'; });
			result.resize(std::distance(result.begin(), it));
			return result;*/
		}

	}
}
