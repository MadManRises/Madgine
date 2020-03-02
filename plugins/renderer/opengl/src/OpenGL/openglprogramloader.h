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

        virtual void setParameters(Program &program, const ByteBuffer &data, size_t index) override;

		virtual void setDynamicParameters(Program &program, const ByteBuffer &data, size_t index) override;
    };
}
}