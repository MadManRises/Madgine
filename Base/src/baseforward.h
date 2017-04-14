#pragma once

namespace Engine {

	class MadgineObject;

	namespace Scene {
		class SceneManagerBase;

		namespace Entity {
			class Entity;
		}

		class Light;

	}

	namespace App {
		enum class ContextMask : unsigned int;
		struct AppSettings;
		class Application;
		class FrameListener;
	}

	namespace UI {
		class GuiHandlerBase;
		class GameHandlerBase;
	}
}