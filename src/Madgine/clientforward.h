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
		class Widget;
		class Bar;
		class Button;
		class SceneWindow;
		class Checkbox;
		class Combobox;
		class TabWidget;
		class Textbox;
		class Label;
		class Image;
		class TopLevelWindow;

		enum class Class;

		struct WindowOverlay;

		struct Vertex;

	}

	namespace Scene
	{
		class SceneComponentBase;

		namespace Entity
		{
			class Entity;
			class EntityComponentBase;
		}
	}

	namespace Math
	{
		class Bounds;
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
		class InputListener;

		struct KeyEventArgs;
		struct MouseEventArgs;

		namespace MouseButton
		{
			enum MouseButton : unsigned char;
		}
	}

	namespace Render {
		class RendererBase;
		class RenderWindow;
		struct RenderTarget;
		class Camera;
	}

}
