#pragma once

#include "templates.h"
#include "valuetype.h"
#include "../serializemanager.h"
#include "../serializable.h"
#include "../observable.h"
#include "../Streams/bufferedstream.h"
#include "../toplevelserializableunit.h"

namespace Engine {
	namespace Serialize {
		enum Operations {
			ADD_ITEM = 0x1,
			CLEAR = 0x2,


			MASK = 0x0F,
			ACCEPT = 0x10,
			REJECT = 0x20
		};

		template <class T, bool b = std::is_base_of<SerializableUnit, T>::value>
		struct UnitHelper {

			typedef T Type;



			static void read_id(SerializeInStream &in, T &item) {
			}

			static void write_id(SerializeOutStream &out, const T &item) {
			}

			static void write_creation(SerializeOutStream &out, const T &item) {
			}

			static void read_state(SerializeInStream &in, T &item) {
				in >> item;
			}

			static void write_state(SerializeOutStream &out, const T &item) {
				out << item;
			}

			static bool filter(SerializeOutStream &out, const T &item) {
				return true;
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, T &item) {
			}

			static void setTopLevel(T &item, TopLevelSerializableUnit *topLevel) {
			}
			

		};

		template <class T, bool b>
		struct UnitHelper<T*, b> {

			typedef T* Type;

			static void read_state(SerializeInStream &in, T *&item) {
				SerializableUnit *unit = in.manager().readPtr(in);
				T *temp = dynamic_cast<T*>(unit);
				if (!temp)
					throw 0;
				item = temp;
			}

			static void read_id(SerializeInStream &in, T *item) {
			}

			static void write_id(SerializeOutStream &out, T *item) {
			}

			static void write_creation(SerializeOutStream &out, T *item) {
			}

			static void write_state(SerializeOutStream &out, T *item) {
				out.manager().writePtr(out, item);
			}

			static bool filter(SerializeOutStream &out, T *item) {
				return true;
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, T *&item) {
			}

			static void setTopLevel(T* &item, TopLevelSerializableUnit *topLevel) {
			}

		};

		template <bool b>
		struct UnitHelper<ValueType, b> {

			typedef ValueType Type;

			static void read_state(SerializeInStream &in, ValueType &item) {
				in >> item;
			}

			static void read_id(SerializeInStream &in, ValueType &item) {
			}

			static void write_id(SerializeOutStream &out, const ValueType &item) {
			}

			static void write_creation(SerializeOutStream &out, const ValueType &item) {
			}

			static void write_state(SerializeOutStream &out, const ValueType &item) {
				out << item;
			}

			static bool filter(SerializeOutStream &out, const ValueType &item) {
				return true;
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, ValueType &item) {
				item.applySerializableMap(map);
			}

			static void setTopLevel(ValueType &item, TopLevelSerializableUnit *topLevel) {
			}

		};

		template <class T, bool b>
		struct UnitHelper<Ogre::unique_ptr<T>, b> {

			typedef Ogre::unique_ptr<typename UnitHelper<T>::Type> Type;

			static void read_state(SerializeInStream &in, Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::read_state(in, *item);
			}

			static void read_id(SerializeInStream &in, Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::read_id(in, *item);
			}

			static void write_id(SerializeOutStream &out, const Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::write_id(out, *item);
			}

			static void write_creation(SerializeOutStream &out, const Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::write_creation(out, *item);
			}

			static void write_state(SerializeOutStream &out, const Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::write_state(out, *item);
			}

			static bool filter(SerializeOutStream &out, const Ogre::unique_ptr<T> &item) {
				return UnitHelper<T>::filter(out, *item);
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, const Ogre::unique_ptr<T> &item) {
				UnitHelper<T>::applyMap(map, *item);
			}

			static void setTopLevel(Ogre::unique_ptr<T> &item, TopLevelSerializableUnit *topLevel) {
				UnitHelper<T>::setTopLevel(*item, topLevel);
			}


		};

		template <class T>
		struct UnitHelper<T, true> {

			typedef T Type;

			static void read_state(SerializeInStream &in, T &item) {
				item.readState(in);			
			}

			static void read_id(SerializeInStream &in, T &item) {
				item.readId(in);
			}

			static void write_id(SerializeOutStream &out, const T &item) {
				item.writeId(out);
			}

			static void write_creation(SerializeOutStream &out, const T &item) {
				item.writeCreationData(out);
			}

			static void write_state(SerializeOutStream &out, const T &item) {
				item.writeState(out);
			}

			static bool filter(SerializeOutStream &out, const T &item) {
				return out.manager().filter(&item, out.id());
			}

			static void applyMap(const std::map<InvPtr, SerializableUnit*> &map, T &item) {
				item.applySerializableMap(map);
			}

