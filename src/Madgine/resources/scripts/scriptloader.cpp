#include "../../baselib.h"
#include "scriptloader.h"
#include "../resourcemanager.h"

#include "../../core/root.h"

#include "Interfaces/reflection/classname.h"

UNIQUECOMPONENT(Engine::Scripting::Parsing::ScriptLoader);

namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{
			ScriptLoader::ScriptLoader(Resources::ResourceManager &mgr) :
				ResourceLoader(mgr, { ".lua" }, true),
				mParser()
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
