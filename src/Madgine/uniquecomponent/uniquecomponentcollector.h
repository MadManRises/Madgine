#pragma once

#include "uniquecomponentregistry.h"

#ifndef STATIC_BUILD

#include "Interfaces/reflection/classname.h"

namespace Engine
{

	struct IndexHolder
	{	
		virtual size_t index() = 0;
	};


	template <class _Base, class _Ty>
	struct UniqueComponentCollector
	{
	public:
		typedef _Base Base;
		typedef _Ty Ty;
		typedef Collector_F<Base, Ty> F;
		typedef UniqueComponentRegistry<Base, Ty> Registry;

		UniqueComponentCollector() {
			mInfo.mComponents = reinterpret_cast<std::vector<Collector_F<void, void*>>*>(&mComponents);
			mInfo.mRegistryInfo = &ClassInfo<Registry>();
			mInfo.mBaseInfo = &ClassInfo<Base>();
			collectorRegistry()->mInfos.push_back(&mInfo);
		}
		UniqueComponentCollector(const UniqueComponentCollector&) = delete;
		void operator=(const UniqueComponentCollector&) = delete;

		static UniqueComponentCollector<_Base, _Ty> &sInstance();

		size_t size() const {
			return mComponents.size();
		}

	private:
		std::vector<F> mComponents;
		CollectorInfo mInfo;

		template <class T>
		static size_t registerComponent()
		{
			LOG(Database::message("Registering Component: ", "...")(ClassName<T>()));
			sInstance().mComponents.emplace_back(createComponent<T, _Base, _Ty>);
			sInstance().mInfo.mElementInfos.push_back(&ClassInfo<T>());
			return sInstance().mComponents.size() - 1;
		}

		static void unregisterComponent(size_t i)
		{
			sInstance().mComponents[i] = F();
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
		static UniqueComponentCollector<_Base, _Ty> dummy;
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

#define RegisterCollector(collector) RegisterClass(collector::Registry)
