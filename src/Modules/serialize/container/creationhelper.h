#pragma once

#include "../serializableunit.h"
#include "../../generic/callable_traits.h"
#include "../../generic/tupleunpacker.h"
#include "tupleserialize.h"

namespace Engine
{
	namespace Serialize
	{

		template <class FirstCreator, class SecondCreator>
		struct PairCreator : private FirstCreator, private SecondCreator
		{		
			using ArgsTuple = std::tuple<std::piecewise_construct_t, typename FirstCreator::ArgsTuple, typename SecondCreator::
			                             ArgsTuple>;		

			auto readCreationData(SerializeInStream& in)
			{
				auto&& first = FirstCreator::readCreationData(in);
				return std::make_tuple(std::piecewise_construct, first, SecondCreator::readCreationData(in));
			}

			auto readCreationDataPlain(SerializeInStream& in, Formatter& format) {
                            auto &&first = FirstCreator::readCreationDataPlain(in, format);
                            return std::make_tuple(std::piecewise_construct, first, SecondCreator::readCreationDataPlain(in, format));
			}
		};

		template <class... Args>
		struct DefaultCreator
		{
			using ArgsTuple = std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>;
		
			ArgsTuple readCreationData(SerializeInStream& in)
			{
				ArgsTuple tuple;
				in >> tuple;
				return std::move(tuple);
			}

			ArgsTuple readCreationDataPlain(SerializeInStream& in, Formatter& format) {
                            ArgsTuple tuple;
                            readTuplePlain(in, tuple, format);
                            return std::move(tuple);
			}
		};

		/*namespace __creationhelper__impl__{
			template <class R, class T, class... _Ty>
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
			template <auto f, class R, class T, class... _Ty>
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

				R readCreationDataPlain(SerializeInStream& in, Formatter& format) {
                    std::tuple<std::remove_const_t<std::remove_reference_t<_Ty>>...> tuple;
                    in.readPlain(tuple, format);
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
