#pragma once

#include "../observable.h"
#include "../serializable.h"
#include "container.h"

namespace Engine {
	namespace Serialize {

		template <class T>
		class Keyed {
		public:
			typedef T KeyType;

			virtual KeyType key() const = 0;
		};

		template <class T>
		struct KeyHelper {
			typedef typename T::KeyType KeyType;

			static KeyType key(const T &t) {
				return t.key();
			}
		};

		template <class T>
		struct KeyHelper<Ogre::unique_ptr<T>> {
			typedef typename KeyHelper<T>::KeyType KeyType;

			static KeyType key(const Ogre::unique_ptr<T> &t) {
				return KeyHelper<T>::key(*t);
			}
		};
			

		template <class T>
		class KeyHolder {
		public:
			typedef typename KeyHelper<T>::KeyType KeyType;

			template <class... _Ty>
			KeyHolder(_Ty&&... args) :
				mItem(std::forward<_Ty>(args)...){
				mKey = KeyHelper<T>::key(mItem);
			}

			const typename KeyType &key() const {
				return mKey;
			}

			operator T &() const {
				return mItem;
			}

			bool operator < (const KeyHolder<T> &other) const {
				return mKey < other.mKey;
			}

			mutable T mItem;
		private:
			KeyType mKey;			
		};

		template <class T>
		class SetIterator;
		template <class T>
		class SetConstIterator;

		template <class T, class... Args>
		class SerializableSet : public Serializable, protected Container<T, Args...> {
		public:
			typedef std::set<KeyHolder<T>> SetType;
			typedef typename KeyHolder<T>::KeyType KeyType;
			typedef typename SetIterator<T> iterator;
			typedef typename SetConstIterator<T> const_iterator;
			

			
			SerializableSet() {}
			SerializableSet(Serialize::SerializableUnit *parent) :
				Serializable(parent) {}			
			template <class P, class... _Ty>
			SerializableSet(P *parent, ArgsTuple(P::*factory)(_Ty...)) :
				Serializable(parent),
				Container(parent, factory) {}
			virtual ~SerializableSet() = default;

			virtual void writeState(SerializeOutStream &out) const override {
				for (const T &t : mData) {
					if (filter(out, t)) { 						
						write(out, t);
					}
				}

				out << ValueType();
			}

			virtual void readState(SerializeInStream &in) override {

				while (in.loopRead()) {
					loadItem(in);
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

			bool contains(const T &item) const {
				return mData.find(item) != mData.end();
			}

			bool contains(const KeyType &key) const {
				return std::find_if(mData.begin(), mData.end(), [&](const KeyHolder<T> &holder) {return holder.key() == key; }) != mData.end();
			}

			iterator find(const T &item) const {
				return find(KeyHelper<T>::key(item));
			}

			iterator find(const KeyType &key) const {
				return std::find_if(mData.begin(), mData.end(), [&](const KeyHolder<T> &holder) {return holder.key() == key; });
			}

			virtual void clear() {
				mData.clear();
			}

			template <class... _Ty>
			T &emplace(_Ty&&... args) {
				return *insert(std::forward<_Ty>(args)...);
			}

			iterator erase(const iterator &where) {
				return mData.erase(where.mIterator);//TODO
			}

		protected:
			typedef typename SetType::iterator intern_iterator;

			void loadItem(SerializeInStream &in) {
				iterator it = insertTuple(readCreationData(in));
				read(in, *it);
			}

			iterator insertTuple(ArgsTuple&& tuple) {
				return TupleUnpacker<>::call(this, &SerializableSet::insert<Args...>, std::forward<ArgsTuple>(tuple));
			}

			template <class... _Ty>
			iterator insert(_Ty&&... args) {
				KeyHolder<T> item(std::forward<_Ty>(args)...);
				auto it = mData.lower_bound(item);
				if (it != mData.end() && it->key() == item.key()) {
					onInsert(it);
					return it;
				}
				return insert_hint(it, std::move(item));
			}

			template <class... _Ty>
			iterator insert_hint(const intern_iterator &where, _Ty&&... args) {
				iterator it = mData.emplace_hint(where, std::forward<_Ty>(args)...);
				onInsert(it);
				return it;
			}

			virtual void onInsert(const iterator &where) {}

			SetType mData;

		};

		template <class T, class... Args>
		class ObservableSet : public SerializableSet<T, Args...>, public Observable {
		public:

			ObservableSet(Serialize::SerializableUnit *parent) :
				SerializableSet(parent),
				Observable(parent) {}
			template <class P, class... _Ty>
			ObservableSet(P *parent, ArgsTuple(P::*factory)(_Ty...)) :
				SerializableSet(parent, factory),
				Observable(parent) {}

			virtual void clear() override {
				for (SerializeOutStream *out : getMessageTargets(true)) {
					*out << CLEAR;
				}

				SerializableSet::clear();
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
					*out << ADD_ITEM;
					write(*out, *it);
				}
			}

		};

		template <class T>
		class SetIterator {
		public:
			typedef typename std::set<KeyHolder<T>>::iterator It;

			SetIterator(It&& it) :
				mIterator(std::forward<It>(it)) {}
			SetIterator(const It &it) :
				mIterator(it) {}

			T &operator *() const {
				return mIterator->mItem;
			}

			T *operator ->() const {
				return &mIterator->mItem;
			}
			bool operator !=(const SetIterator<T> &other) const {
				return mIterator != other.mIterator;
			}
			bool operator ==(const SetIterator<T> &other) const {
				return mIterator == other.mIterator;
			}

			void operator++() {
				++mIterator;
			}

		private:
			template <class T, class... Args>
			friend class SerializableSet;

			It mIterator;
		};

		template <class T>
		class SetConstIterator {
		public:
			typedef typename std::set<KeyHolder<T>>::const_iterator It;

			SetConstIterator(It &&it) :
				mIterator(std::forward<It>(it)) {}
			SetConstIterator(const It &it) :
				mIterator(it) {}

		private:
			It mIterator;
		};



	}
}


