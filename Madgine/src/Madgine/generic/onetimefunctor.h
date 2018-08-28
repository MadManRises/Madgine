#pragma once

#include "tupleunpacker.h"

namespace Engine
{
	
	template <class T, class... Ty>
	struct OneTimeFunctor
	{
		OneTimeFunctor(void (T::*f)(Ty ...), T* t, Ty&&... args) :
			mT(t),
			mData(std::forward<Ty>(args)...),
			mF(f),
			mCalled(false)
		{
		}

		OneTimeFunctor(OneTimeFunctor<T, Ty...>&& other) noexcept :
			mT(other.mT),
			mData(std::forward<std::tuple<std::remove_reference_t<Ty>...>>(other.mData)),
			mF(other.mF),
			mCalled(other.mCalled)
		{
			other.mCalled = true;
		}

		OneTimeFunctor(const OneTimeFunctor<T, Ty...>& other) :
			mT(other.mT),
			mData(std::forward<std::tuple<std::remove_reference_t<Ty>...>>(other.mData)),
			mF(other.mF),
			mCalled(other.mCalled)
		{
			other.mCalled = true;
		}

		~OneTimeFunctor()
		{
		}

		void operator()()
		{
			assert(!mCalled);
			mCalled = true;
			TupleUnpacker::call(mT, mF, std::move(mData));
		}

	private:
		T* mT;
		void (T::*mF)(Ty ...);
		mutable std::tuple<std::remove_reference_t<Ty>...> mData;		
		mutable bool mCalled;
	};

	template <class T, class... Ty>
	auto oneTimeFunctor(void (T::*f)(Ty ...), T* t, Ty&&... args)
	{
		return OneTimeFunctor<T, Ty...>{f, t, std::forward<Ty>(args)...};
	}

}