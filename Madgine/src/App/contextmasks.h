#pragma once

namespace Engine{
namespace App{

enum class ContextMask : size_t{
	NoContext = 0x0,
    SceneContext = 0x1,
	AnyContext = 0x2
};

inline bool operator & (ContextMask first, ContextMask second) {
	return (((size_t)first) & ((size_t)second)) != 0;
}

inline ContextMask operator | (ContextMask first, ContextMask second) {
	return (ContextMask)(((size_t)first) | ((size_t)second));
}

}
}


