#pragma once

#include "creationhelper.h"
#include "observablecontainer.h"
#include "unithelper.h"
#include "generic/keyvalue.h"

namespace Engine {
	namespace Serialize {

		template<class _Ty = void>
		struct KeyCompare
		{	// functor for operator<
			typedef _Ty first_argument_type;
			typedef _Ty second_argument_type;
			typedef bool result_type;

			constexpr bool operator()(const _Ty& _Left, const _Ty& _Right) const
			{	// apply operator< to operands
				return (kvKey(_Left) < kvKey(_Right));
			}
		};

		template <class T>
		class SetConstIterator;

		template <class T>
		class SetIterator : public std::iterator<typename std::iterator_traits<typename std::set<T>::iterator>::iterator_category, T> {
		public:
			typedef typename std::set<T>::iterator It;

			SetIterator() {}
			SetIterator(It&& it) :
				mIterator(std::forward<It>(it)) {}
			SetIterator(const It &it) :
				mIterator(it) {}

			T &operator *() const {
				return const_cast<T&>(*mIterator);
			}

			T *operator ->() const {
				return &const_cast<T&>(*mIterator);
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

			void operator--() {
				--mIterator;
			}

			operator const It &() const {
				return mIterator;
			}

		private:
			friend class SetConstIterator<T>;

			It mIterator;
		};

		template <class T>
		class SetConstIterator : public std::iterator<typename std::iterator_traits<typename std::set<T>::iterator>::iterator_category, T> {
		public:
			typedef typename std::set<T>::const_iterator It;

			SetConstIterator(It &&it) :
				mIterator(std::forward<It>(it)) {}
			SetConstIterator(const It &it) :
				mIterator(it) {}
			SetConstIterator(const SetIterator<T> &it) :
				mIterator(it.mIterator) {}

			const T &operator *() const {
				return *mIterator;
			}

			const T *operator ->() const {
				return &*mIterator;
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

			void operator--() {
				--mIterator;
			}

			operator const It &() const {
				return mIterator;
			}

		private:

			It mIterator;
		};


		template <class T>
		struct container_traits<std::set, T> {
			static constexpr const bool sorted = true;

			typedef std::set<T, KeyCompare<T>> container;
			typedef SetIterator<T> iterator;
			typedef SetConstIterator<T> const_iterator;
			typedef typename KeyType<T>::type key_type;
			typedef typename container::value_type value_type;
			typedef T type;

			template <class... _Ty>
			static iterator insert(container &c, const const_iterator &where, _Ty&&... args) {
				return c.emplace(std::forward<_Ty>(args)...).first;
			}

		};

		template <class C>
		class SetImpl : public C {
		public:

			typedef typename C::Type Type;

			typedef typename C::iterator iterator;
			typedef typename C::const_iterator const_iterator;

			using C::C;


			template <class... _Ty>
			iterator emplace(_Ty&&... args) {
				return this->insert(this->end(), std::forward<_Ty>(args)...);
			}

		};

		template <class C>
		using ObservableSetImpl = SetImpl<C>;

		template <class T, class Creator = DefaultCreator<>>
		using SerializableSet = SetImpl<SerializableContainer<container_traits<std::set, typename UnitHelper<T>::Type>, Creator>>;

		template <class T, const _ContainerPolicy &Config, class Creator = DefaultCreator<>>
		using ObservableSet = ObservableSetImpl<ObservableContainer<container_traits<std::set, typename UnitHelper<T>::Type>, Creator, Config>>;





	}
}


