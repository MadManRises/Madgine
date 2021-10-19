#include "clickbricklib.h"

#include "brick.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"


ENTITYCOMPONENT_IMPL(Brick, ClickBrick::Scene::Brick)

METATABLE_BEGIN(ClickBrick::Scene::Brick)
METATABLE_END(ClickBrick::Scene::Brick)

SERIALIZETABLE_BEGIN(ClickBrick::Scene::Brick)
SERIALIZETABLE_END(ClickBrick::Scene::Brick)



namespace ClickBrick {
namespace Scene {


}
}
