#pragma once

#include "generic/templates.h"
#include "serialize/serializableunit.h"

namespace Engine {
	namespace Serialize {

		struct SerializableUnitTupleExtender {
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

		template <class FirstCreator, class SecondCreator>
		class PairCreator : private FirstCreator, private SecondCreator {
		public:
			template <class T>
			using ArgsTuple = std::tuple<std::piecewise_construct_t, typename FirstCreator::template ArgsTuple<T>, typename SecondCreator::template ArgsTuple<T>>;
		protected:
			template <class P>
			auto readCreationData(SerializeInStream &in, SerializableUnitBase *parent) {
				auto &&first = FirstCreator::template readCreationData<typename P::first_type>(in, parent);
				return std::make_tuple(std::piecewise_construct, first, SecondCreator::template readCreationData<typename P::second_type>(in, parent));
			}

		};

		template <bool extend, class... Args>
		class _DefaultCreator {
		public:
			template <class T>
			using ArgsTuple = ExtendedTuple<extend, T, std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>>;
		protected:
			template <class T>
			ArgsTuple<T> readCreationData(SerializeInStream &in, SerializableUnitBase *parent) {
				std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...> tuple;
				TupleSerializer::readTuple(tuple, in);
				return extendTuple<extend, T>(parent, std::move(tuple));
			}
		};

		template <class... Args>
		using DefaultCreator = _DefaultCreator<true, Args...>;

		template <class... Args>
		using NoExtendCreator = _DefaultCreator<false, Args...>;

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
			R readCreationData(SerializeInStream &in, SerializableUnitBase *parent) {
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

			template <class _>
			using ArgsTuple = R;

		protected:
			template <class _>
			R readCreationData(SerializeInStream &in, SerializableUnitBase *parent) {
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
