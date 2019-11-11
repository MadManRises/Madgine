#pragma once

#include "Modules/resources/resourceloader.h"

#include "programloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLProgramLoader : Resources::VirtualResourceLoaderImpl<OpenGLProgramLoader, ProgramLoader> {
        OpenGLProgramLoader();

        std::shared_ptr<Program> loadImpl(ResourceType *res) override;
        std::shared_ptr<Program> create(const std::string &name) override;

        virtual void bind(Program &program) override;

        virtual void setUniform(Program &program, const std::string &var, int value) override;
        virtual void setUniform(Program &program, const std::string &var, const Matrix3 &value) override;
        virtual void setUniform(Program &program, const std::string &var, const Matrix4 &value) override;
        virtual void setUniform(Program &program, const std::string &var, const Vector3 &value) override;
    };
}
}