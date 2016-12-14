#pragma once

#include "templates.h"
#include "valuetype.h"
#include "../serializemanager.h"
#include "../serializable.h"
#include "../observable.h"

namespace Engine {
	namespace Serialize {
		enum Operations {
			ADD_ITEM,
			CLEAR
		};

		template <class T, bool b = std::is_base_of<SerializableUnit, T>::value>
		struct UnitHelper {

			static void read(SerializeInStream &in, T &item) {
				item.readState(in);
			}

			static void write(SerializeOutStream &out, const T &item) {
				item.writeCreationData(out);
				item.writeState(out);
			}

			static bool filter(SerializeOutStream &out, const T &item) {
				return true;
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, T &item) {
				item.applySerializableMap(map);
			}
		};

		template <class T, bool b>
		struct UnitHelper<Ogre::unique_ptr<T>, b> {

			static void read(SerializeInStream &in, Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::read(in, *item);
			}

			static void write(SerializeOutStream &out, const Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::write(out, *item);
			}

			static bool filter(SerializeOutStream &out, const Ogre::unique_ptr<T> &item) {
				return UnitHelper<T>::filter(out, *item);
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, const Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::applyMap(map, *item);
			}
		};

		template <class T>
		struct UnitHelper<T, true> {

			static void read(SerializeInStream &in, T &item) {
				item.readState(in);
				InvPtr id;
				in >> id;
				in.manager().addMapping(id, &item);
				item.setMasterId(id);				
			}

			static void write(SerializeOutStream &out, const T &item) {
				item.writeCreationData(out);
				item.writeState(out);
				out << static_cast<InvPtr>(reinterpret_cast<uintptr_t>(static_cast<const SerializableUnit*>(&item)));
			}

			static bool filter(SerializeOutStream &out, const T &item) {
				return out.manager().filter(&item, out.id());
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, T &item) {
				item.applySerializableMap(map);
			}
		};

		
		template <bool areValueTypes, class... Args>
		class CreationHelper;

		template <class... Args>
		using Creator = CreationHelper<all_of<ValueType::isValueType<Args>::value...>::value, Args...>;


		template <class... Args>
		class CreationHelper<true, Args...> {
		protected:
			typedef std::tuple<Args...> ArgsTuple;

			ArgsTuple defaultCreationData(SerializeInStream &in) {
				ArgsTuple tuple;
				TupleSerializer::readTuple(tuple, in);
				return tuple;
			}
		};

		template <class... Args>
		class CreationHelper<false, Args...> {
		protected:
			typedef std::tuple<Args...> ArgsTuple;
			ArgsTuple defaultCreationData(SerializeInStream &in) {
				throw 0;
			}
		};

		template <class List>
		class ContainerWrapper;

		

		

		template <class T>
		class ContainerWrapper<std::vector<T>> : protected UnitHelper<T> {
		protected:
			typedef typename std::vector<T>::iterator native_iterator;
			typedef typename std::vector<T>::const_iterator native_const_iterator;


			static T &access(const native_iterator &it) {
				return *it;
			}

			static const T &access(const native_const_iterator &it) {
				return *it;
			}

			template <class... _Ty>
			native_iterator insert_impl(_Ty&&... args) {
				return mData.emplace(mData.end(), std::forward<_Ty>(args)...);
			}

			static void write(SerializeOutStream &out, const native_iterator &it) {
				write(out, *it);
			}

		protected:
			std::vector<T> mData;

		};
		

		template <class T, class NativeContainer, class Creator>
		class Container : protected Creator, public Serializable, protected ContainerWrapper<NativeContainer> {
		public:
			typedef typename Creator::ArgsTuple ArgsTuple;
			typedef std::function<ArgsTuple(SerializeInStream&)> Factory;
			
			/*typedef NativeContainer NativeContainer;*/
			typedef typename ContainerWrapper<NativeContainer>::native_iterator native_iterator;
			typedef typename ContainerWrapper<NativeContainer>::native_const_iterator native_const_iterator;
			typedef std::iterator_traits<native_iterator> native_traits;
			typedef typename native_traits::value_type value_type;

			typedef size_t TransactionId;

			struct Change {
				native_const_iterator mWhere;
				TransactionId mId;
			};
			typedef std::list<Change> ChangesList;


			class iterator : public std::iterator<typename native_traits::iterator_category, value_type> {
			public:
				struct begin_t {};
				struct end_t {};
				static constexpr const begin_t begin = begin_t();
				static constexpr const end_t end = end_t();

