#pragma once

namespace Engine {

	class MadgineObjectBase;

	namespace Scene {
		class SceneManager;

		namespace Entity {
			class Entity;
		}

		class Light;

	}

	namespace App {
		enum class ContextMask : unsigned int;
		struct AppSettings;
		class Application;
	}

	namespace UI {
		class GuiHandlerBase;
		class GameHandlerBase;
	}
}