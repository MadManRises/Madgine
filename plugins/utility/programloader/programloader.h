#pragma once

#include "Modules/resources/resourceloader.h"
#include "Modules/threading/workgroupstorage.h"

#include "program.h"

namespace Engine {
namespace Render {

    struct MADGINE_PROGRAMLOADER_EXPORT ProgramLoader : Resources::VirtualResourceLoaderBase<ProgramLoader, Program, std::list<Placeholder<0>>, Threading::WorkGroupStorage> {

        using Base = Resources::VirtualResourceLoaderBase<ProgramLoader, Program, std::list<Placeholder<0>>, Threading::WorkGroupStorage>;

        struct MADGINE_PROGRAMLOADER_EXPORT HandleType : Base::HandleType {

            using Base::HandleType::HandleType;
            HandleType(Base::HandleType handle)
                : Base::HandleType(handle)
            {
            }

			void create(const std::string &name, ProgramLoader *loader = nullptr);

            void bind(ProgramLoader *loader = nullptr);

            void setUniform(const std::string &var, int value, ProgramLoader *loader = nullptr);
            void setUniform(const std::string &var, const Matrix3 &value, ProgramLoader *loader = nullptr);
            void setUniform(const std::string &var, const Matrix4 &value, ProgramLoader *loader = nullptr);
            void setUniform(const std::string &var, const Vector3 &value, ProgramLoader *loader = nullptr);
        };

        ProgramLoader();

        virtual bool create(Program &program, const std::string &name) = 0;

        virtual void bind(Program &program) = 0;

        virtual void setUniform(Program &program, const std::string &var, int value) = 0;
        virtual void setUniform(Program &program, const std::string &var, const Matrix3 &value) = 0;
        virtual void setUniform(Program &program, const std::string &var, const Matrix4 &value) = 0;
        virtual void setUniform(Program &program, const std::string &var, const Vector3 &value) = 0;
    };

}
}