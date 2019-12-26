#include "programloaderlib.h"

#include "programloader.h"

#include "Modules/keyvalue/metatable_impl.h"
#include "Modules/reflection/classname.h"

METATABLE_BEGIN(Engine::Render::ProgramLoader)
METATABLE_END(Engine::Render::ProgramLoader)

METATABLE_BEGIN_BASE(Engine::Render::ProgramLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::ProgramLoader::ResourceType)

RegisterType(Engine::Render::ProgramLoader)

    namespace Engine
{
    namespace Render {

        ProgramLoader::ProgramLoader()
            : VirtualResourceLoaderBase(std::vector<std::string> {})
        {
        }

        void ProgramLoader::HandleType::create(const std::string &name, ProgramLoader *loader)
        {
            *this = ProgramLoader::loadManual(
                name, {}, [=](ProgramLoader *loader, Program &program, const ProgramLoader::ResourceType *res) { return loader->create(program, name); }, {},
                loader);
        }

        void ProgramLoader::HandleType::bind(ProgramLoader *loader)
        {
            if (!loader)
                loader = &ProgramLoader::getSingleton();
            loader->bind(getData(*this, loader));
        }

        void ProgramLoader::HandleType::setUniform(const std::string &var, int value, ProgramLoader *loader)
        {
            if (!loader)
                loader = &ProgramLoader::getSingleton();
            loader->setUniform(getData(*this, loader), var, value);
        }

        void ProgramLoader::HandleType::setUniform(const std::string &var, const Matrix3 &value, ProgramLoader *loader)
        {
            if (!loader)
                loader = &ProgramLoader::getSingleton();
            loader->setUniform(getData(*this, loader), var, value);
        }

        void ProgramLoader::HandleType::setUniform(const std::string &var, const Matrix4 &value, ProgramLoader *loader)
        {
            if (!loader)
                loader = &ProgramLoader::getSingleton();
            loader->setUniform(getData(*this, loader), var, value);
        }

        void ProgramLoader::HandleType::setUniform(const std::string &var, const Vector3 &value, ProgramLoader *loader)
        {
            if (!loader)
                loader = &ProgramLoader::getSingleton();
            loader->setUniform(getData(*this, loader), var, value);
        }
    }
}