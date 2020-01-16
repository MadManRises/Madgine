#include "fontloaderlib.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "fontloader.h"

METATABLE_BEGIN_BASE(Engine::Render::FontLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Render::FontLoader::ResourceType)

RegisterType(Engine::Render::FontLoader)
METATABLE_BEGIN(Engine::Render::FontLoader)
METATABLE_END(Engine::Render::FontLoader)