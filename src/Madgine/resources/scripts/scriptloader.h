#pragma once
#include "../resourceloader.h"
#include "Interfaces/scripting/parsing/scriptparser.h"


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


				virtual std::shared_ptr<MethodHolder> load(ResourceType* res) override;

			private:

				ScriptParser mParser;
				
			};
		}
	}
}

RegisterType(Engine::Scripting::Parsing::ScriptLoader);