#pragma once

#include "../generic/callable_traits.h"
#include "../generic/tupleunpacker.h"
#include "tupleserialize.h"

namespace Engine
{
	namespace Serialize
	{

		template <typename FirstCreator, typename SecondCreator>
		struct PairCreator : private FirstCreator, private SecondCreator
		{		
			using ArgsTuple = std::tuple<std::piecewise_construct_t, typename FirstCreator::ArgsTuple, typename SecondCreator::
			                             ArgsTuple>;		

			auto readCreationData(SerializeInStream& in)
			{
				auto&& first = FirstCreator::readCreationData(in);
				return std::make_tuple(std::piecewise_construct, first, SecondCreator::readCreationData(in));
			}
		};

		template <typename... Args>
		struct DefaultCreator
		{
			using ArgsTuple = std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>;

			DefaultCreator() = default;
			DefaultCreator(const void *) {}
		
			ArgsTuple readCreationData(SerializeInStream& in)
			{
				ArgsTuple tuple;
				in >> tuple;
				return std::move(tuple);
			}
		};

		/*namespace __creationhelper__impl__{
			template <typename R, typename T, typename... _Ty>
			struct _CustomCreator
			{
				using ArgsTuple = R;

				void setCreator(const std::function<R(_Ty ...)>& f)
				{
					mCallback = f;
				}

				R readCreationData(SerializeInStream& in)
				{
					std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> tuple;
					in >> tuple;
					return TupleUnpacker::invokeExpand(mCallback, std::move(tuple));
				}

			private:
				std::function<R(_Ty ...)> mCallback;
			};
		}

		template <typename F>
		using CustomCreator = typename CallableTraits<F>::template instance<__creationhelper__impl__::_CustomCreator>::type;*/

		namespace __creationhelper__impl__{
                    template <auto f, typename R, typename T, typename... _Ty>
			struct _ParentCreator
			{	
				using ArgsTuple = R;
				
				_ParentCreator(T *t) :
					mParent(t)
				{
					assert(mParent);
				}

				R readCreationData(SerializeInStream& in)
				{
					std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> tuple;
					in >> tuple;
					return TupleUnpacker::invokeExpand(f, mParent, std::move(tuple));
				}

			private:
				T* mParent;
			};
		}

		template <auto f>
		using ParentCreator = typename MemberFunctionCapture<__creationhelper__impl__::_ParentCreator, f>::type;
	}
}
