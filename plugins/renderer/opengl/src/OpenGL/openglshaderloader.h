#pragma once

#include "Madgine/resources/resourceloader.h"

#include "Modules/threading/workgroupstorage.h"

#include "util/openglshader.h"

namespace Engine {
namespace Render {

    struct OpenGLShaderLoader : Resources::ResourceLoader<OpenGLShaderLoader, OpenGLShader, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {
        OpenGLShaderLoader();

        bool loadImpl(OpenGLShader &shader, ResourceDataInfo &info);
        void unloadImpl(OpenGLShader &shader, ResourceDataInfo &info);
    };

}
}

RegisterType(Engine::Render::OpenGLShaderLoader);