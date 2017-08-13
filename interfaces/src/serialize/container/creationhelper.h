#pragma once

#include "templates.h"
#include "serialize/serializableunit.h"

namespace Engine {
	namespace Serialize {

		template <class Tuple, typename... _Ty>
		struct ExtendedTupleType {
			typedef decltype(std::tuple_cat(std::make_tuple(std::declval<_Ty>()...), std::declval<Tuple>())) type;
		};

		template <class Tuple>
		class SerializableUnitExtendedTuple : 
			public ExtendedTupleType<Tuple, TopLevelSerializableUnitBase*>::type{
		public:
			SerializableUnitExtendedTuple(TopLevelSerializableUnitBase *topLevel, Tuple &&tuple) :
				ExtendedTupleType<Tuple, TopLevelSerializableUnitBase*>::type(std::tuple_cat(std::make_tuple(topLevel), std::forward<Tuple>(tuple))) {}
		};

		template <class Tuple>
		class NotExtendedTuple : public Tuple {
		public:
			NotExtendedTuple(TopLevelSerializableUnitBase *topLevel, Tuple &&tuple) :
				Tuple(std::forward<Tuple>(tuple)) {}
		};

		template <class T, class Tuple>
		using ExtendedTuple = typename std::conditional<std::is_base_of<SerializableUnitBase, T>::value && !std::is_base_of<TopLevelSerializableUnitBase, T>::value, SerializableUnitExtendedTuple<Tuple>, NotExtendedTuple<Tuple>>::type;

		
		template <class... Args>
		class DefaultCreator {
		protected:

			typedef std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...> ArgsTuple;

			template <class T>
			ExtendedTuple<T, ArgsTuple> readCreationData(SerializeInStream &in, TopLevelSerializableUnitBase *topLevel) {
				ArgsTuple tuple;
				TupleSerializer::readTuple(tuple, in);
				return { topLevel, std::move(tuple) };
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

			template <class T>
			R readCreationData(SerializeInStream &in, TopLevelSerializableUnitBase *topLevel) {
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

			template <class _>
			R readCreationData(SerializeInStream &in, TopLevelSerializableUnitBase *topLevel) {
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
