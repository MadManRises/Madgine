#pragma once

#include "Modules/resources/resourceloader.h"

#include "programloader.h"

#include "util/openglprogram.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLProgramLoader : Resources::VirtualResourceLoaderImpl<OpenGLProgramLoader, OpenGLProgram, ProgramLoader> {
        OpenGLProgramLoader();

        bool loadImpl(OpenGLProgram &program, ResourceType *res);
        void unloadImpl(OpenGLProgram &program, ResourceType *res);
        bool create(Program &program, const std::string &name) override;

        virtual void setParameters(Program &program, const void *data, size_t size, size_t index) override;
    };
}
}