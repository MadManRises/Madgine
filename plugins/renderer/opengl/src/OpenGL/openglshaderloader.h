#pragma once

#include "Madgine/resources/resourceloader.h"

namespace Engine {
	namespace Render {

		struct OpenGLShaderLoader : Resources::ResourceLoader<OpenGLShaderLoader, OpenGLShader>
		{			
			OpenGLShaderLoader(Resources::ResourceManager &mgr);

			std::shared_ptr<OpenGLShader> loadImpl(ResourceType * res) override;
		};

	}
}

RegisterType(Engine::Render::OpenGLShaderLoader);