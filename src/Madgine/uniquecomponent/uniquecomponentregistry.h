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
		std::vector<Collector_F<void, void*>> mComponents;
		const TypeInfo *mRegistryInfo;
		const TypeInfo *mBaseInfo;
		const Plugins::BinaryInfo *mBinary;
		std::vector<const TypeInfo*> mElementInfos;
		size_t mBaseIndex = 0;
	};

	MADGINE_BASE_EXPORT void exportStaticComponentHeader(const Filesystem::Path &outFile, std::vector<const TypeInfo*> skip = {});
		
	MADGINE_BASE_EXPORT std::map<std::string, ComponentRegistryBase *> &registryRegistry();

	struct MADGINE_BASE_EXPORT ComponentRegistryBase {
		ComponentRegistryBase(const TypeInfo *ti, const Plugins::BinaryInfo *binary) : mTi(ti), mBinary(binary){
			registryRegistry()[ti->mFullName] = this;
		}

		~ComponentRegistryBase() {
			registryRegistry().erase(mTi->mFullName);
		}

		virtual void onPluginLoad(const Plugins::BinaryInfo *) = 0;
		virtual void onPluginUnload(const Plugins::BinaryInfo *) = 0;

		const TypeInfo *type_info() {
			return mTi;
		}

		std::vector<CollectorInfo*>::iterator begin()
		{
			return mLoadedCollectors.begin();
		}

		std::vector<CollectorInfo*>::iterator end()	
		{
			return mLoadedCollectors.end();
		}

		const Plugins::BinaryInfo *mBinary;

	protected:
		std::vector<CollectorInfo*> mLoadedCollectors;

	private:
		const TypeInfo *mTi;
	};

	template <class _Base, class _Ty>
	struct DLL_EXPORT UniqueComponentRegistry : ComponentRegistryBase {

		typedef _Base Base;
		typedef _Ty Ty;
		typedef Collector_F<Base, Ty> F;

		UniqueComponentRegistry() :
			ComponentRegistryBase(&typeInfo<UniqueComponentRegistry>(), &Plugins::PLUGIN_LOCAL(binaryInfo)) {}

		static UniqueComponentRegistry &sInstance();
		static std::vector<F> &sComponents() {
			return sInstance().mComponents;
		}

		void addCollector(CollectorInfo *info) {
			mUnloadedCollectors.push_back(info);
		}

		void onPluginLoad(const Plugins::BinaryInfo *bin){
			for (auto it = mUnloadedCollectors.begin(); it != mUnloadedCollectors.end();)
			{
				CollectorInfo *info = *it;
				if (info->mBinary == bin)
				{
					mLoadedCollectors.push_back(info);
					info->mBaseIndex = mComponents.size();
					const std::vector<F> &comps = reinterpret_cast<const std::vector<F>&>(info->mComponents);
					for (F f : comps) {
						mComponents.push_back(f);
					}
					mUpdate.emit(info, true, comps);
					it = mUnloadedCollectors.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		void removeCollector(CollectorInfo *info) {
			//assert(std::find(mLoadedCollectors.begin(), mLoadedCollectors.end(), info) == mLoadedCollectors.end());
			mUnloadedCollectors.erase(std::remove(mUnloadedCollectors.begin(), mUnloadedCollectors.end(), info), mUnloadedCollectors.end());
		}

		void onPluginUnload(const Plugins::BinaryInfo *bin) {
			for (auto it = mLoadedCollectors.begin(); it != mLoadedCollectors.end();)
			{
				CollectorInfo *info = *it;
				if (info->mBinary == bin)
				{
					mUnloadedCollectors.push_back(info);
					mComponents.erase(mComponents.begin() + info->mBaseIndex, mComponents.begin() + info->mBaseIndex + info->mComponents.size());

					for (CollectorInfo *i : mLoadedCollectors) {
						if (i->mBaseIndex >= info->mBaseIndex)
							i->mBaseIndex -= info->mComponents.size();
					}
					info->mBaseIndex = -1;

					std::vector<F> clearV{};
					mUpdate.emit(info, false, clearV);
					it = mLoadedCollectors.erase(it);
				}
				else
				{
					++it;
				}
			}			
		}

		static SignalSlot::SignalStub<CollectorInfo*, bool, const std::vector<F>&> &update() {
			return sInstance().mUpdate;
		}

	private:
		std::vector<F> mComponents;
		SignalSlot::Signal<CollectorInfo*, bool, const std::vector<F>&> mUpdate;

		std::vector<CollectorInfo*> mUnloadedCollectors;
	};


	template <class _Base, class _Ty>
	inline DLL_EXPORT UniqueComponentRegistry<_Base, _Ty> &UniqueComponentRegistry<_Base, _Ty>::sInstance() {
		static UniqueComponentRegistry dummy;
		return dummy;
	}

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