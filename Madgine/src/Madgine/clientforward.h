#pragma once


namespace Engine
{
	namespace App
	{
		struct ClientAppSettings;
		class ClientApplication;
	}

	namespace UI
	{
		class Handler;
		class GameHandlerBase;
		class GuiHandlerBase;
		class UIManager;
	}

	namespace GUI
	{
		class GUISystem;
		struct WindowSizeRelVector;
		class WindowContainer;
		class Window;
		class Bar;
		class Button;
		class SceneWindow;
		class Checkbox;
		class Combobox;
		class TabWindow;
		class Textbox;
		class Label;
		class Image;

		struct KeyEventArgs;
		struct MouseEventArgs;

		namespace MouseButton
		{
			enum MouseButton : unsigned char;
		}
	}

	namespace Scene
	{
		class OgreSceneManager;
		class SceneComponentBase;

		namespace Entity
		{
			class Entity;
			class EntityComponentBase;

			class OgreTransform;
			class OgreMesh;
			class OgreAnimation;
		}
	}

	namespace Math
	{
		class Bounds;
	}

	namespace Resources
	{
		class OgreTexturePtr;

		namespace Shading
		{
			class ShaderFactoryBase;
		}
	}

	namespace Util
	{
		class Profiler;
		class ProcessStats;
		class Util;
	}

	namespace Input
	{
		class InputHandler;
	}
}
