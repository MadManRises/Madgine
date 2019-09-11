#pragma once

namespace Engine
{


	namespace Scene
	{
		class SceneManager;
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

		class LightManager;
		class Light;

		class Camera;
	}

	namespace App
	{
		struct AppSettings;
		class Application;			
		class GlobalAPIBase;
	}

	namespace Core
	{
		class Root;

	}






	namespace Server
	{
		class ServerBase;
	}

}