				iterator(native_iterator&& it, typename ChangesList::const_iterator&& skipIt, const ChangesList *skipList) :
					mIntern(std::forward<native_iterator>(it)),
					mSkipIt(std::forward<ChangesList::const_iterator>(skipIt)),
					mSkipList(skipList)
				{
					validate();
				}

				iterator(native_iterator&& it, const ChangesList *skipList, begin_t) : iterator(std::forward<native_iterator>(it), skipList ? skipList->begin() : ChangesList::const_iterator(), skipList)
				{

				}

				iterator(Container &c, begin_t) : iterator(c.mData.begin(), c.pendingRemoves(), begin)
				{

				}

				iterator(native_iterator&& it, const ChangesList *skipList, end_t) : iterator(std::forward<native_iterator>(it), skipList ? skipList->end() : ChangesList::const_iterator(), skipList)
				{

				}

				iterator(Container &c, end_t) : iterator(c.mData.end(), c.pendingRemoves(), begin)
				{

				}

				value_type &operator *() const {
					return *mIntern;
				}

				value_type *operator ->() const {
					return &*mIntern;
				}
				bool operator !=(const iterator &other) const {
					return mIntern != other.mIntern;
				}
				bool operator ==(const iterator &other) const {
					return mIntern == other.mIntern;
				}

				void operator++() {
					++mIntern;
					validate();
				}

				void operator--() {
					--mIntern;
					inverse_validate();
				}

			private:
				void validate() {
					if (mSkipList) {
						while (mSkipIt != mSkipList->end() && mSkipIt->mWhere == mIntern) {
							++mSkipIt;
							++mIntern;
						}
					}
				}

				void inverse_validate() {
					if (mSkipList) {
						while (mSkipIt != mSkipList->begin() && std::prev(mSkipIt)->mWhere == mIntern) {
							--mSkipIt;
							--mIntern;
						}
					}
				}

			private:
				friend class Container<T, NativeContainer, Creator>;

				const ChangesList *mSkipList;
				typename ChangesList::const_iterator mSkipIt;

				typename native_iterator mIntern;
			};

			class const_iterator : public std::iterator<typename native_traits::iterator_category, value_type> {
			public:
				struct begin_t {};
				struct end_t {};
				static constexpr const begin_t begin = begin_t();
				static constexpr const end_t end = end_t();

				const_iterator(native_const_iterator&& it, typename ChangesList::const_iterator&& skipIt, const ChangesList *skipList) :
					mIntern(std::forward<native_const_iterator>(it)),
					mSkipIt(std::forward<ChangesList::const_iterator>(skipIt)),
					mSkipList(skipList)
				{
					validate();
				}

				const_iterator(const native_const_iterator& it, const typename ChangesList::const_iterator& skipIt, const ChangesList *skipList) :
					mIntern(it),
					mSkipIt(skipIt),
					mSkipList(skipList)
				{
					validate();
				}

				const_iterator(const typename Container::iterator &it) :
					const_iterator(it.mIntern, it.mSkipIt, it.mSkipList)
				{

				}

				const_iterator(native_const_iterator&& it, const ChangesList *skipList, begin_t) : const_iterator(std::forward<native_const_iterator>(it), skipList ? skipList->begin() : ChangesList::const_iterator(), skipList)
				{

				}

				const_iterator(const Container &c, begin_t) : const_iterator(c.mData.begin(), c.pendingRemoves(), begin)
				{

				}

				const_iterator(native_const_iterator&& it, const ChangesList *skipList, end_t) : const_iterator(std::forward<native_const_iterator>(it), skipList ? skipList->end() : ChangesList::const_iterator(), skipList)
				{

				}

				const_iterator(const Container &c, end_t) : const_iterator(c.mData.end(), c.pendingRemoves(), begin)
				{

				}

				const value_type &operator *() const {
					return *mIntern;
				}

				const value_type *operator ->() const {
					return &*mIntern;
				}
				bool operator !=(const const_iterator &other) const {
					return mIntern != other.mIntern;
				}
				bool operator ==(const const_iterator &other) const {
					return mIntern == other.mIntern;
				}

				void operator++() {
					++mIntern;
					validate();
				}

				void operator--() {
					--mIntern;
					inverse_validate();
				}

			private:
				void validate() {
					if (mSkipList) {
						while (mSkipIt != mSkipList->end() && mSkipIt->mWhere == mIntern) {
							++mSkipIt;
							++mIntern;
						}
					}
				}