			static void setTopLevel(T &item, TopLevelSerializableUnit *topLevel) {
				item.setTopLevel(topLevel);
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
			typedef std::vector<Type> NativeContainer;
			typedef typename NativeContainer::iterator native_iterator;
			typedef typename NativeContainer::const_iterator native_const_iterator;


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

		protected:
			NativeContainer mData;

		};
		

		template <class T, class NativeContainer, class Creator>
		class Container : protected Creator, public Serializable, protected ContainerWrapper<NativeContainer> {
		public:
			typedef T T;

			typedef typename Creator::ArgsTuple ArgsTuple;
			typedef std::function<ArgsTuple(SerializeInStream&)> Factory;
			
			/*typedef NativeContainer NativeContainer;*/
			typedef typename ContainerWrapper<NativeContainer>::native_iterator iterator;
			typedef typename ContainerWrapper<NativeContainer>::native_const_iterator const_iterator;

			
			/*class iterator : public std::iterator<typename native_traits::iterator_category, value_type> {
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

				iterator(native_iterator&& it, const typename ChangesList::const_iterator& skipIt, const ChangesList *skipList) :
					mIntern(std::forward<native_iterator>(it)),
					mSkipIt(skipIt),
					mSkipList(skipList)
				{
					validate();
				}

				iterator(const native_iterator& it, Container &c) : iterator(c, begin) {
					while (mIntern != it) {
						++mIntern;
						validate();
					}
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
			};*/

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
				return mData.begin();
				//return{ *this, const_iterator::begin };
			}

			const_iterator end() const {
				return mData.end();
				//return{ *this, const_iterator::end };
			}

			iterator begin() {
				return mData.begin();
				//return{ *this, iterator::begin };
			}

			iterator end() {
				return mData.end();
				//return{ *this, iterator::end };
			}

			size_t size() const {
				return mData.size();
			}

			void clear() {
				onClear();
				mData.clear();
			}

			bool contains(const T &item) const {
				return std::find(begin(), end(), item) != end();
			}

			iterator erase(const iterator &where) {
				onRemove(where);
				return erase_impl(where);
			}



virtual void writeState(SerializeOutStream &out) const override {
	for (auto it = begin(); it != end(); ++it) {
		const T &t = access(it);
		if (filter(out, t)) {
			write_creation(out, it);
			write_id(out, t);
			write_state(out, t);
		}
	}
	out << ValueType::EOL();
}

virtual void readState(SerializeInStream &in) override {
	clear();
	while (in.loopRead()) {
		iterator it = insert_tuple_where(true, end(), readCreationData(in));
		read_id(in, access(it));
		read_state(in, access(it));		
	}
}



virtual void applySerializableMap(const std::map<InvPtr, SerializableUnit*> &map) override {
	for (auto it = begin(); it != end(); ++it) {
		T &t = access(it);
		applyMap(map, t);
	}
}

void setCallback(std::function<void(const iterator &, Operations)> callback) {
	mCallback = callback;
}

		protected:

			template <class... _Ty>
			iterator insert_tuple_where(bool &&authorized, const iterator &where, std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<bool, const iterator &>::call(this, &Container::insert_where<_Ty...>, std::forward<bool>(authorized), where, std::forward<std::tuple<_Ty...>>(tuple));
			}

			template <class... _Ty>
			void insert_tuple_where_safe(std::function<void(const iterator &)> callback, const iterator &where, std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<std::function<void(const iterator &)>, const iterator &>::call(this, &Container::insert_where_safe<_Ty...>, std::forward<std::function<void(const iterator &)>>(callback), where, std::forward<std::tuple<_Ty...>>(tuple));
			}

			template <class... _Ty>
			iterator insert_where(bool authorized, const iterator &where, _Ty&&... args) {
				if (!authorized && observed() && !topLevel()->isMaster()) {
					T temp(std::forward<_Ty>(args)...);
					requestInsert(where, temp);
					return end();
				}
				else {
					iterator it = insert_where_impl(where, std::forward<_Ty>(args)...);
					setTopLevel(access(it), topLevel());
					onInsert(it);
					return it;
				}
			}

			template <class... _Ty>
			void insert_where_safe(std::function<void(const iterator &)> callback, const iterator &where, _Ty&&... args) {
				if (observed() && !topLevel()->isMaster()) {
					T temp(std::forward<_Ty>(args)...);
					requestInsert(where, temp, callback);
				}
				else {
					iterator it = insert_where_impl(where, std::forward<_Ty>(args)...);
					setTopLevel(access(it), topLevel());
					onInsert(it);
					callback(it);
				}
			}

