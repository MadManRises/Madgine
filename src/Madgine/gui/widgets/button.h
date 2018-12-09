#pragma once

#include "widget.h"

#include "Interfaces/signalslot/signal.h"

namespace Engine
{
	namespace GUI
	{
		class MADGINE_CLIENT_EXPORT Button : public Widget
		{
		public:
			using Widget::Widget;
			virtual ~Button() = default;
			virtual void setText(const std::string& text);

			SignalSlot::SignalStub<> &clickEvent();

			std::vector<Vertex> vertices(const Vector3 &screenSize) override;

			bool injectMouseEnter(const Input::MouseEventArgs &arg) override;
			bool injectMouseLeave(const Input::MouseEventArgs &arg) override;

		protected:
			void emitClicked();

		private:
			SignalSlot::Signal<> mClicked;

			bool mHovered = false;
		};

	}
}
