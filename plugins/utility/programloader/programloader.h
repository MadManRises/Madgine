#pragma once

#include "Modules/resources/resourceloader.h"

namespace Engine {
namespace Render {

    struct MADGINE_PROGRAMLOADER_EXPORT ProgramLoader : Resources::VirtualResourceLoaderBase<ProgramLoader, Program, Resources::ThreadLocalResource> {
        ProgramLoader();

		virtual std::shared_ptr<Program> create(const std::string &name) = 0;

        virtual void bind(Program &program) = 0;

        virtual void setUniform(Program &program, const std::string &var, int value) = 0;
        virtual void setUniform(Program &program, const std::string &var, const Matrix3 &value) = 0;
        virtual void setUniform(Program &program, const std::string &var, const Matrix4 &value) = 0;
        virtual void setUniform(Program &program, const std::string &var, const Vector3 &value) = 0;
    };

}
}