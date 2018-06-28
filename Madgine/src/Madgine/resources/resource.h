#pragma once

#include "resourcebase.h"

namespace Engine
{
	namespace Resources
	{
	
		template <class Loader>
		class Resource : public ResourceBase
		{
		public:
			Resource(Loader *loader, const std::experimental::filesystem::path &path) :
				ResourceBase(path),
				mLoader(loader) {}

			virtual ~Resource()
			{
				if (!unload())
					LOG_WARNING(Database::message("Deleted Resource \"", "\" still used. Memory not freed!")(name()));
			}

			std::shared_ptr<typename Loader::Data> data()
			{
				return mPtr;
			}

			std::shared_ptr<typename Loader::Data> loadImpl()
			{
				if (mPtr)
				{
					return mPtr;
				}
				else if (std::shared_ptr<typename Loader::Data> ptr = mWeakPtr.lock())
				{
					return ptr;
				}
				else
				{
					ptr = mLoader->load(this);
					mWeakPtr = ptr;
					if (isPersistent())
						mPtr = ptr;
					return ptr;
				}
			}

			virtual bool load() override
			{
				return loadImpl().operator bool();
			}

			bool unload()
			{
				mPtr.reset();
				bool b = mWeakPtr.expired();
				mWeakPtr.reset();
				return b;
			}

			virtual void setPersistent(bool b) override
			{
				ResourceBase::setPersistent(b);
				if (!mPtr && !mWeakPtr.expired())
				{
					mPtr = std::shared_ptr<typename Loader::Data>(mWeakPtr);
				}
			}
			
		private:
			std::shared_ptr<typename Loader::Data> mPtr;
			std::weak_ptr<typename Loader::Data> mWeakPtr;

			Loader *mLoader;

		};

	}
}