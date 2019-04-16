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
			Resource(Loader *loader, const Filesystem::Path &path) :
				ResourceBase(path),
				mLoader(loader) {}

			virtual ~Resource()
			{
				if (!unload())
					LOG_WARNING("Deleted Resource \"" << name() << extension() << "\" still used. Memory not freed!");
			}

			std::shared_ptr<typename Loader::Data> data()
			{
				return mPtr;
			}

			std::shared_ptr<typename Loader::Data> loadData()
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
					ptr = mLoader->loadImpl(this);
					mWeakPtr = ptr;
					if (isPersistent())
						mPtr = ptr;
					return ptr;
				}
			}

			virtual bool load() override
			{
				return loadData().operator bool();
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
				if (b) {
					if (!mPtr && !mWeakPtr.expired())
					{
						mPtr = std::shared_ptr<typename Loader::Data>(mWeakPtr);
					}
				}
				else {
					mPtr.reset();
				}
			}
			
		private:
			std::shared_ptr<typename Loader::Data> mPtr;
			std::weak_ptr<typename Loader::Data> mWeakPtr;

			Loader *mLoader;

		};

		template <class Loader>
		class ThreadLocalResource : public ResourceBase
		{
		public:
			ThreadLocalResource(Loader *loader, const Filesystem::Path &path) :
				ResourceBase(path),
				mLoader(loader) {}

			virtual ~ThreadLocalResource()
			{
				if (!unload())
					LOG_WARNING("Deleted Resource \"" << name() << extension() << "\" still used. Memory not freed!");
			}

			std::shared_ptr<typename Loader::Data> data()
			{
				return mData.at(std::this_thread::get_id()).mPtr;
			}

			std::shared_ptr<typename Loader::Data> loadData()
			{
				Data &d = mData[std::this_thread::get_id()];
				if (d.mPtr)
				{
					return d.mPtr;
				}
				else if (std::shared_ptr<typename Loader::Data> ptr = d.mWeakPtr.lock())
				{
					return ptr;
				}
				else
				{
					ptr = mLoader->loadImpl(this);
					d.mWeakPtr = ptr;
					if (isPersistent())
						d.mPtr = ptr;
					return ptr;
				}
			}

			virtual bool load() override
			{
				return loadData().operator bool();
			}

			bool unload()
			{
				Data &d = mData.at(std::this_thread::get_id());
				d.mPtr.reset();
				bool b = d.mWeakPtr.expired();
				d.mWeakPtr.reset();
				return b;
			}

			virtual void setPersistent(bool b) override
			{
				ResourceBase::setPersistent(b);
				if (b) {
					for (std::pair<const std::thread::id, Data> &d : mData) {
						if (!d.second.mPtr && !d.second.mWeakPtr.expired())
						{
							d.second.mPtr = std::shared_ptr<typename Loader::Data>(d.second.mWeakPtr);
						}
					}
				}
				else {
					for (std::pair<const std::thread::id, Data> &d : mData) {
						d.second.mPtr.reset();
					}
				}
			}

		private:
			struct Data{
				std::shared_ptr<typename Loader::Data> mPtr;
				std::weak_ptr<typename Loader::Data> mWeakPtr;
			};

			std::map<std::thread::id, Data> mData;			

			Loader *mLoader;

		};

	}
}