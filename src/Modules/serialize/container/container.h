#pragma once

#include "../streams/serializestream.h"
#include "unithelper.h"
#include "../serializable.h"
#include "../../generic/tupleunpacker.h"

namespace Engine
{
	namespace Serialize
	{
		template <class traits, class Creator>
		class Container : public Serializable, protected UnitHelper<typename traits::type>, public Creator
		{
		public:

			//protected:
			typedef typename traits::container NativeContainerType;
			typedef typename traits::iterator iterator;
			typedef typename traits::const_iterator const_iterator;

			typedef typename traits::type Type;

			Container() :
				mActiveIterator(mData.begin())
			{
			}

			Container(const NativeContainerType& c) :
				mData(c),
				mActiveIterator(mData.begin())
			{
			}

			Container(const Container& other) :
				mData(other.mData),
				mActiveIterator(mData.begin())
			{
			}

			Container(Container&& other)
			{
				if (other.isSynced())
				{
					other.setDataSynced(false);
				}
				mData = std::forward<NativeContainerType>(other.mData);
				for (auto &t : *this)
					this->setParent(t, unit());
				mActiveIterator = other.mActiveIterator;
				other.mData.clear();
				other.mActiveIterator = other.mData.begin();
			}

			const_iterator begin() const
			{
				return mData.begin();
			}

			const_iterator end() const
			{
				return mData.end();
			}

			iterator begin()
			{
				return mData.begin();
			}

			iterator end()
			{
				return mData.end();
			}

			const NativeContainerType& data() const
			{
				return mData;
			}

			Container<traits, Creator>& operator=(const NativeContainerType& other)
			{
				bool wasActive = unsync();
				mData = other;
				mActiveIterator = mData.begin();
				sync(wasActive);
				return *this;
			}

			bool operator==(const Container<traits, Creator>& other) const
			{
				return mData == other.mData;
			}

			void clear()
			{
				bool wasActive = unsync();
				mData.clear();
				mActiveIterator = mData.begin();
				sync(wasActive);
			}

			iterator erase(const iterator& it)
			{
				if (isSynced())
				{
					this->setItemDataSynced(*it, false);
				}
				if (isItemActive(it)) 
				{
					this->setItemActive(*it, false);
				}
				return erase_intern(it);
			}

			iterator erase(const iterator &from, const iterator &to) {
				if (isSynced())
				{
					for (iterator it = from; it != to; ++it) {
						this->setItemDataSynced(*it, false);
					}
				}
				for (iterator it = from; it != to && isItemActive(it); ++it) 
				{
					this->setItemActive(*it, false);
				}
				return erase_intern(from, to);
			}

			size_t size() const
			{
				return this->mData.size();
			}

			bool empty() const
			{
				return this->mData.empty();
			}

			void writeState(SerializeOutStream& out) const override
			{
				for (const auto &t : *this)
				{					
					if (this->filter(out, t))
					{
						write_item(out, t);
					}
				}
				out << EOLType();
			}

			void readState(SerializeInStream& in) override
			{
				bool wasActive = unsync();
				mData.clear();
				mActiveIterator = mData.begin();
				while (in.loopRead())
				{
					this->read_item_where_intern(end(), in);
				}
				sync(wasActive);
			}

			void applySerializableMap(const std::map<size_t, SerializableUnitBase*>& map) override
			{
				for (auto &t : *this)
				{
					this->applyMap(map, t);
				}
			}

			void setDataSynced(bool b) override
			{
				for (auto &t : *this)
				{
					this->setItemDataSynced(t, b);
				}
			}

			void setActive(bool active) override
			{
				if (!active)
				{
					while (mActiveIterator != begin())
					{
						--mActiveIterator;
						this->setItemActive(*mActiveIterator, active);
					}
				}
				Serializable::setActive(active);
				if (active)
				{
					for (mActiveIterator = begin(); mActiveIterator != end();)
					{
						Type& t = *mActiveIterator;
						++mActiveIterator;
						this->setItemActive(t, active);
					}
				}
			}

			template <class... _Ty>
			std::pair<iterator, bool> emplace(const const_iterator& where, _Ty&&... args)
			{
				std::pair<iterator, bool> it = emplace_intern(where, std::forward<_Ty>(args)...);
				if (it.second)
				{
					if (isSynced())
					{
						this->setItemDataSynced(*it.first, true);
					}
					if (isItemActive(it.first))
						this->setItemActive(*it.first, true);
				}
				return it;
			}

