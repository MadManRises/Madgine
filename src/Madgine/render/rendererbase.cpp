#include "../clientlib.h"
#include "rendererbase.h"

#include "camera.h"

#include "../gui/guisystem.h"

#include "Interfaces/generic/keyvalueiterate.h"

namespace Engine {
	namespace Render {

		RendererBase::RendererBase(GUI::GUISystem *gui) :
			Scope(gui)
		{
		}

		RendererBase::~RendererBase()
		{
		}

		Camera * RendererBase::createCamera()
		{
			return mCameras.emplace_back(std::make_unique<Camera>(this)).get();
		}

		KeyValueMapList RendererBase::maps()
		{
			return Scope::maps().merge(mCameras);
		}

	}
}


