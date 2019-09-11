#include "../clientlib.h"
#include "rendererbase.h"

#include "../gui/guisystem.h"

#include "Modules/reflection/classname.h"
#include "Modules/keyvalue/metatable_impl.h"

namespace Engine {
	namespace Render {

		RendererBase::RendererBase(GUI::GUISystem *gui) :
			mGui(gui)
		{
		}

		RendererBase::~RendererBase()
		{
		}

		/*App::Application & RendererBase::app(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mGui->app(init);
		}*/

		const MadgineObject * RendererBase::parent() const
		{
			return mGui;
		}

	}
}



METATABLE_BEGIN(Engine::Render::RendererBase)
METATABLE_END(Engine::Render::RendererBase)

RegisterType(Engine::Render::RendererBase);