#include "../baselib.h"

#include "resourcebase.h"

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
	}
}
