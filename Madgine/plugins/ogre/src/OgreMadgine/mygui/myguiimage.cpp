#include "../ogrelib.h"

#include "myguiimage.h"

#include "myguisystem.h"

namespace Engine
{
	namespace GUI
	{
		MyGUIImage::MyGUIImage(const std::string& name, MyGUIWindow* parent) :
		MyGUIWindow(name, parent),
		Window(name, parent),
		Image(name, parent)
		{
			mImage = parent->widget()->createWidget<MyGUI::StaticImage>("ImageBox", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		MyGUIImage::MyGUIImage(const std::string& name, MyGUISystem& system) :
		MyGUIWindow(name, system),
		Window(name, system),
		Image(name, system)
		{
			mImage = system.rootWidget()->createWidget<MyGUI::StaticImage>("ImageBox", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		void MyGUIImage::setImage(const std::string& name)
		{
			mImage->setImageTexture(name);
		}

		Class MyGUIImage::getClass()
		{
			return Class::IMAGE_CLASS;
		}

		MyGUI::Widget* MyGUIImage::widget() const
		{
			return mImage;
		}
	}
}