#pragma once

#include "../streams/serializestream.h"
#include "container_traits.h"
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

			Container()
			{
			}

			Container(const NativeContainerType &c) :
				mData(c)
			{
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
				mData = other;
				return *this;
			}

			void clear() {
				mData.clear();
			}

			iterator erase(const const_iterator &it) {
				return mData.erase(it);
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
				mData.clear();
				while (in.loopRead()) {
					this->read_item_where(end(), in);
				}
			}

			virtual void applySerializableMap(const std::map<size_t, SerializableUnitBase*> &map) override {
				for (auto it = begin(); it != end(); ++it) {
					this->applyMap(map, *it);
				}
			}

			virtual void setActive(bool b) override {
				for (auto it = begin(); it != end(); ++it) {
					this->setItemActive(*it, b);
				}
			}

		protected:
			template <class... _Ty>
			iterator insert(const const_iterator &where, _Ty&&... args) {
				iterator it = insert_intern(where, std::forward<_Ty>(args)...);
				if (unit() && unit()->isActive()) {
					this->setItemActive(*it, true);
				}
				return it;
			}

			template <class T, class... _Ty>
			iterator insert(T &&init, const const_iterator &where, _Ty&&... args) {
				iterator it = insert_intern(where, std::forward<_Ty>(args)...);
				init(*it);
				if (unit() && unit()->isActive()) {
					this->setItemActive(*it, true);
				}
				return it;
			}

			template <class... _Ty>
			iterator insert_tuple(const const_iterator &where, std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<const const_iterator &>::call(static_cast<Container<traits, Creator>*>(this), &Container<traits, Creator>::template insert<_Ty...>, where, std::forward<std::tuple<_Ty...>>(tuple));
			}


			iterator read_item_where(const const_iterator &where, SerializeInStream &in) {
				iterator it = insert_tuple_intern(where, this->template readCreationData<Type>(in, unit()));
				this->read_state(in, *it);
				if (!in.isMaster()) {
					this->read_id(in, *it);
				}
				if (unit() && unit()->isActive()) {
					this->setItemActive(*it, true);
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

		private:
			template <class... _Ty>
			iterator insert_intern(const const_iterator &where, _Ty&&... args) {
				iterator it = traits::insert(mData, where, std::forward<_Ty>(args)...);
				this->postConstruct(*it);
				return it;
			}

			template <class... _Ty>
			iterator insert_tuple_intern(const const_iterator &where, std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<const const_iterator &>::call(static_cast<Container<traits, Creator>*>(this), &Container<traits, Creator>::template insert_intern<_Ty...>, where, std::forward<std::tuple<_Ty...>>(tuple));
			}

		};


	}
}
