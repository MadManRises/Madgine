#pragma once

#include "widget.h"
#include "Interfaces/generic/virtualbase.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT Label : public Scripting::Scope<Label, Widget>
		{
		public:
			using Scope::Scope;
			virtual ~Label() = default;

			virtual std::string getText();
			virtual void setText(const std::string& text);

			KeyValueMapList maps() override;
		};
	}
}


RegisterType(Engine::GUI::Label);