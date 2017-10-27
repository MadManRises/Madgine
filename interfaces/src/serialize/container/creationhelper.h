#pragma once

#include "generic/templates.h"
#include "serialize/serializableunit.h"

namespace Engine {
	namespace Serialize {

		/*struct SerializableUnitTupleExtender {
			template <class Tuple>
			static auto extend(SerializableUnitBase *unit, Tuple &&tuple) {
				return std::tuple_cat(std::make_tuple(unit), std::forward<Tuple>(tuple));
			}
		};

		struct NoTupleExtender {
			template <class Tuple>
			static auto extend(SerializableUnitBase *topLevel, Tuple &&tuple) {
				return std::forward<Tuple>(tuple);
			}
		};

		template <bool extend, class T>
		using TupleExtender = typename std::conditional<extend && std::is_base_of<SerializableUnitBase, T>::value && !std::is_base_of<TopLevelSerializableUnitBase, T>::value, SerializableUnitTupleExtender, NoTupleExtender>::type;

		template <bool extend, class T, class Tuple>
		auto extendTuple(SerializableUnitBase *unit, Tuple &&tuple) {
			return TupleExtender<extend, T>::extend(unit, std::forward<Tuple>(tuple));
		}

		template <bool extend, class T, class Tuple>
		using ExtendedTuple = decltype(extendTuple<extend, T>(std::declval<SerializableUnitBase*>(), std::declval<Tuple>()));
		*/

		template <class FirstCreator, class SecondCreator>
		class PairCreator : private FirstCreator, private SecondCreator {
		public:
			using ArgsTuple = std::tuple<std::piecewise_construct_t, typename FirstCreator::template ArgsTuple, typename SecondCreator::template ArgsTuple>;
		protected:
			auto readCreationData(SerializeInStream &in) {
				auto &&first = FirstCreator::readCreationData(in);
				return std::make_tuple(std::piecewise_construct, first, SecondCreator::readCreationData(in));
			}

		};

		template <class... Args>
		class DefaultCreator {
		public:
			using ArgsTuple = std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>;
		protected:
			ArgsTuple readCreationData(SerializeInStream &in) {
				ArgsTuple tuple;
				TupleSerializer::readTuple(tuple, in);
				return std::move(tuple);
			}
		};

		template <typename F, class R, class... _Ty>
		class _CustomCreator {
		public:
			void setCreator(const std::function<R(_Ty...)> &f) {
				mCallback = f;
			}

			using ArgsTuple = R;

		protected:
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

			using ArgsTuple = R;

		protected:
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
