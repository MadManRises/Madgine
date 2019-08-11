#include "../../baselib.h"
#include "scriptloader.h"
#include "../resourcemanager.h"

#include "Modules/reflection/classname.h"
#include "Modules/keyvalue/metatable_impl.h"

UNIQUECOMPONENT(Engine::Scripting::Parsing::ScriptLoader);

namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{
			ScriptLoader::ScriptLoader() :
				ResourceLoader({ ".lua" }, true)
			{
			}

			ScriptLoader::~ScriptLoader()
			{
			}

			
			std::shared_ptr<MethodHolder> ScriptLoader::loadImpl(ResourceType *res)
			{
				std::ifstream stream(res->path().str());
				std::optional<MethodHolder> method{ mParser.parseScript(stream, res->name()) };
				if (method)
				{
					return std::make_shared<MethodHolder>(std::move(*method));
				}else
				{
					return {};
				}
			}

		}
	}
}


METATABLE_BEGIN(Engine::Scripting::Parsing::ScriptLoader)
METATABLE_END(Engine::Scripting::Parsing::ScriptLoader)

RegisterType(Engine::Scripting::Parsing::ScriptLoader);