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

        virtual void bind(Program &program) override;

        virtual void setUniform(Program &program, const std::string &var, int value) override;
        virtual void setUniform(Program &program, const std::string &var, const Matrix3 &value) override;
        virtual void setUniform(Program &program, const std::string &var, const Matrix4 &value) override;
        virtual void setUniform(Program &program, const std::string &var, const Vector3 &value) override;
    };
}
}