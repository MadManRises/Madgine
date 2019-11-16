#pragma once

#include "widget.h"

namespace Engine
{
namespace Widgets
	{
    class MADGINE_WIDGETS_EXPORT Combobox : public Widget<Combobox>
		{
		public:
			using Widget::Widget;
			virtual ~Combobox() = default;

			virtual void addItem(const std::string& text);
			virtual void clear();

			virtual void setText(const std::string& s);
			virtual std::string getText();

			virtual WidgetClass getClass() const override;
		};
	}
}
