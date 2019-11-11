#include "fontloaderlib.h"

#include "Modules/keyvalue/metatable_impl.h"

#include "Modules/resources/resource.h"

#include "fontloader.h"

using FontResource = Engine::Resources::ThreadLocalResource<Engine::Font::Font>;

METATABLE_BEGIN_BASE(FontResource, Engine::Resources::ResourceBase)
METATABLE_END(FontResource)

RegisterType(Engine::Font::FontLoader)
METATABLE_BEGIN(Engine::Font::FontLoader)
METATABLE_END(Engine::Font::FontLoader)