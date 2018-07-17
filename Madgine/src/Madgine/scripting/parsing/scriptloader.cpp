#include "../../baselib.h"
#include "scriptloader.h"
#include "../../resources/resourcemanager.h"

#include "../../core/root.h"


namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{
			ScriptLoader::ScriptLoader(Resources::ResourceManager &mgr) :
				ResourceLoader(mgr, { ".lua" }, true),
				mParser(mgr.root().luaState())
			{
			}

			ScriptLoader::~ScriptLoader()
			{
			}

			
			std::shared_ptr<MethodHolder> ScriptLoader::load(ResourceType *res)
			{
				std::ifstream stream(res->path());
				MethodHolder method{ mParser.parseScript(stream, res->name()) };
				if (method)
				{
					return std::make_shared<MethodHolder>(std::move(method));
				}else
				{
					return {};
				}
			}

		}
	}
}
