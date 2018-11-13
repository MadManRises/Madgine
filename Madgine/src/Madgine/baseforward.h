#pragma once

namespace Engine
{
	class MadgineObject;
	struct IndexHolder;

	struct UniqueComponentCollectorManager;
	struct ComponentRegistryBase;

	namespace Scene
	{
		class SceneManager;
		class SceneComponentBase;
		class Camera;
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
		class GlobalAPIComponentBase;
	}

	namespace Core
	{
		class Root;

		class FrameListener;
		class FrameLoop;
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
