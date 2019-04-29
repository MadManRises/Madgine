#include "../interfaceslib.h"

#include "threadlocal.h"

#if EMSCRIPTEN
#include <pthread.h>
#endif

namespace Engine
{
	namespace Threading
	{

		static std::vector<Any> &sLocalVariableConstructors()
		{
			static std::vector<Any> dummy;
			return dummy;
		}
		
		struct VariableStore
		{
			VariableStore()
			{
				for (const Any &ctor : sLocalVariableConstructors())
				{
					mVariables.emplace_back(ctor);
				}
			}

			std::vector<Any> mVariables;
		};

#if !EMSCRIPTEN

		static VariableStore &sLocalVariables()
		{
			static thread_local VariableStore dummy;
			return dummy;
		}
#else

		static pthread_key_t &sKey()
		{
			static pthread_key_t dummy;
			return dummy;
		}

		static VariableStore &sLocalVariables()
		{
			VariableStore *store = static_cast<VariableStore*>(pthread_getspecific(sKey()));
			assert(store);
			return *store;
		}

		static void destructTLS(void *store)
		{
			delete static_cast<VariableStore*>(store);
		}
#endif
		

		size_t ThreadLocalStorage::registerLocalVariable(Any &&ctor)
		{
			sLocalVariableConstructors().emplace_back(std::move(ctor));
			return sLocalVariableConstructors().size() - 1;
		}

		const Any& ThreadLocalStorage::localVariable(size_t index)
		{
			while (sLocalVariables().mVariables.size() <= index)
			{
				sLocalVariables().mVariables.emplace_back(sLocalVariableConstructors()[sLocalVariables().mVariables.size()]);
			}
			return sLocalVariables().mVariables.at(index);
		}

		void ThreadLocalStorage::init()
		{
#if EMSCRIPTEN	
			static int result = pthread_key_create(&sKey(), &destructTLS);
			assert(result == 0);

			pthread_setspecific(sKey(), new VariableStore);
#endif

			while (sLocalVariables().mVariables.size() < sLocalVariableConstructors().size())
			{
				sLocalVariables().mVariables.emplace_back(sLocalVariableConstructors()[sLocalVariables().mVariables.size()]);
			}
		}

	}
}