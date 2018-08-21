#include "../ogrelib.h"

#include "myguiimage.h"

#include "myguitoplevelwindow.h"

namespace Engine
{
	namespace GUI
	{
		MyGUIImage::MyGUIImage(const std::string& name, MyGUIWidget* parent) :
			Widget(name, parent),
		Image(name, parent),
			MyGUIWidget(name, parent)
		{
			mImage = parent->widget()->createWidget<MyGUI::StaticImage>("ImageBox", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		MyGUIImage::MyGUIImage(const std::string& name, MyGUITopLevelWindow& window) :
			Widget(name, window),
		Image(name, window),
			MyGUIWidget(name, window)
		{
			mImage = window.rootWidget()->createWidget<MyGUI::StaticImage>("ImageBox", 0, 0, 0, 0, MyGUI::Align::Default, name);
		}

		void MyGUIImage::setImage(const std::string& name)
		{
			mImage->setImageTexture(name);
		}

		std::string MyGUIImage::getImage() const
		{
			return mImage->_getTextureName();
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