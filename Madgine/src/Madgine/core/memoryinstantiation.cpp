#include "../interfaceslib.h"

#include "memory.h"

void *operator new (size_t s) {
	return Engine::Core::MemoryTracker::allocate(s);
}

void *operator new [](size_t s) {
	return operator new (s);
}

void operator delete(void *p) {
	Engine::Core::MemoryTracker::deallocate(p);
}

void operator delete [](void *p) {
	operator delete (p);
}