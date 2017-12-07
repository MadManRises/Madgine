#pragma once

#include "../serializable.h"
#include "../serializableunit.h"

namespace Engine
{
	namespace Serialize
	{

		namespace __serializedmapper__impl__{

			template <typename T, typename U>
			struct decay_equiv :
				std::is_same<typename std::decay<T>::type, typename std::decay<U>::type>::type
			{
			};

			template <class T, class Return, class Argument, Return(T::*g)() const, void (T::*s)(Argument)>
			class SerializedMapperImpl : public Serializable
			{
			public:
				SerializedMapperImpl() :
					mParent(dynamic_cast<T*>(unit()))
				{
					assert(mParent);
				}

				void readState(SerializeInStream& in) override
				{
					std::decay_t<Argument> arg;
					in >> arg;
					(mParent ->* s)(arg);
				}

				void writeState(SerializeOutStream& out) const override
				{
					out << (mParent ->* g)();
				}


			private:
				T* mParent;
			};

			template <class T, class Return, class Argument>
			struct SerializedMapperWrapper
			{
				template <Return (T::*g)() const, void (T::*s)(Argument)>
				using impl = SerializedMapperImpl<T, Return, Argument, g, s>;
			};

			template <class T, class Return, class Argument>
			std::enable_if_t<decay_equiv<Return, Argument>::value, SerializedMapperWrapper<T, Return, Argument>>
			serializedMapperHelper(Return (T::*)() const, void (T::*)(Argument));

			template <typename G, G g, typename S, S s>
			using SerializedMapperDeduce = decltype(serializedMapperHelper(g, s));

			template <typename G, G g, typename S, S s>
			struct SerializedMapperType
			{
				typedef typename SerializedMapperDeduce<G, g, S, s>::template impl<g, s> type;
			};

		}

		template <typename G, G g, typename S, S s>
		using SerializedMapper = typename __serializedmapper__impl__::SerializedMapperType<G, g, S, s>::type;
	}
}
