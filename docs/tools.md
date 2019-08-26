# Tools

The plugin-based design of the Madgine-framework allows tools to be integrated directly into the engine and then be disabled for a release build. This has the advantage of getting easy access to all information, but at the same time increases the risk to introduce dependencies from core features into tools. This page describes some concepts that try to mitigate that risk. It also describes the rich basis implementation for tooling support already available in the framework.

## Rendering

The tools library of the Madgine is built on top of the wonderful [Dear ImGui](https://github.com/ocornut/imgui). It provides at the same time a powerful toolset to create immediate Mode GUIs and an abstraction level to the renderer, allowing all tools to be written (mostly) renderer-agnostic. The following image illustrates the rendering process with tools.


First the engine renders its content as if no tools exist, then imgui renders on top of that. That keeps rendering of Tool-Windows and ImGui-Elements as straightforward as it is in basic ImGui-Applications. Rendering into the game content is a little more complicated. The preferred way to do that is by rendering into the background of ImGui (see image). This keeps the seperation of Tools and Game intact. For 2D overlays this should be manageable. Rendering into a 3D scene can be more complicated. If possible, it is still the recommended way to calculate the positions and write into the background-buffer of ImGui. Alternatively, the entity-component-system can be used to add additional elements to the scene. If this is done, extra caution is necessary to not mix tools and engine code.