#pragma once
/*
#include "keyvalue.h"

namespace Engine {
	
	template <class traits>
	class ListenerContainer {
	public:

		struct iterator {
			iterator(ListenerContainer &cont, traits::const_iterator it) :
				mCont(cont),
				mIt(it)
			{
				assert(!cont.mCurrentIterator);
				cont.mCurrentIterator = this;
			}
			~iterator() {
				assert(mCont.mCurrentIterator == this);
				mCont.mCurrentIterator = nullptr;
			}
			typename traits::const_iterator mIt;
			ListenerContainer &mCont;
		};

		void add(traits::value_type listener) 
		{
			if (mCurrentIterator) {
				auto keep = traits::keepIterator(mData.begin(), mCurrentIterator->mIt);
				auto newIt = traits::emplace(mData, mData.end(), std::forward<traits::value_type>(listener));
				mCurrentIterator->mIt = traits::revalidateIteratorInsert(mData.begin(), keep, newIt);
			}
			else {
				traits::emplace(mData, mData.end(), std::forward<traits::value_type>(listener));
			}
		}

		void remove(traits::value_type listener) 
		{
			mData.erase(kvFind(mData, listener));
		}

		iterator begin() const 
		{
			return { *this, mData.begin() };
		}

		iterator end() const 
		{
			return { *this, mData.end() };
		}

	private:
		typename traits::container mData;
		typename iterator *mCurrentIterator = nullptr;
	};
	
}
*/