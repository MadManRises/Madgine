#pragma once

#include "generic/templates.h"
#include "serialize/serializableunit.h"
#include "serialize/serializehelper.h"

namespace Engine
{
	namespace Serialize
	{

		template <class FirstCreator, class SecondCreator>
		class PairCreator : private FirstCreator, private SecondCreator
		{
		public:
			using ArgsTuple = std::tuple<std::piecewise_construct_t, typename FirstCreator::ArgsTuple, typename SecondCreator::
			                             ArgsTuple>;
		protected:
			auto readCreationData(SerializeInStream& in)
			{
				auto&& first = FirstCreator::readCreationData(in);
				return std::make_tuple(std::piecewise_construct, first, SecondCreator::readCreationData(in));
			}
		};

		template <class... Args>
		class DefaultCreator
		{
		public:
			using ArgsTuple = std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>;
		protected:
			ArgsTuple readCreationData(SerializeInStream& in)
			{
				ArgsTuple tuple;
				TupleSerializer::readTuple(tuple, in);
				return std::move(tuple);
			}
		};

		namespace __creationhelper__impl__{
			template <typename F, class R, class... _Ty>
			class _CustomCreator
			{
			public:
				void setCreator(const std::function<R(_Ty ...)>& f)
				{
					mCallback = f;
				}

				using ArgsTuple = R;

			protected:
				R readCreationData(SerializeInStream& in)
				{
					std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> tuple;
					TupleSerializer::readTuple(tuple, in);
					return TupleUnpacker<>::call(mCallback, std::move(tuple));
				}

			private:
				std::function<R(_Ty ...)> mCallback;
			};
		}

		template <typename F>
		using CustomCreator = typename CallableDeduce<__creationhelper__impl__::_CustomCreator, F>::type;

		namespace __creationhelper__impl__{
			template <typename F, F f, class T, class R, class... _Ty>
			class _ParentCreator
			{
			public:
				_ParentCreator() :
					mParent(dynamic_cast<T*>(SerializableUnitBase::findParent(this)))
				{
					assert(mParent);
				}

				using ArgsTuple = R;

			protected:
				R readCreationData(SerializeInStream& in)
				{
					std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> tuple;
					TupleSerializer::readTuple(tuple, in);
					return TupleUnpacker<>::call(mParent, f, std::move(tuple));
				}

			private:
				T* mParent;
			};
		}

		template <typename F, F f>
		using ParentCreator = typename MemberFunctionCapture<__creationhelper__impl__::_ParentCreator, F, f>::type;
	}
}
