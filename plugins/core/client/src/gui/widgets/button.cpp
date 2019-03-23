#include "../../clientlib.h"

#include "button.h"

#include "Interfaces/math/vector4.h"

#include "../vertex.h"

#include <iostream>

namespace Engine
{
	namespace GUI
	{
		void Button::setText(const std::string & text)
		{
		}

		SignalSlot::SignalStub<>& Button::clickEvent()
		{
			return mClicked;
		}

		std::vector<Vertex> Button::vertices(const Vector3 & screenSize)
		{
			std::vector<Vertex> result;

			Vector3 pos = (getAbsolutePosition() * screenSize) / screenSize;
			Vector3 size = (getAbsoluteSize() * screenSize) / screenSize;

			Vector4 color = mHovered ? Vector4{ 1.0f, 0.1f, 0.1f, 1.0f } : Vector4{0.4f, 0.4f, 0.4f, 1.0f};

			Vector3 v = pos;
			v.z = static_cast<float>(depth());
			result.push_back({ v, color });
			v.x += size.x;
			result.push_back({ v, color });
			v.y += size.y;
			result.push_back({ v, color });
			result.push_back({ v, color });
			v.x -= size.x;
			result.push_back({ v, color });
			v.y -= size.y;
			result.push_back({ v, color });
			return result;
		}

		bool Button::injectMouseEnter(const Input::PointerEventArgs & arg)
		{
			mHovered = true;
			return true;
		}

		bool Button::injectMouseLeave(const Input::PointerEventArgs & arg)
		{
			mHovered = false;
			return true;
		}

		void Button::emitClicked()
		{
			mClicked.emit();
		}
	}
}