				void inverse_validate() {
					if (mSkipList) {
						while (mSkipIt != mSkipList->begin() && std::prev(mSkipIt)->mWhere == mIntern) {
							--mSkipIt;
							--mIntern;
						}
					}
				}

			private:
				friend class Container<T, NativeContainer, Creator>;

				const ChangesList *mSkipList;
				typename ChangesList::const_iterator mSkipIt;

				typename native_const_iterator mIntern;
			};

		public:			

			Container() {

			}

			Container(SerializableUnit *parent) :
			Serializable(parent){

			}

			template <class P, class... _Ty>
			Container(P *parent, ArgsTuple(P::*factory)(_Ty...)) :
				Serializable(parent)
			{
				mCreationDataFactory = [=](SerializeInStream &in) {
					std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> args;
					TupleSerializer::readTuple(args, in);
					return TupleUnpacker<>::call(parent, factory, args); };
			}

			const_iterator begin() const {
				return{ *this, const_iterator::begin };
			}

			const_iterator end() const {
				return{ *this, const_iterator::end };
			}

			iterator begin() {
				return{ *this, iterator::begin };
			}

			iterator end() {
				return{ *this, iterator::end };
			}

			size_t size() const {
				return mData.size();
			}

			void clear() {
				mData.clear();
			}

			bool contains(const T &item) const {
				return std::find(begin(), end(), item) != end();
			}

			iterator erase(const iterator &where) {
				iterator it = where;
				++it;
				erase_impl(where.mIntern);
				return it;
			}

			T &at(size_t i) {
				iterator it = begin();
				std::advance(it, i);
				return *it;
			}

			const T &at(size_t i) const {
				const_iterator it = begin();
				std::advance(it, i);
				return *it;
			}

			virtual void writeState(SerializeOutStream &out) const override {
				for (auto it = begin(); it != end(); ++it) {
					const T &t = access(it.mIntern);
					if (filter(out, t)) {
						write(out, it.mIntern);
					}
				}
				out << ValueType::EOL();
			}

			virtual void readState(SerializeInStream &in) override {
				while (in.loopRead()) {
					iterator it = insertTuple(readCreationData(in));
					read(in, access(it.mIntern));
				}
			}

			virtual void applySerializableMap(const std::map<InvPtr, SerializableUnit*> &map) override {
				for (auto it = begin(); it != end(); ++it) {
					T &t = access(it.mIntern);
					applyMap(map, t);
				}
			}

			template <class... _Ty>
			T &emplace(_Ty&&... args) {
				return access(insert(std::forward<_Ty>(args)...).mIntern);
			}

			template <class... _Ty>
			T &emplaceTuple(std::tuple<_Ty...>&& tuple) {
				return access(insertTuple(std::forward<std::tuple<_Ty...>>(tuple)).mIntern);
			}

		protected:

			virtual const ChangesList *pendingAdds() const { return 0; }
			virtual const ChangesList *pendingRemoves() const { return 0; }

			virtual void onInsert(const iterator &) const {}
			virtual void onRemove(const iterator &) const {}

			template <class... _Ty>
			iterator insertTuple(std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<>::call(this, &Container::insert<_Ty...>, std::forward<std::tuple<_Ty...>>(tuple));
			}

			template <class... _Ty>
			iterator insert(_Ty&&... args) {
				iterator it(insert_impl(std::forward<_Ty>(args)...), pendingRemoves(), iterator::end);
				onInsert(it);
				return it;
			}


			ArgsTuple readCreationData(SerializeInStream &in) {
				if (mCreationDataFactory) {
					return mCreationDataFactory(in);
				}
				else {
					return defaultCreationData(in);
				}
			}

			const native_iterator &intern(const iterator &it) {
				return it.mIntern;
			}


		private:
			Factory mCreationDataFactory;
			

		};

		template <class C>
		class ObservableContainer : public C, public Observable {
		public:
			typedef typename Container::ArgsTuple ArgsTuple;

			ObservableContainer(SerializableUnit *parent) :
				Observable(parent),
				C(parent){

			}

			template <class P, class... _Ty>
			ObservableContainer(P *parent, ArgsTuple(P::*factory)(_Ty...)) :
				Observable(parent),
				C(parent, factory)
			{
			}

			// Inherited via Observable
			virtual void readChanges(SerializeInStream & in) override
			{
			}

		};
		
	}
}