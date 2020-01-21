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

            template <typename T>
            void setParameters(const T &param, size_t index, ProgramLoader *loader = nullptr)
            {
                setParameters(&param, sizeof(T), index, loader);
            }
            void setParameters(const void *data, size_t size, size_t index, ProgramLoader *loader = nullptr);
        };

        ProgramLoader();

        virtual bool create(Program &program, const std::string &name) = 0;

        virtual void setParameters(Program &program, const void *data, size_t size, size_t index) = 0;
    };

}
}