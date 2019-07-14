#pragma once

#include "Madgine/resources/resourceloader.h"

namespace Engine {
	namespace Render {

		struct OpenGLMeshLoader : Resources::ResourceLoader<OpenGLMeshLoader, OpenGLMeshData, Resources::ThreadLocalResource>
		{
			OpenGLMeshLoader(Resources::ResourceManager &mgr);

			std::shared_ptr<OpenGLMeshData> loadImpl(ResourceType * res) override;
		};

	}
}