#pragma once

#include "Interfaces/reflection/classname.h"
#include "../core/madgineobject.h"
#include "Interfaces/scripting/types/scope.h"

namespace Engine {
	namespace Render {

		class MADGINE_CLIENT_EXPORT RendererBase : public Core::MadgineObject, public Scripting::Scope<RendererBase> {
		public:			
			RendererBase(GUI::GUISystem *gui);
			RendererBase(const RendererBase&) = delete;
			virtual ~RendererBase();

			void operator=(const RendererBase&) = delete;

			virtual std::unique_ptr<RenderWindow> createWindow(GUI::TopLevelWindow *w) = 0;
			Camera *createCamera();

			KeyValueMapList maps() override;

		private:
			std::vector<std::unique_ptr<Camera>> mCameras;
		};

	}
}

RegisterClass(Engine::Render::RendererBase);