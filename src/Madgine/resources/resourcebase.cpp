#include "../baselib.h"

#include "resourcebase.h"

namespace Engine {
	namespace Resources {
		ResourceBase::ResourceBase(const std::experimental::filesystem::path& path) :
		mPath(path),
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

		const std::experimental::filesystem::path& ResourceBase::path()
		{
			return mPath;
		}

		std::string ResourceBase::extension()
		{
			return mPath.extension().generic_string();
		}

		std::string ResourceBase::name()
		{
			return mPath.stem().generic_string();
		}
	}
}
