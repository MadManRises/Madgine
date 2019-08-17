# Overview

The framework is strongly plugin-oriented. It contains only two fixed libraries that need to be included
in a working Madgine-Application: Interfaces and Modules. These contain abstract concept implementations
(see below for details) that are not necessarily tied to game development, but at the same time are used
often in different parts of the code, which makes it not suitable for plugins. These libraries also contain
the full [Plugins][] system. Further features and implementations can then be included as desired using 
that system. This keeps the minimal footprint of the framework low and allows customisations to as much
parts of the code as possible.

![libraries](img/libraries.svg)


[Plugins]: plugins.md
