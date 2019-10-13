#pragma once

namespace Engine
{
	namespace GUI
	{
		enum class WidgetClass
		{
			WIDGET_CLASS,
			SCENEWINDOW_CLASS,
			COMBOBOX_CLASS,
			TABWIDGET_CLASS,
			CHECKBOX_CLASS,
			TEXTBOX_CLASS,
			BUTTON_CLASS,
			LAYOUT_CLASS,
			LABEL_CLASS,
			BAR_CLASS,
			IMAGE_CLASS,
			CLASS_COUNT
		};

		constexpr const char *widgetClassNames[] = {
                    "Widget",
                    "Scene Window",
                    "Combo Box",
                    "Tab Widget",
                    "Checkbox",
                    "Textbox",
                    "Button",
                    "Layout",
                    "Label",
                    "Bar",
                    "Image"
                };
	}
}