			template <class T, class... _Ty>
			std::pair<iterator, bool> emplace_init(T&& init, const const_iterator& where, _Ty&&... args)
			{
				std::pair<iterator, bool> it = emplace_intern(where, std::forward<_Ty>(args)...);
				if (it.second)
				{
					init(*it.first);
					if (isSynced())
					{
						this->setItemDataSynced(*it.first, true);
					}
					if (isItemActive(it.first))
						this->setItemActive(*it.first, true);
				}
				return it;
			}


			template <class... _Ty>
			std::pair<iterator, bool> emplace_tuple(const const_iterator& where, std::tuple<_Ty...>&& tuple)
			{
				return TupleUnpacker::invokeExpand(&Container<traits, Creator>::emplace<_Ty...>,
					static_cast<Container<traits, Creator>*>(this), 
					where,
				    std::forward<std::tuple<_Ty...>>(tuple));
			}

			std::pair<iterator, bool> read_item_where(const const_iterator &where, SerializeInStream &in) {
				std::pair<iterator, bool> it = read_item_where_intern(where, in);
				if (it.second) {
					if (isSynced()) {
						this->setItemDataSynced(*it.first, true);
					}
					if (isItemActive(it.first))
						this->setItemActive(*it.first, true);
				}
				return it;
			}

		protected:
			bool unsync()
			{
				if (isSynced())
				{
					setDataSynced(false);
				}
				if (isActive())
				{
					setActive(false);
					return true;
				}
				return false;
			}

			void sync(bool wasActive)
			{
				if (isSynced())
				{
					setDataSynced(true);
				}
				if (wasActive)
					setActive(true);
			}

			std::pair<iterator, bool> read_item_where_intern(const const_iterator& where, SerializeInStream& in)
			{
				std::pair<iterator, bool> it = emplace_tuple_intern(where, this->readCreationData(in));
				assert(it.second);
				this->read_state(in, *it.first);
				if (!in.isMaster())
					this->read_id(in, *it.first);
				return it;
			}

			void write_item(SerializeOutStream& out, const Type& t) const
			{
				this->write_creation(out, t);
				this->write_state(out, t);
				if (out.isMaster())
					this->write_id(out, t);
			}

			bool isItemActive(const iterator& it)
			{
				if (mActiveIterator == end())
					return true;
				for (auto it2 = begin(); it2 != mActiveIterator; ++it2)
				{
					if (it2 == it)
						return true;
				}
				return false;
			}

			template <class... _Ty>
			std::pair<iterator, bool> emplace_intern(const const_iterator& where, _Ty&&... args)
			{
				auto keep = traits::keepIterator(begin(), mActiveIterator);
				std::pair<iterator, bool> it = traits::emplace(mData, where, std::forward<_Ty>(args)...);
				if (it.second)
				{
					iterator oldActiveIt = traits::revalidateIteratorInsert(begin(), keep, it.first);
					if (std::next(it.first) == oldActiveIt && !isActive())
						mActiveIterator = it.first;
					else
						mActiveIterator = oldActiveIt;
					this->postConstruct(*it.first);
					this->setParent(*it.first, unit());
				}
				return it;
			}

			template <class... _Ty>
			std::pair<iterator, bool> emplace_tuple_intern(const const_iterator& where, std::tuple<_Ty...>&& tuple)
			{
				return TupleUnpacker::invokeExpand(&Container<traits, Creator>::emplace_intern<_Ty...>,
					this,
					where,
					std::forward<std::tuple<_Ty...>>(tuple));
			}

			iterator erase_intern(const iterator& it)
			{
				bool test = mActiveIterator == it;
				auto keep = traits::keepIterator(begin(), mActiveIterator);
				iterator newIt = mData.erase(it);
				if (test)
					mActiveIterator = newIt;
				else
					mActiveIterator = traits::revalidateIteratorRemove(begin(), keep, newIt);
				return newIt;
			}

			iterator erase_intern(const iterator &from, const iterator &to) 
			{
				bool test = false;
				size_t count = 0;
				for (iterator it = from; it != to; ++it) {
					if (mActiveIterator == it)
						test = true;
					if (test)
						++count;
				}
				auto keep = traits::keepIterator(begin(), mActiveIterator);
				iterator newIt = mData.erase(from, to);
				if (test)
					mActiveIterator = newIt;
				else
					mActiveIterator = traits::revalidateIteratorRemove(begin(), keep, newIt, count);
				return newIt;
			}


		protected:
			NativeContainerType mData;
			iterator mActiveIterator;
		};
	}
}
