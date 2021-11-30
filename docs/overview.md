\page Core Core Overview

\subpage Generic
\subpage Interfaces
\subpage Meta
\subpage Modules

# Core Overview

## Folder hierarchy

- cmake: cmake utility scripts
- docs: documentation (doxygen)
- launcher: executable/entry-point setup for all supported platforms
- plugins: set of plugins grouped by functionality
- samples: small working examples of using the framework
- shared: (external) libraries (note: not plugins)
- src: core libraries
- test: tests for core libraries


## Core Libraries

The framework is strongly plugin-oriented. It contains a set of core libraries that provide the necessary generic functionality needed in a working Madgine-Application:
- [Generic][]: header-only library. Mostly template code, including container traits
- [Meta][]: reflection and serialization/synchronization library
- [Interfaces][]: hardware/platform abstractions
- [Modules][]: threading and plugin functionality




This keeps the minimal footprint of the framework low and allows customizations to as much
parts of the code as possible.

![libraries](img/libraries.svg)

[Generic]: generic.md
[Meta]: meta.md
[Interfaces]: interfaces.md
[Modules]: modules.md

