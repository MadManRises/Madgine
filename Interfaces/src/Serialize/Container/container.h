#pragma once

#include "basecontainer.h"
#include "templates.h"

namespace Engine {
	namespace Serialize {

		template <class NativeContainer, class Creator>
		class Container : public BaseContainer<NativeContainer>, protected Creator {
		public:
			
			typedef typename Creator::ArgsTuple ArgsTuple;
			typedef std::function<ArgsTuple(SerializeInStream&)> Factory;

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

		protected:

			Container() {

			}

			template <class P, class... _Ty>
			Container(P *parent, ArgsTuple(P::*factory)(_Ty...))
			{
				mCreationDataFactory = [=](SerializeInStream &in) {
					std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> args;
					TupleSerializer::readTuple(args, in);
					return TupleUnpacker<>::call(parent, factory, args); };
			}

			template <class... _Ty>
			iterator insert_tuple_where(const iterator &where, std::tuple<_Ty...>&& tuple) {
				return TupleUnpacker<const iterator &>::call(static_cast<BaseContainer<NativeContainer>*>(this), &BaseContainer<NativeContainer>::template insert_where<_Ty...>, where, std::forward<std::tuple<_Ty...>>(tuple));
			}

			ArgsTuple readCreationData(SerializeInStream &in) {
				if (mCreationDataFactory) {
					return mCreationDataFactory(in);
				}
				else {
					return this->defaultCreationData(in);
				}
			}


		private:
			Factory mCreationDataFactory;

		};

	}
}