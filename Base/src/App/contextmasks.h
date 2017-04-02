#pragma once

namespace Engine{
namespace App{

  /**
   * Describes the current context of the game. This is used to decide which components should be updated each frame.
   * The current root-UI::GuiHandler determines the current Context. That is matched against the ContextMask of the UI::GameHandlers and
   * the Scene::SceneManagerBase to decide wether it gets updated or not.
   */
	enum class ContextMask : unsigned int{
		NoContext = 0x0, /*! No specific context */
		SceneContext = 0x1, /*! All elements of the scene will be updated. */
		AnyContext = 0x2 /*! Use only as mask, not as actual context. Will be always matched. */
	};

inline bool operator & (ContextMask first, ContextMask second) {
	return (((size_t)first) & ((size_t)second)) != 0;
}

inline ContextMask operator | (ContextMask first, ContextMask second) {
	return (ContextMask)(((size_t)first) | ((size_t)second));
}

}
}
