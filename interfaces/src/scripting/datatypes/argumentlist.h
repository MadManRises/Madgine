#pragma once

#include "valuetype.h"
#include "serialize/serializable.h"

namespace Engine {
	namespace Scripting {

		class INTERFACES_EXPORT ArgumentList :
			public std::vector<ValueType>,
			public Serialize::Serializable
		{
		public:
			using std::vector<ValueType>::vector;
			ArgumentList() = default;
			ArgumentList(lua_State *state, int count);
			template <class... T>
			ArgumentList(T&&... args) :
				ArgumentList({ ValueType(std::forward<T>(args))... })
			{
			}

			// Geerbt über Serializable
			virtual void readState(Serialize::SerializeInStream &) override;

			virtual void writeState(Serialize::SerializeOutStream &) const override;

			template <class... _Ty>
			bool parse(_Ty&... args) const {
				if (size() != sizeof...(_Ty))
					return false;
				return parseSeq(std::make_index_sequence<sizeof...(_Ty)>(), args...);
			}

			void pushToStack(lua_State *state) const;

		private:

			template <class... _Ty, size_t... Is>
			bool parseSeq(std::index_sequence<Is...>, _Ty&... args) const {

				using expander = bool[];
				expander valid{
					at(Is).is<_Ty>()...
				};
				for (int i = 0; i < sizeof...(_Ty); ++i)
					if (!valid[i])
						return false;

				(void)expander {
					(void(args = at(Is).as<_Ty>()), false)...
				};
				return true;
			}

			bool parseSeq(std::index_sequence<>) const {
				return true;
			}

			
		};

	}
}


