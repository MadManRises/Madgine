#include "../../clientlib.h"

#include "image.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

namespace Engine
{
	namespace GUI
	{
		void Image::setImage(const std::string & name)
		{
		}
		std::string Image::getImage() const
		{
			return std::string();
		}
	}
}

METATABLE_BEGIN(Engine::GUI::Image)
METATABLE_END(Engine::GUI::Image)

RegisterType(Engine::GUI::Image);