#pragma once

#include "../streams/serializestream.h"
#include "generic/templates.h"
#include "unithelper.h"

namespace Engine {
	namespace Serialize {

		template <class traits, class Creator>
		class Container : public Serializable, protected UnitHelper<typename traits::type>, public Creator {
		public:

			//protected:
			typedef typename traits::container NativeContainerType;
			typedef typename traits::iterator iterator;
			typedef typename traits::const_iterator const_iterator;

			typedef typename traits::type Type;

			Container() :
				mLocallyActiveIterator(mData.begin())
			{
			}

			Container(const NativeContainerType &c) :
				mData(c),
				mLocallyActiveIterator(mData.begin())
			{
			}

			Container(const Container &other) :
				mData(other.mData),
				mLocallyActiveIterator(mData.begin()) {
			}

			Container(Container &&other) :
				mData(std::forward<decltype(mData)>(other.mData)),
				mLocallyActiveIterator(mData.begin()) {
			}

			const_iterator begin() const {
				return mData.begin();
			}

			const_iterator end() const {
				return mData.end();
			}

			iterator begin() {
				return mData.begin();
			}

			iterator end() {
				return mData.end();
			}

			const NativeContainerType &data() const {
				return mData;
			}

			Container<traits, Creator> &operator=(const NativeContainerType &other) {
				bool wasActive = false;
				if (isActive()) {
					setActiveFlag(false);	
				}
				if (isLocallyActive()) {
					notifySetActive(false);
					wasActive = true;
				}
				mData = other;
				mLocallyActiveIterator = mData.begin();
				if (isActive()) {
					setActiveFlag(true);
				}
				if (wasActive)
					notifySetActive(true);
				return *this;
			}

			void clear() {
				bool wasActive = false;
				if (isActive()) {
					setActiveFlag(false);
				}
				if (isLocallyActive()) {
					notifySetActive(false);
					wasActive = true;
				}
				mData.clear();
				mLocallyActiveIterator = mData.begin();
				if (isActive()) {
					setActiveFlag(true);
				}
				if (wasActive)
					notifySetActive(true);

			}

			iterator erase(const iterator &it) {
				if (isActive()) {
					this->setItemActiveFlag(*it, false);
				}
				if (isItemLocallyActive(it))
					this->notifySetItemActive(*it, false);
				return erase_intern(it);
			}

			size_t size() const {
				return this->mData.size();
			}

			bool empty() const {
				return this->mData.empty();
			}

			virtual void writeState(SerializeOutStream &out) const override {
				for (auto it = begin(); it != end(); ++it) {
					const auto &t = *it;
					if (this->filter(out, t)) {
						write_item(out, t);
					}
				}
				out << ValueType::EOL();
			}

			virtual void readState(SerializeInStream &in) override {
				bool wasActive = false;
				if (isActive()) {
					setActiveFlag(false);
				}
				if (isLocallyActive()) {
					notifySetActive(false);
					wasActive = true;
				}
				mData.clear();
				mLocallyActiveIterator = mData.begin();
				while (in.loopRead()) {
					this->read_item_where_intern(end(), in);
				}
				if (isActive()) {
					setActiveFlag(true);
				}
				if (wasActive)
					notifySetActive(true);
			}

			virtual void applySerializableMap(const std::map<size_t, SerializableUnitBase*> &map) override {
				for (auto it = begin(); it != end(); ++it) {
					this->applyMap(map, *it);
				}
			}

			virtual void setActiveFlag(bool b) override {
				for (auto it = begin(); it != end(); ++it) {
					this->setItemActiveFlag(*it, b);
				}
			}

			virtual void notifySetActive(bool active) override {
				if (!active) {
					while (mLocallyActiveIterator != begin()) {
						--mLocallyActiveIterator;
						this->notifySetItemActive(*mLocallyActiveIterator, active);
					}
				}
				Serializable::notifySetActive(active);
				if (active) {
					for (mLocallyActiveIterator = begin(); mLocallyActiveIterator != end();) {
						Type &t = *mLocallyActiveIterator;
						++mLocallyActiveIterator;
						this->notifySetItemActive(t, active);
					}
				}
			}

