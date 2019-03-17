#include "../clientlib.h"
#include "rendererbase.h"

#include "../gui/guisystem.h"

namespace Engine {
	namespace Render {

		RendererBase::RendererBase(GUI::GUISystem *gui) :
			Scope(gui)
		{
		}

		RendererBase::~RendererBase()
		{
		}

	}
}


