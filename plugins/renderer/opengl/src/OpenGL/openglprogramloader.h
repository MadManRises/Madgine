#pragma once

#include "Madgine/resources/resourceloader.h"

#include "programloader.h"

#include "util/openglprogram.h"

namespace Engine {
namespace Render {

    struct MADGINE_OPENGL_EXPORT OpenGLProgramLoader : Resources::VirtualResourceLoaderImpl<OpenGLProgramLoader, OpenGLProgram, ProgramLoader> {
        OpenGLProgramLoader();

        bool loadImpl(OpenGLProgram &program, ResourceDataInfo &info);
        void unloadImpl(OpenGLProgram &program, ResourceDataInfo &info);
        bool create(Program &program, const std::string &name) override;
        bool create(Program &program, const std::string &name, const CodeGen::ShaderFile &file) override;

        virtual void setParametersSize(Program &program, size_t index, size_t size) override;
        virtual WritableByteBuffer mapParameters(Program &program, size_t index) override;

        virtual void setInstanceDataSize(Program &program, size_t size) override;
        virtual void setInstanceData(Program &program, const ByteBuffer &data) override;

		virtual void setDynamicParameters(Program &program, size_t index, const ByteBuffer &data) override;
    };
}
}

RegisterType(Engine::Render::OpenGLProgramLoader);