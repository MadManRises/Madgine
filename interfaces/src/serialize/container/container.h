#pragma once

#include "../streams/serializestream.h"
#include "basecontainer.h"
#include "templates.h"

namespace Engine {
	namespace Serialize {

		template <class NativeContainer, class Creator>
		class Container : public BaseContainer<NativeContainer>, public Creator {
		public:
			
			typedef typename Creator::ArgsTuple ArgsTuple;

			typedef typename BaseContainer<NativeContainer>::NativeContainerType NativeContainerType;

			typedef typename BaseContainer<NativeContainer>::Type Type;

			typedef typename BaseContainer<NativeContainer>::iterator iterator;
			typedef typename BaseContainer<NativeContainer>::const_iterator const_iterator;

			const_iterator begin() const {
				return this->mData.begin();
			}

			const_iterator end() const {
				return this->mData.end();
			}

			iterator begin() {
				return this->mData.begin();
			}

			iterator end() {
				return this->mData.end();
			}

			const NativeContainerType &data() const {
				return this->mData;
			}

			using BaseContainer<NativeContainer>::BaseContainer;
			using BaseContainer<NativeContainer>::operator=;
			Container<NativeContainer, Creator> &operator=(const NativeContainer &other) {
				this->mData = other;
				return *this;
			}

		protected:
			template <class... _Ty>
			iterator insert_tuple(const const_iterator &where, std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<const const_iterator &>::call(static_cast<BaseContainer<NativeContainer>*>(this), &BaseContainer<NativeContainer>::template insert<_Ty...>, where, std::forward<std::tuple<_Ty...>>(tuple));
			}


			iterator read_item_where(const const_iterator &where, TopLevelSerializableUnitBase *topLevel, SerializeInStream &in) {				
				iterator it = this->insert_tuple(where, this->readCreationData<Type>(in, topLevel));				
				if (in.isMaster()) {
					size_t old = this->setItemInitialising(*it);
					this->read_state(in, *it);
					this->resetItemInitialising(*it, old);
				}
				else {
					this->read_state(in, *it);
					this->read_id(in, *it);
				}					
				return it;
			}

			void write_item(SerializeOutStream &out, const Type &t) const {
				this->write_creation(out, t);
				this->write_state(out, t);
				if (out.isMaster())
					this->write_id(out, t);
			}

		};


	}
}
