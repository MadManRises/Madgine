#pragma once

#include "Modules/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

namespace Engine {
namespace Render {

    struct OpenGLShaderLoader : Resources::ResourceLoader<OpenGLShaderLoader, OpenGLShader, std::vector<Placeholder<0>>, Threading::WorkGroupStorage> {
        OpenGLShaderLoader();

        bool loadImpl(OpenGLShader &shader, ResourceType *res);
        void unloadImpl(OpenGLShader &shader, ResourceType *res);
    };

}
}