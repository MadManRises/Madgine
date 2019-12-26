#include "fontloaderlib.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "fontloader.h"

METATABLE_BEGIN_BASE(Engine::Font::FontLoader::ResourceType, Engine::Resources::ResourceBase)
METATABLE_END(Engine::Font::FontLoader::ResourceType)

RegisterType(Engine::Font::FontLoader)
METATABLE_BEGIN(Engine::Font::FontLoader)
METATABLE_END(Engine::Font::FontLoader)