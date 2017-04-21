#pragma once

namespace Engine {

	class MadgineObject;

	namespace Scene {
		class SceneManagerBase;
		enum class ContextMask : unsigned int;

		namespace Entity {
			class Entity;
		}

		class Light;

	}

	namespace App {
		
		struct AppSettings;
		class Application;
		class FrameListener;
	}

	namespace UI {
		class GuiHandlerBase;
		class GameHandlerBase;
	}
}