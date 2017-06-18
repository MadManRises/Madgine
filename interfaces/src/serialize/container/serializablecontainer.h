#pragma once

#include "../serializable.h"
#include "containerselector.h"
#include "signalslot/signal.h"

namespace Engine {
	namespace Serialize {
		
		enum Operations {
			INSERT_ITEM = 0x1,
			REMOVE_ITEM = 0x2,
			CLEAR = 0x3,
			RESET = 0x4,


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

			typedef typename Container<C, Creator>::NativeContainerType NativeContainerType;

			typedef typename Base::ArgsTuple ArgsTuple;
			typedef std::function<ArgsTuple(SerializeInStream&)> Factory;
			
			typedef typename Base::iterator iterator;
			typedef typename Base::const_iterator const_iterator;
			
		public:			

			using Base::Base;
			//using Base::operator=;
			//WORKAROUND for VS bug
			template <class T>
			void operator=(T &&t) {
				Base::operator=(std::forward<T>(t));
			}

			void clear() {
				beforeClear();
				this->mData.clear();
				afterClear();
			}

			iterator erase(const iterator &where) {
				beforeRemove(where);
				iterator it = Base::erase(where);
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
				beforeReset();
				this->mData.clear();
				while (in.loopRead()) {
					this->read_item_where(this->end(), in, topLevel());
				}
				afterReset();
			}



			virtual void applySerializableMap(const std::map<size_t, SerializableUnitBase*> &map) override {
				for (auto it = this->begin(); it != this->end(); ++it) {
					this->applyMap(map, *it);
				}
			}

			template <class T>
			void connectCallback(T &&slot) {
				mSignal.connect(slot);
			}



		protected:

			void beforeReset() {
				mSignal.emit(this->end(), BEFORE | RESET);
			}

			void afterReset() {
				mSignal.emit(this->end(), AFTER | RESET);
			}

			void beforeClear() {
				mSignal.emit(this->end(), BEFORE | CLEAR);
			}

			void afterClear() {
				mSignal.emit(this->end(), AFTER | CLEAR);
			}
			
			void onInsert(const iterator &it) {
				mSignal.emit(it, INSERT_ITEM);
			}
			
			void beforeRemove(const iterator &it) {
				this->clearItemTopLevel(*it);
				mSignal.emit(it, BEFORE | REMOVE_ITEM);
			}

			void afterRemove() {
				mSignal.emit(this->end(), AFTER | REMOVE_ITEM);
			}

			template <class T, class... _Ty>
			iterator insert(T &callback, const const_iterator &where, _Ty&&... args) {
				iterator it = Base::insert(where, std::forward<_Ty>(args)...);
				TupleUnpacker<>::call(callback, std::make_tuple(it, true));					
				onInsert(it);
				if (topLevel())
					this->setItemTopLevel(*it, topLevel());				
				return it;
			}

			template <class... _Ty>
			iterator insert(const const_iterator &where, _Ty&&... args) {				
				iterator it = Base::insert(where, std::forward<_Ty>(args)...);					
				onInsert(it);
				if (topLevel())
					this->setItemTopLevel(*it, topLevel());
				return it;
			}

		private:
			SignalSlot::Signal<const iterator &, int> mSignal;			

		};


	}
}
