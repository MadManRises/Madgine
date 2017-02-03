#pragma once

#include "../serializable.h"
#include "containerselector.h"

namespace Engine {
	namespace Serialize {
		
		enum Operations {
			INSERT_ITEM = 0x1,
			REMOVE_ITEM = 0x2,
			CLEAR = 0x3,


			MASK = 0x0F,
			ACCEPT = 0x10,
			REJECT = 0x20,

			BEFORE = 0x0,
			AFTER = 0x10
		};

		template <class C, class Creator>
		class SerializableContainer : public Serializable, public ContainerSelector<C, Creator> {
		public:



			typedef ContainerSelector<C, Creator> Base;

			typedef typename Base::ArgsTuple ArgsTuple;
			typedef std::function<ArgsTuple(SerializeInStream&)> Factory;
			
			typedef typename Base::iterator iterator;
			typedef typename Base::const_iterator const_iterator;
			
		public:			

			SerializableContainer() {

			}

			SerializableContainer(SerializableUnit *parent) :
			Serializable(parent){

			}

			template <class P, class... _Ty>
			SerializableContainer(P *parent, ArgsTuple(P::*factory)(_Ty...)) :
				Serializable(parent),
				Base(parent, factory)
			{
			}

			SerializableContainer(const SerializableContainer &) = delete;
			void operator=(const SerializableContainer &) = delete;

			void clear() {
				beforeClear();
				this->mData.clear();
				afterClear();
			}

			const_iterator erase(const const_iterator &where) {
				beforeRemove(where);
				const_iterator it = Base::erase(where);
				afterRemove();
				return it;
			}
			
			virtual void writeState(SerializeOutStream &out) const override {
				for (auto it = this->begin(); it != this->end(); ++it) {
					const auto &t = *it;
					if (this->filter(out, t)) {
						this->write_item(out, t);
					}
				}
				out << ValueType::EOL();
			}

			virtual void readState(SerializeInStream &in) override {
				clear();
				while (in.loopRead()) {
					iterator it = this->read_item_where(this->end(), in);
					onInsert(it);
				}	
			}



			virtual void applySerializableMap(const std::map<InvPtr, SerializableUnit*> &map) override {
				for (auto it = this->begin(); it != this->end(); ++it) {
					this->applyMap(map, *it);
				}
			}

			void setCallback(std::function<void(const const_iterator &, int)> callback) {
				mCallback = callback;
			}


		protected:

			void beforeClear() const {
				if (mCallback)
					mCallback(this->end(), BEFORE | CLEAR);
			}

			void afterClear() const {
				if (mCallback)
					mCallback(this->end(), AFTER | CLEAR);
			}
			
			void onInsert(const iterator &it) const {
				this->setTopLevel(*it, topLevel());
				if (mCallback)
					mCallback(it, INSERT_ITEM);
			}
			
			void beforeRemove(const const_iterator &it) const {
				if (mCallback)
					mCallback(it, BEFORE | REMOVE_ITEM);
			}

			void afterRemove() const {
				if (mCallback)
					mCallback(this->end(), AFTER | REMOVE_ITEM);
			}

			template <class... _Ty>
			iterator insert_where(const iterator &where, _Ty&&... args) {
				iterator it = Base::insert_where(where, std::forward<_Ty>(args)...);
				onInsert(it);
				return it;
			}

			

		private:
			std::function<void(const const_iterator &, int)> mCallback;

		};

	}
}