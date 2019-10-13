#pragma once

#include "Modules/resources/resourceloader.h"

namespace Engine {
	namespace Render {

		struct OpenGLShaderLoader : Resources::ResourceLoader<OpenGLShaderLoader, OpenGLShader, Resources::ThreadLocalResource>
		{			
			OpenGLShaderLoader();

			std::shared_ptr<OpenGLShader> loadImpl(ResourceType *res) override;
		};

	}
}