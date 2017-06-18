#pragma once

#include "creationhelper.h"
#include "observablecontainer.h"
#include "unithelper.h"

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
		struct KeyHelper<std::unique_ptr<T>> {
			typedef typename KeyHelper<T>::KeyType KeyType;

			static KeyType key(const std::unique_ptr<T> &t) {
				return KeyHelper<T>::key(*t);
			}
		};
			

		template <class T>
		class KeyHolder {
		public:
			typedef typename KeyHelper<T>::KeyType KeyType;

			template <class... _Ty>
			KeyHolder(_Ty&&... args) :
				mItem(std::forward<_Ty>(args)...),
				mKey(KeyHelper<T>::key(mItem))
			{
			}

			const KeyType &key() const {
				return mKey;
			}

			operator T &() const {
				return mItem;
			}

			bool operator < (const KeyHolder<T> &other) const {
				return mKey < other.mKey;
			}

			bool operator == (const T &other) const {
				return mKey == KeyHelper<T>::key(other);
			}

			mutable T mItem;
		private:
			KeyType mKey;			
		};

		template <class T>
		class SetConstIterator;

		template <class T>
		class SetIterator : public std::iterator<typename std::iterator_traits<typename std::set<T>::iterator>::iterator_category, T>{
		public:
			typedef typename std::set<KeyHolder<T>>::iterator It;
			typedef typename KeyHolder<T>::KeyType KeyType;

			SetIterator() {}
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

			const KeyType &key() const {
				return mIterator->key();
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
			friend class SetConstIterator<T>;
			friend class BaseContainer<std::set<T>>;

			It mIterator;
		};

		template <class T>
		class SetConstIterator : public std::iterator<typename std::iterator_traits<typename std::set<T>::iterator>::iterator_category, T> {
		public:
			typedef typename std::set<KeyHolder<T>>::const_iterator It;
			typedef typename KeyHolder<T>::KeyType KeyType;

			SetConstIterator(It &&it) :
				mIterator(std::forward<It>(it)) {}
			SetConstIterator(const It &it) :
				mIterator(it) {}
			SetConstIterator(const SetIterator<T> &it) :
				mIterator(it.mIterator) {}

			const T &operator *() const {
				return mIterator->mItem;
			}

			const T *operator ->() const {
				return &mIterator->mItem;
			}

			const KeyType &key() const {
				return mIterator->key();
			}

			bool operator !=(const SetConstIterator<T> &other) const {
				return mIterator != other.mIterator;
			}
			bool operator ==(const SetConstIterator<T> &other) const {
				return mIterator == other.mIterator;
			}
			

			void operator++() {
				++mIterator;
			}

		private:
			friend class BaseContainer<std::set<T>>;

			It mIterator;
		};

		template <class T>
		class BaseContainer<std::set<T>> : protected UnitHelper<T> {
		public:

			static constexpr const bool sorted = true;
		//protected:
			typedef typename UnitHelper<T>::Type Type;
			typedef std::set<KeyHolder<Type>> NativeContainerType;
			typedef SetIterator<Type> iterator;
			typedef SetConstIterator<Type> const_iterator;
			typedef typename KeyHolder<T>::KeyType KeyType;


			BaseContainer()
			{
			}

			BaseContainer(const NativeContainerType &c) :
				mData(c)
			{
			}

			template <class... _Ty>
			iterator insert(const const_iterator &where, _Ty&&... args) {
				iterator it = mData.emplace_hint(where.mIterator, std::forward<_Ty>(args)...);
				this->postConstruct(*it);
				return it;
			}

			iterator erase(const const_iterator &it) {
				return mData.erase(it.mIterator);
			}

			const KeyType &key(const const_iterator &where) const {
				return where.key();
			}


			iterator find(const Type &item) {
				return std::find(mData.begin(), mData.end(), item);
			}

			const_iterator find(const Type &item) const {
				return std::find(mData.begin(), mData.end(), item);
			}

			bool contains(const Type &item) const {
				return find(item) != mData.end();
			}

			iterator find(const KeyType &key) {
				for (iterator it = mData.begin(); it != mData.end(); ++it) {
					if (it.key() == key)
						return it; ////TODO  O(log(n))
				}
				return mData.end();
			}

			bool contains(const KeyType &key) {
				return find(key) != mData.end();
			}


		protected:
			NativeContainerType mData;

		};


		template <class T, class Creator>
		class SerializableSetImpl : public SerializableContainer<std::set<T>, Creator> {
		public:
			using SerializableContainer<std::set<T>, Creator>::SerializableContainer;

			typedef typename KeyHolder<T>::KeyType KeyType;

			typedef typename SerializableContainer<std::set<T>, Creator>::iterator iterator;
			typedef typename SerializableContainer<std::set<T>, Creator>::const_iterator const_iterator;
		};

		template <class T, class Creator, const _ContainerPolicy &Config>
		class ObservableSetImpl : public ObservableContainer<SerializableSetImpl<T, Creator>, Config> {
		public:
			using ObservableContainer<SerializableSetImpl<T, Creator>, Config>::ObservableContainer;

			typedef typename ObservableContainer<SerializableSetImpl<T, Creator>, Config>::iterator iterator;
			typedef typename ObservableContainer<SerializableSetImpl<T, Creator>, Config>::const_iterator const_iterator;

		};

		template <class C>
		class SetImpl : public C {
		public:

			typedef typename C::Type Type;
			typedef typename C::KeyType KeyType;

			typedef typename C::iterator iterator;
			typedef typename C::const_iterator const_iterator;

			using C::C;


			template <class... _Ty>
			iterator emplace(_Ty&&... args) {
				return this->insert(this->end(), std::forward<_Ty>(args)...);
			}

		};

		
		template <class T, class Creator = DefaultCreator<>>
		using SerializableSet = SetImpl<SerializableSetImpl<T, Creator>>;

		template <class T, const _ContainerPolicy &Config, class Creator = DefaultCreator<>>
		using ObservableSet = SetImpl<ObservableSetImpl<T, Creator, Config>>;





	}
}


