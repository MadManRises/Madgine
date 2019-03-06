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

		class MadgineObject;

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


	namespace Server
	{
		class ServerBase;
	}

}
