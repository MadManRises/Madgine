#include "../clientlib.h"
#include "rendererbase.h"

#include "../gui/guisystem.h"

namespace Engine {
	namespace Render {

		RendererBase::RendererBase(GUI::GUISystem *gui) :
			Scope(gui),
			mGui(gui)
		{
		}

		RendererBase::~RendererBase()
		{
		}

		App::Application & RendererBase::app(bool init)
		{
			if (init)
			{
				checkInitState();
			}
			return mGui->app(init);
		}

		const Core::MadgineObject * RendererBase::parent() const
		{
			return mGui;
		}

	}
}


