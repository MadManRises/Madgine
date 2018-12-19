#pragma once

#include "Interfaces/reflection/classname.h"

#include "Interfaces/signalslot/signal.h"

namespace Engine {

	template <class T, class Base, class _Ty>
	std::unique_ptr<Base> createComponent(_Ty arg) {
		return std::make_unique<T>(std::forward<_Ty>(arg));
	}

	template <class Base, class _Ty>
	using Collector_F = std::unique_ptr<Base>(*)(_Ty);

	template <class _Base, class _Ty, template <class...> class Container = std::vector>
	class UniqueComponentContainer;

	template <class _Base, class _Ty>
	class UniqueComponentSelector;

}

#ifndef STATIC_BUILD

namespace Engine{

	

	struct CollectorInfo {
		const std::vector<Collector_F<void, void*>> *mComponents;
		const TypeInfo *mRegistryInfo;
		const TypeInfo *mBaseInfo;
		std::vector<const TypeInfo*> mElementInfos;
		size_t mBaseIndex = 0;
	};

	struct CollectorRegistry {
		std::vector<CollectorInfo*> mInfos;
	};

	MADGINE_BASE_EXPORT void exportStaticComponentHeader(const std::experimental::filesystem::path &outFile, std::vector<const TypeInfo*> skip = {});
	   
	extern "C"
#ifdef PLUGIN_BUILD
		DLL_EXPORT
#else
		MADGINE_BASE_EXPORT
#endif
		inline CollectorRegistry *PLUGIN_LOCAL(collectorRegistry)()
	{
		static CollectorRegistry dummy;
		return &dummy;
	}
		
	MADGINE_BASE_EXPORT std::map<std::string, ComponentRegistryBase *> &registryRegistry();

	struct MADGINE_BASE_EXPORT ComponentRegistryBase {
		ComponentRegistryBase(const TypeInfo *ti) : mTi(ti){
			registryRegistry()[ti->mFullName] = this;
		}

		~ComponentRegistryBase() {
			registryRegistry().erase(mTi->mFullName);
		}

		virtual void addCollector(CollectorInfo *info) = 0;
		virtual void removeCollector(CollectorInfo *info) = 0;

		const TypeInfo *type_info() {
			return mTi;
		}

	private:
		const TypeInfo *mTi;
		const Plugins::BinaryInfo *mBi;
	};

	template <class _Base, class _Ty>
	struct DLL_EXPORT UniqueComponentRegistry : ComponentRegistryBase {

		typedef _Base Base;
		typedef _Ty Ty;
		typedef Collector_F<Base, Ty> F;

		UniqueComponentRegistry() :
			ComponentRegistryBase(&ClassInfo<UniqueComponentRegistry>()) {}

		static UniqueComponentRegistry sInstance;
		static std::vector<F> &sComponents() {
			return sInstance.mComponents;
		}

		void addCollector(CollectorInfo *info) override {
			info->mBaseIndex = mComponents.size();
			const std::vector<F> &comps = reinterpret_cast<const std::vector<F>&>(*info->mComponents);
			for (F f : comps) {
				mComponents.push_back(f);
			}
			mCollectors.push_back(info);
			mUpdate.emit(info, true, comps);			
		}

		void removeCollector(CollectorInfo *info) override {
			mComponents.erase(mComponents.begin() + info->mBaseIndex, mComponents.begin() + info->mBaseIndex + info->mComponents->size());

			mCollectors.erase(std::remove(mCollectors.begin(), mCollectors.end(), info), mCollectors.end());

			for (CollectorInfo *i : mCollectors) {
				if (i->mBaseIndex >= info->mBaseIndex)
					i->mBaseIndex -= info->mComponents->size();
			}

			std::vector<F> clearV{};
			mUpdate.emit(info, false, clearV);
			info->mBaseIndex = -1;
		}

		static SignalSlot::SignalStub<CollectorInfo*, bool, const std::vector<F>&> &update() {
			return sInstance.mUpdate;
		}

	private:
		std::vector<F> mComponents;
		SignalSlot::Signal<CollectorInfo*, bool, const std::vector<F>&> mUpdate;
		std::vector<CollectorInfo*> mCollectors;
	};


	template <class _Base, class _Ty>
	inline DLL_EXPORT UniqueComponentRegistry<_Base, _Ty> UniqueComponentRegistry<_Base, _Ty>::sInstance{};

}

#else

namespace Engine {

	template <class _Base, class _Ty>
	struct UniqueComponentRegistry {

		typedef _Base Base;
		typedef _Ty Ty;
		typedef Collector_F<Base, _Ty> F;

		static std::vector<F> sComponents();

	};

}
#endif