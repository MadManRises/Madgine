#pragma once

#include "widget.h"

#include "Modules/signalslot/signal.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT Button : public Widget<Button>
		{
		public:
			using Widget::Widget;
			virtual ~Button() = default;
			virtual void setText(const std::string& text);

			SignalSlot::SignalStub<> &clickEvent();

			std::pair<std::vector<Vertex>, uint32_t> vertices(const Vector3 &screenSize) override;

			bool injectPointerEnter(const Input::PointerEventArgs &arg) override;
			bool injectPointerLeave(const Input::PointerEventArgs &arg) override;

			bool injectPointerPress(const Input::PointerEventArgs &arg) override;
			bool injectPointerRelease(const Input::PointerEventArgs &arg) override;

			virtual WidgetClass getClass() const override;

		protected:
			void emitClicked();

		private:
			SignalSlot::Signal<> mClicked;

			bool mHovered = false;
			bool mClicking = false;
		};

	}
}
