#pragma once

#include "uniquecomponentregistry.h"

#include "../threading/slot.h"

namespace Engine
{

	template <class _Base, class _Ty>
	class UniqueComponentSelector
	{
	public:
		typedef UniqueComponentRegistry<_Base, _Ty> Registry;
		typedef typename Registry::F F;
		typedef typename Registry::Base Base;

		UniqueComponentSelector(_Ty arg, int index = 0) :
			mIndex(index)
#ifndef STATIC_BUILD
			, mUpdateSlot(this)
			, mArg(arg)
#endif
		{
			if (index >= 0){
				if (Registry::sComponents().size() > index) 				
					mValue = Registry::sComponents().at(index)(arg);
				else
					mIndex = -1;
			}
#ifndef STATIC_BUILD
			Registry::update().connect(mUpdateSlot);
#endif
		}

		UniqueComponentSelector(const UniqueComponentSelector&) = delete;
		void operator=(const UniqueComponentSelector&) = delete;

		Base *operator->() const
		{
			return mValue.get();
		}

		operator Base *() const
		{
			return mValue.get();
		}

		Base *get() const 
		{
			return mValue.get();
		}

	private:
		std::unique_ptr<Base> mValue;
		int mIndex;

#ifndef STATIC_BUILD

	protected:
		void updateComponents(CollectorInfo *info, bool add, const std::vector<F> &vals) {
			if (add) {
				if (mIndex < 0 && vals.size() > 0) {
					mValue = vals[0](mArg);
					mIndex = 0;
				}
			}
			else {
				mValue.reset();
				mIndex = -1;
			}
		}

	private:
		Threading::Slot<&UniqueComponentSelector<Base, _Ty>::updateComponents> mUpdateSlot;
		_Ty mArg;

#endif
	};

}