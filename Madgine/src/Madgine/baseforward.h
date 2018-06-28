#pragma once

namespace Engine
{
	class MadgineObject;

	namespace Scene
	{
		class SceneManagerBase;
		class SceneComponentBase;
		enum class ContextMask : unsigned int;

		namespace Entity
		{
			class Entity;
			class EntityComponentBase;
			class Transform;
			class Animation;
			class Mesh;
		}

		class Light;
	}

	namespace App
	{
		class Root;

		struct AppSettings;
		class Application;

		class FrameListener;
	}

	namespace UI
	{
		class GuiHandlerBase;
		class GameHandlerBase;
		class UIManager;
	}

	namespace Scripting
	{
		class GlobalScope;
		class GlobalAPIComponentBase;
	}

	namespace GUI
	{
		class GUISystem;
		struct WindowSizeRelVector;
		class WindowContainer;
		class Window;
		class Bar;
		class Button;
		class TextureDrawer;
		class Checkbox;
		class Combobox;
		class TabWindow;
		class Textbox;
		class Label;
	}

	namespace Resources
	{
		class ResourceManager;
		template <class Loader>
		class Resource;
		template <class T, class Data>
		class ResourceLoader;
		class ResourceBase;
	}

}
