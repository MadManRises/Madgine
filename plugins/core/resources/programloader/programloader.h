#pragma once

#include "Madgine/resources/resourceloader.h"
#include "Modules/threading/workgroupstorage.h"

#include "Generic/bytebuffer.h"

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
                setParameters({ &param, sizeof(T) }, index, loader);
            }
            void setParameters(const ByteBuffer &data, size_t index, ProgramLoader *loader = nullptr);

			void setDynamicParameters(const ByteBuffer &data, size_t index, ProgramLoader *loader = nullptr);
        };

        ProgramLoader();

        virtual bool create(Program &program, const std::string &name) = 0;

        virtual void setParameters(Program &program, const ByteBuffer &data, size_t index) = 0;

		virtual void setDynamicParameters(Program &program, const ByteBuffer &data, size_t index) = 0;
    };

}
}

RegisterType(Engine::Render::ProgramLoader);