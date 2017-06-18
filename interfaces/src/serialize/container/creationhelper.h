#pragma once

#include "templates.h"
#include "serialize/serializableunit.h"

namespace Engine {
	namespace Serialize {

		template <class... Args>
		class DefaultCreator {
		protected:

			typedef std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...> ArgsTuple;

			ArgsTuple readCreationData(SerializeInStream &in) {
				ArgsTuple tuple;
				TupleSerializer::readTuple(tuple, in);
				return tuple;
			}
		};

		template <typename F, class R, class... _Ty>
		class _CustomCreator {
		public:
			void setCreator(const std::function<R(_Ty...)> &f) {
				mCallback = f;
			}

		protected:
			typedef R ArgsTuple;


			R readCreationData(SerializeInStream &in) {
				std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> tuple;
				TupleSerializer::readTuple(tuple, in);
				return TupleUnpacker<>::call(mCallback, std::move(tuple));				
			}

		private:
			std::function<R(_Ty...)> mCallback;
		};

		template <typename F>
		using CustomCreator = typename CallableDeduce<_CustomCreator, F>::type;

		template <typename F, F f, class T, class R, class... _Ty>
		class _ParentCreator {
		public:
			_ParentCreator() :
				mParent(dynamic_cast<T*>(SerializableUnitBase::findParent(this)))
			{
				assert(mParent);
			}

		protected:
			typedef R ArgsTuple;

			R readCreationData(SerializeInStream &in) {
				std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> tuple;
				TupleSerializer::readTuple(tuple, in);
				return TupleUnpacker<>::call(mParent, f, std::move(tuple));
			}

		private:
			T *mParent;
		};

		template <typename F, F f>
		using ParentCreator = typename MemberFunctionCapture<_ParentCreator, F, f>::type;

	}
}
