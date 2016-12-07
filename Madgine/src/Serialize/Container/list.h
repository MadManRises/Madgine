#pragma once

#include "../observable.h"
#include "../serializable.h"

namespace Engine {
namespace Serialize {



	template <class T, class... Args>
	class SerializableList : public Serializable, protected Container<T, Args...> {
	public:
		typedef std::list<T> ListType;
		typedef typename ListType::iterator iterator;
		typedef typename ListType::const_iterator const_iterator;
		
		template <class P, class... _Ty>
		SerializableList(P *parent, ArgsTuple(P::*factory)(_Ty...)) :
			Serializable(parent),
			Container(parent, factory) {}
		SerializableList(Serialize::SerializableUnit *parent) :
			Serializable(parent) {}
		SerializableList() {}
		virtual ~SerializableList() = default;

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

		virtual void clear() {
			mData.clear();
		}

		template <class... _Ty>
		T &emplace(_Ty&&... args) {
			return *insert(std::forward<_Ty>(args)...);
		}

		iterator erase(const iterator &where) {
			return mData.erase(where);//TODO
		}

		T &emplaceTuple(ArgsTuple&& args) {
			return *insertTuple(std::forward<ArgsTuple>(args));
		}

	protected:
		
		void loadItem(SerializeInStream &in) {
			iterator it = insertTuple(readCreationData(in));
			read(in, *it);
		}

		iterator insertTuple(ArgsTuple&& tuple) {
			return TupleUnpacker<>::call(this, &SerializableList::insert<Args...>, std::forward<ArgsTuple>(tuple));
		}

		template <class... _Ty>
		iterator insert(_Ty&&... args) {
			iterator it = mData.emplace(mData.end(), std::forward<_Ty>(args)...);
			onInsert(it);
			return it;
		}

		virtual void onInsert(const iterator &where) {}

		std::list<T> mData;		
	};

	template <class T, class... Args>
	class ObservableList : public SerializableList<T, Args...>, public Observable {
	public:

		template <class P, class... _Ty>
		ObservableList(P *parent, ArgsTuple (P::*factory)(_Ty...)) :
			SerializableList(parent, factory),
			Observable(parent) {}
		ObservableList(Serialize::SerializableUnit *parent) :
			SerializableList(parent),
			Observable(parent) {}
		

		virtual void clear() override {
			for (SerializeOutStream *out : getMessageTargets(true)) {

				*out << CLEAR;

			}

			SerializableList::clear();
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
				it->writeCreationData(*out);
				it->writeState(*out);
			}
		}


	};



}
}


