#pragma once

#include "generic/templates.h"
#include "serialize/serializableunit.h"

namespace Engine {
	namespace Serialize {

		struct SerializableUnitTupleExtender {
			template <class Tuple>
			static auto extend(TopLevelSerializableUnitBase *topLevel, Tuple &&tuple) {
				return std::tuple_cat(std::make_tuple(topLevel), std::forward<Tuple>(tuple));
			}
		};

		struct NoTupleExtender {
			template <class Tuple>
			static auto extend(TopLevelSerializableUnitBase *topLevel, Tuple &&tuple) {
				return std::forward<Tuple>(tuple);
			}
		};

		template <class T>
		using TupleExtender = typename std::conditional<std::is_base_of<SerializableUnitBase, T>::value && !std::is_base_of<TopLevelSerializableUnitBase, T>::value, SerializableUnitTupleExtender, NoTupleExtender>::type;

		template <class T, class Tuple>
		auto extendTuple(TopLevelSerializableUnitBase *topLevel, Tuple &&tuple) {
			return TupleExtender<T>::extend(topLevel, std::forward<Tuple>(tuple));
		}

		template <class T, class Tuple>
		using ExtendedTuple = decltype(extendTuple<T>(std::declval<TopLevelSerializableUnitBase*>(), std::declval<Tuple>()));

		template <class FirstCreator, class SecondCreator>
		class PairCreator : private FirstCreator, private SecondCreator {
		public:
			template <class T>
			using ArgsTuple = std::tuple<std::piecewise_construct_t, typename FirstCreator::template ArgsTuple<T>, typename SecondCreator::template ArgsTuple<T>>;
		protected:
			template <class P>
			auto readCreationData(SerializeInStream &in, TopLevelSerializableUnitBase *topLevel) {
				auto &&first = FirstCreator::template readCreationData<typename P::first_type>(in, topLevel);
				return std::make_tuple(std::piecewise_construct, first, SecondCreator::template readCreationData<typename P::second_type>(in, topLevel));
			}

		};

		template <class... Args>
		class DefaultCreator {
		public:
			template <class T>
			using ArgsTuple = ExtendedTuple<T, std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>>;
		protected:
			template <class T>
			ArgsTuple<T> readCreationData(SerializeInStream &in, TopLevelSerializableUnitBase *topLevel) {
				std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...> tuple;
				TupleSerializer::readTuple(tuple, in);
				return extendTuple<T>(topLevel, std::move(tuple));
			}
		};

		template <typename F, class R, class... _Ty>
		class _CustomCreator {
		public:
			void setCreator(const std::function<R(_Ty...)> &f) {
				mCallback = f;
			}

			template <class T>
			using ArgsTuple = R;

		protected:
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

			template <class T>
			using ArgsTuple = R;

		protected:
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