			template <class... _Ty>
			std::pair<iterator, bool> emplace(const const_iterator &where, _Ty&&... args) {
				std::pair<iterator, bool> it = emplace_intern(where, std::forward<_Ty>(args)...);
				if (it.second) {
					if (isActive()) {
						this->setItemActiveFlag(*it.first, true);
					}
					if (isItemLocallyActive(it.first))
						this->notifySetItemActive(*it.first, true);
				}
				return it;
			}

			template <class T, class... _Ty>
			std::pair<iterator, bool> emplace_init(T &&init, const const_iterator &where, _Ty&&... args) {
				std::pair<iterator, bool> it = emplace_intern(where, std::forward<_Ty>(args)...);
				if (it.second) {
					init(*it.first);
					if (isActive()) {
						this->setItemActiveFlag(*it.first, true);
					}
					if (isItemLocallyActive(it.first))
						this->notifySetItemActive(*it.first, true);
				}
				return it;
			}


			template <class... _Ty>
			std::pair<iterator, bool> emplace_tuple(const const_iterator &where, std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<const const_iterator &>::call(static_cast<Container<traits, Creator>*>(this), &Container<traits, Creator>::template emplace<_Ty...>, where, std::forward<std::tuple<_Ty...>>(tuple));
			}

		protected:
			/*std::pair<iterator, bool> read_item_where(const const_iterator &where, SerializeInStream &in) {
				std::pair<iterator, bool> it = read_item_where_intern(where, in);
				if (isActive()) {
					this->setItemActiveFlag(*it.first, true);
				}
				if (isItemLocallyActive(it.first))
					this->notifySetItemActive(*it.first, true);
				return it;
			}*/

			std::pair<iterator, bool> read_item_where_intern(const const_iterator &where, SerializeInStream &in) {
				std::pair<iterator, bool> it = emplace_tuple_intern(where, this->readCreationData(in));
				assert(it.second);
				this->read_state(in, *it.first);
				if (!in.isMaster()) {
					this->read_id(in, *it.first);
				}
				return it;
			}

			void write_item(SerializeOutStream &out, const Type &t) const {
				this->write_creation(out, t);
				this->write_state(out, t);
				if (out.isMaster())
					this->write_id(out, t);
			}

			NativeContainerType mData;
			iterator mLocallyActiveIterator;

			bool isItemLocallyActive(const iterator &it) {
				if (!isLocallyActive())
					return false;
				if (mLocallyActiveIterator == end())
					return true;
				for (auto it2 = begin(); it2 != mLocallyActiveIterator; ++it2) {
					if (it2 == it)
						return true;
				}
				return false;
			}

			template <class... _Ty>
			std::pair<iterator, bool> emplace_intern(const const_iterator &where, _Ty&&... args) {
				auto keep = traits::keepIterator(begin(), mLocallyActiveIterator);
				std::pair<iterator, bool> it = traits::emplace(mData, where, std::forward<_Ty>(args)...);
				if (it.second) {
					mLocallyActiveIterator = traits::revalidateIteratorInsert(begin(), keep, it.first);
					this->postConstruct(*it.first, this);
				}
				return it;
			}

			template <class... _Ty>
			std::pair<iterator, bool> emplace_tuple_intern(const const_iterator &where, std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<const const_iterator &>::call(static_cast<Container<traits, Creator>*>(this), &Container<traits, Creator>::template emplace_intern<_Ty...>, where, std::forward<std::tuple<_Ty...>>(tuple));
			}

			iterator erase_intern(const iterator &it) {
				auto keep = traits::keepIterator(begin(), mLocallyActiveIterator);
				iterator newIt = mData.erase(it);
				mLocallyActiveIterator = traits::revalidateIteratorRemove(begin(), keep, newIt);
				return newIt; 
			}



		};


	}
}
