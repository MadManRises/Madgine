#pragma once

namespace Engine {
namespace Serialize {

struct TypeAnnotation {

	template <typename T>
    TypeAnnotation(type_holder_t<T>)
        : mType(&serializeTable<T>())
    {
    }

	const SerializeTable *mType;
};

}
}