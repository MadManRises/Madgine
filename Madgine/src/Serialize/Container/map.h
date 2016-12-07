#pragma once

#include "../observable.h"
#include "../serializable.h"
#include "container.h"

namespace Engine {
	namespace Serialize {




		template <class T, class... Args>
		class SerializableMap : public Serializable, protected Container<T, Args...> {
		public:
			typedef std::map<std::string, T> MapType;
			typedef typename MapType::iterator iterator;
			typedef typename MapType::const_iterator const_iterator;

			
			SerializableMap() {}
			SerializableMap(Serialize::SerializableUnit *parent) :
				Serializable(parent) {}			
			template <class P, class... _Ty>
			SerializableMap(P *parent, ArgsTuple(P::*factory)(_Ty...)) :
				Serializable(parent),
				KeyContainer(parent, factory) {}
			virtual ~SerializableMap() = default;

			virtual void writeState(SerializeOutStream &out) const override {
				for (const std::pair<const std::string, T> &p : mData) {
					if (filter(out, p.second)) {
						out << p.first;
						write(out, p.second);
					}
				}

				out << ValueType();
			}

			virtual void readState(SerializeInStream &in) override {
				std::string key;

				while (in.loopRead(key)) {
					loadItem(key, in);
				}
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

			virtual T &operator[](const std::string &key) {
				return mData[key];
			}

			bool contains(const std::string &key) const {
				return mData.find(key) != mData.end();
			}

			const T &at(const std::string &key) const {
				return mData.at(key);
			}

			virtual void clear() {
				mData.clear();
			}

			template <class... _Ty>
			T &emplace(const std::string &key, _Ty&&... args) {
				return insert(key, std::forward<_Ty>(args)...)->second;
			}

			iterator find(const std::string &key) {
				return mData.find(key);
			}

			iterator erase(const iterator &where) {
				return mData.erase(where);//TODO
			}

		protected:
			void loadItem(SerializeInStream &in) {
				std::string key;
				in >> key;
				loadItem(key, in);
			}

			void loadItem(const std::string &key, SerializeInStream &in) {
				iterator it = insertTuple(key, readCreationData(in));
				read(in, it->second);
			}

			iterator insertTuple(const std::string &key, ArgsTuple&& tuple) {
				return TupleUnpacker<const std::string&>::call(this, &SerializableMap::insert<Args...>, key, std::forward<ArgsTuple>(tuple));
			}

			template <class... _Ty>
			iterator insert(const std::string &key, _Ty&&... args) {
				iterator it = mData.lower_bound(key);
				if (it != mData.end() && it->first == key)
					it = mData.erase(it);
				return insert(it, key, std::forward<_Ty>(args)...);
			}

			template <class... _Ty>
			iterator insert(const iterator &where, const std::string &key, _Ty&&... args) {
				if (where != mData.end() && where->first == key)
					throw 0;
				iterator it = mData.try_emplace(where, key, std::forward<_Ty>(args)...);
				onInsert(it);
				return it;
			}

			virtual void onInsert(const iterator &where) {}

			std::map<std::string, T> mData;

		};

		template <class T, class... Args>
		class ObservableMap : public SerializableMap<T, Args...>, public Observable {
		public:

			ObservableMap(Serialize::SerializableUnit *parent) :
				SerializableMap(parent),
				Observable(parent) {}
			template <class P, class... _Ty>
			ObservableMap(P *parent, ArgsTuple(P::*factory)(_Ty...)) :
				SerializableMap(parent, factory),
				Observable(parent) {}


			virtual T &operator[](const std::string &key) override {
				iterator it = mData.lower_bound(key);
				if (it != mData.end() && it->first == key) {
					return it->second;
				}
				else {
					return insert(it, key)->second;
				}
			}

			virtual void clear() override {
				for (SerializeOutStream *out : getMessageTargets(true)) {
					*out << CLEAR;
				}

				SerializableMap::clear();
			}


			// Inherited via Observable
			virtual void readChanges(SerializeInStream & in) override
			{
				Operations op;
				in >> (int&)op;
				switch (op) {
				case ADD_ITEM:
					loadItem(in);
					break;
				case CLEAR:
					clear();
					break;
				default:
					throw 0;
				}
			}

		private:

			virtual void onInsert(const iterator &it) override {

				for (SerializeOutStream *out : getMessageTargets(true)) {
					*out << ADD_ITEM << it->first;
					write(*out, it->second);
				}
			}

		};


	}
}


