#pragma once

#include "uniquecomponentregistry.h"

#ifndef STATIC_BUILD

#include "../reflection/classname.h"

#include "indexholder.h"

namespace Engine
{

	template <class _Base, class _Ty>
	struct UniqueComponentCollector
	{
	public:
		typedef _Base Base;
		typedef _Ty Ty;
		typedef Collector_F<Base, Ty> F;
		typedef UniqueComponentRegistry<Base, Ty> Registry;

		UniqueComponentCollector()
		{
			mInfo.mRegistryInfo = &typeInfo<Registry>();
			mInfo.mBaseInfo = &typeInfo<Base>();
			mInfo.mBinary = &Plugins::PLUGIN_LOCAL(binaryInfo);
			Registry::sInstance().addCollector(&mInfo);
		}
		UniqueComponentCollector(const UniqueComponentCollector&) = delete;
		~UniqueComponentCollector()
		{
			Registry::sInstance().removeCollector(&mInfo);
		}

		void operator=(const UniqueComponentCollector&) = delete;

		static UniqueComponentCollector &sInstance();

		size_t size() const {
			return sInstance().mInfo.mComponents.size();
		}

	private:
		CollectorInfo mInfo;

		template <class T>
		static size_t registerComponent()
		{
			LOG(Database::message("Registering Component: ", "...")(typeName<T>()));
			sInstance().mInfo.mComponents.emplace_back(reinterpret_cast<Collector_F<void, void*>>(&createComponent<T, Base, Ty>));
			sInstance().mInfo.mElementInfos.push_back(&typeInfo<T>());
			return sInstance().mInfo.mComponents.size() - 1;
		}

		static void unregisterComponent(size_t i)
		{
			sInstance().mInfo.mComponents[i] = nullptr;
			sInstance().mInfo.mElementInfos[i] = nullptr;
		}

		static size_t baseIndex()
		{
			return sInstance().mInfo.mBaseIndex;
		}

	public:
		template <class T>
		class ComponentRegistrator : public IndexHolder
		{
		public:
			ComponentRegistrator()
			{
				mIndex = registerComponent<T>();
			}

			~ComponentRegistrator()
			{
				unregisterComponent(mIndex);
				mIndex = -1;
			}

			size_t index() override
			{
				return mIndex + baseIndex();
			}

		private:
			size_t mIndex = -1;

		};

	};



	template <class _Base, class _Ty>
	UniqueComponentCollector<_Base, _Ty>& UniqueComponentCollector<_Base, _Ty>::sInstance()
	{
		static UniqueComponentCollector dummy;
		return dummy;
	}

}

#else

namespace Engine {

	template <class _Base, class _Ty>
	struct UniqueComponentCollector {
		typedef _Base Base;
		typedef _Ty Ty;
		typedef UniqueComponentRegistry<Base, Ty> Registry;
	};

}

#endif

//#define RegisterCollector(collector) RegisterType(collector::Registry)
