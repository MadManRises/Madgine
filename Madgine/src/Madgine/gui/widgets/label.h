#pragma once

#include "widget.h"
#include "../../generic/virtualbase.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT Label : public Scripting::Scope<Label, VirtualBase<Widget>>
		{
		public:
			using Scope::Scope;
			virtual ~Label() = default;

			virtual std::string getText() = 0;
			virtual void setText(const std::string& text) = 0;

			KeyValueMapList maps() override;
		};
	}
}
