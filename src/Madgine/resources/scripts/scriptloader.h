#pragma once
#include "../resourceloader.h"
#include "Modules/scripting/parsing/scriptparser.h"


namespace Engine
{
	namespace Scripting
	{
		namespace Parsing
		{
			class MADGINE_BASE_EXPORT ScriptLoader : public Resources::ResourceLoader<ScriptLoader, MethodHolder>
			{
			public:
				ScriptLoader(Resources::ResourceManager &mgr);
				virtual ~ScriptLoader();


				virtual std::shared_ptr<MethodHolder> loadImpl(ResourceType* res) override;

			private:

				ScriptParser mParser;
				
			};
		}
	}
}