			virtual void requestInsert(const iterator &, const T &) const {}
			virtual void requestInsert(const iterator &, const T &, std::function<void(const iterator &)>) {}
			virtual void onClear() const {}
			virtual void onInsert(const iterator &it) const {
				if (mCallback)
					mCallback(it, ADD_ITEM);
			}
			virtual void onRemove(const iterator &) const {}

			virtual bool observed() const { return false; }


			ArgsTuple readCreationData(SerializeInStream &in) {
				if (mCreationDataFactory) {
					return mCreationDataFactory(in);
				}
				else {
					return defaultCreationData(in);
				}
			}

		private:
			Factory mCreationDataFactory;
			std::function<void(const iterator &, Operations)> mCallback;

		};

		template <class C>
		class ObservableContainer : public C, public Observable {
		public:
			typedef typename C::ArgsTuple ArgsTuple;

			typedef size_t TransactionId;

			struct Transaction {
				Transaction(TransactionId id, std::function<void(const typename C::iterator &it)> callback) :
					mId(id), mCallback(callback) 
				{}

				TransactionId mId;
				std::function<void(const typename C::iterator &it)> mCallback;
			};


			ObservableContainer(SerializableUnit *parent) :
				Observable(parent),
				C(parent),
				mTransactionCounter(0){

			}

			template <class P, class... _Ty>
			ObservableContainer(P *parent, ArgsTuple(P::*factory)(_Ty...)) :
				Observable(parent),
				C(parent, factory)
			{
			}

			virtual void onClear() const override {
				C::onClear();
				for (BufferedOutStream *out : getMasterActionMessageTargets()) {
					*out << CLEAR;
					out->endMessage();
				}
			}

			virtual void requestInsert(const typename C::iterator &where, const typename C::T &item) const override {
				BufferedOutStream *out = getSlaveActionMessageTarget();
				*out << ADD_ITEM;
				*out << (size_t)0;
				write_iterator(*out, where);
				UnitHelper::write_creation(*out, item);
				write_state(*out, item);
				out->endMessage();
			}

			virtual void requestInsert(const typename C::iterator &where, const typename C::T &item, std::function<void(const typename C::iterator &)> callback) override {
				TransactionId id = ++mTransactionCounter;

				mTransactions.emplace_back(id, callback);

				BufferedOutStream *out = getSlaveActionMessageTarget();
				*out << ADD_ITEM;
				*out << id;
				write_iterator(*out, where);
				UnitHelper::write_creation(*out, item);
				write_state(*out, item);
				out->endMessage();
			}

			// Inherited via Observable
			virtual void readRequest(BufferedInOutStream & inout) override
			{
				bool accepted = true; //Check TODO

				Operations op;
				inout >> (int&)op;

				TransactionId id;
				inout >> id;


				if (!accepted) {
					inout.beginMessage();
					inout << (op | REJECT);
					inout << id;
					inout.endMessage();
				}
				else {					

					auto callback = [&, op, id](const typename C::iterator &it) {
						bool accepted = (it != end());
						inout.beginMessage();
						writeMasterActionMessageHeader(inout);
						inout << (op | (accepted ? ACCEPT : REJECT));
						inout << id;
						if (accepted) {
							read_state(inout, access(it));
							write_item(inout, it);
							write_state(inout, access(it));
						}
						inout.endMessage();
					};

					switch (op) {
					case ADD_ITEM:
						read_item_save(callback, inout);
						break;
					case CLEAR:
						clear();
						break;
					default:
						throw 0;
					}


				}
				

			}

			virtual void readAction(BufferedInOutStream &inout) override {
				Operations op;
				inout >> (int&)op;

				TransactionId id;
				inout >> id;

				bool accepted = (op & (~MASK)) == ACCEPT;

				if (id) {
					if (mTransactions.empty() || mTransactions.front().mId != id)
						throw 0;

					iterator it = end();
					switch (op & MASK) {
					case ADD_ITEM:
						it = read_item(inout);
						read_state(inout, access(it));
						break;
					default:
						throw 0;
					}

					mTransactions.front().mCallback(it);
					mTransactions.pop_front();
				}
				else {
					if (!accepted)
						throw 0;
				}
			}

			virtual void onInsert(const typename C::iterator &it) const override {
				C::onInsert(it);
				for (BufferedOutStream *out : getMasterActionMessageTargets()) {
					*out << ADD_ITEM;
					*out << (TransactionId)0;
					write_item(*out, it);
					write_state(*out, access(it));
					out->endMessage();
				}
				
			}

		protected:
			virtual bool observed() const override {
				return true;
			}

		private:

			TransactionId mTransactionCounter;

			std::list<Transaction> mTransactions;
		};
		
	}
}