#pragma once

#include "templates.h"

namespace Engine {
	namespace Serialize {

		template <bool areValueTypes, class... Args>
		class CreationHelper;

		template <class... Args>
		struct _CreationHelper {
			typedef CreationHelper<all_of<_isValueType<Args>::value...>::value, Args...> type;
		};


		template <class... Args>
		using Creator = typename _CreationHelper<Args...>::type;
		



		template <class... Args>
		class CreationHelper<true, Args...> {
		protected:
			typedef std::tuple<Args...> ArgsTuple;

			ArgsTuple defaultCreationData(SerializeInStream &in) {
				ArgsTuple tuple;
				TupleSerializer::readTuple(tuple, in);
				return tuple;
			}
		};

		template <class... Args>
		class CreationHelper<false, Args...> {
		protected:
			typedef std::tuple<Args...> ArgsTuple;

			ArgsTuple defaultCreationData(SerializeInStream &in) {
				throw 0;
			}
		};

	}
}