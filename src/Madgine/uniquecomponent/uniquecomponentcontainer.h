#pragma once

#include "uniquecomponentregistry.h"

#include "Interfaces/generic/container_traits.h"

#include "Interfaces/signalslot/slot.h"

#include "uniquecomponent.h"

namespace Engine
{

	template <class _Base, class _Ty, template <class...> class Container>
	class UniqueComponentContainer
	{
	public:
		typedef UniqueComponentRegistry<_Base, _Ty> Registry;
		typedef typename Registry::F F;
		typedef typename Registry::Base Base;

		typedef typename Container<std::unique_ptr<Base>>::const_iterator const_iterator;

		UniqueComponentContainer(_Ty arg)
#ifndef STATIC_BUILD
			: mUpdateSlot(this)
			, mArg(arg)
#endif
		{
			size_t count = Registry::sComponents().size();
			mSortedComponents.reserve(count);
			if constexpr (std::is_same_v<Container<F>, std::vector<F>>) {
				mComponents.reserve(count);
			}
			for (auto f : Registry::sComponents())
			{
				std::unique_ptr<Base> p = f(arg);
				mSortedComponents.push_back(p.get());
				container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, mComponents.end(), std::move(p));
			}
#ifndef STATIC_BUILD
			Registry::update().connect(mUpdateSlot);
#endif

		}

		UniqueComponentContainer(const UniqueComponentContainer&) = delete;
		void operator=(const UniqueComponentContainer&) = delete;

		const_iterator begin() const
		{
			return mComponents.begin();
		}

		const_iterator end() const
		{
			return mComponents.end();
		}

		size_t size()
		{
			return mComponents.size();
		}

		const Container<std::unique_ptr<Base>>& data() const
		{
			return mComponents;
		}

		template <class T>
		T &get()
		{
			return static_cast<T&>(get(component_index<T>()));
		}

		Base &get(size_t i)
		{
			return *mSortedComponents[i];
		}



	private:
		Container<std::unique_ptr<Base>> mComponents;
		std::vector<Base*> mSortedComponents;

#ifndef STATIC_BUILD

	protected:	
		void updateComponents(CollectorInfo *info, bool add, const std::vector<F> &vals) {
			if (add){
				assert(mComponents.size() == info->mBaseIndex);
				mSortedComponents.reserve(info->mBaseIndex + vals.size());
				if constexpr (std::is_same_v<Container<F>, std::vector<F>>) {
					mComponents.reserve(info->mBaseIndex + vals.size());
				}
				size_t i = 0;
				for (F f : vals) {
					std::unique_ptr<Base> p = f(mArg);
					mSortedComponents.push_back(p.get());
					container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, mComponents.end(), std::move(p));
					++i;
				}
			}
			else {
				mSortedComponents.erase(mSortedComponents.begin() + info->mBaseIndex, mSortedComponents.begin() + info->mBaseIndex + info->mElementInfos.size());
				mComponents.erase(std::next(mComponents.begin(), info->mBaseIndex), std::next(mComponents.begin(), info->mBaseIndex + info->mElementInfos.size()));
			}
		}

	private:
		SignalSlot::Slot<&UniqueComponentContainer<Base, _Ty, Container>::updateComponents> mUpdateSlot;
		_Ty mArg;

#endif
	};

}