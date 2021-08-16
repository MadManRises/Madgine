# Madgine - Build your own game engine

[![Build Status](http://www.madman-studios.com/jenkins/buildStatus/icon?job=Madgine%2Fmaster)](http://www.madman-studios.com/jenkins/job/Madgine/job/master/)

## Description

Originally designed as a game engine framework the Madgine has evolved into a collection of libraries and utilities that can improve application development drastically. The combination of all provided libraries can be used to create an empty real-time application template with lots of tools and features already provided. It is written purely in C++ and uses the C++20-Standard. Its main focus is to provide minimal basic building blocks, which can be easily adjusted or even replaced. This leaves developers with the freedom to spend time on the parts of the application they want to invest in and not loosing time creating e.g. platform abstractions. 

To be able to achieve that all parts of the engine are available for you to use in any kind of software and redistribution you fancy (see [LICENSE.md]). This also implies that I cannot sell copies of my software to support sustained development. If you are using this framework for your own business, please consider supporting the development of this project on [Patreon][] or reach out to me on my business e-mail: madmanrises at web dot de

## Features

* Low level platform-/hardware-abstraction
* Powerful plugin system
* Run-time data introspection using the builtin reflection library
* Fully integrated object-oriented serialization/synchronization library
* Robust Toolchain using [CMake][]

## Documentation

There is currently no documentation in code. Please refer to the explanations of general the concepts in this docs in combination to the implementation. There is also a [doxygen page][Doxygen], that lists all the interfaces and APIs. If you need further help or if the documents are outdated or lacking, please let me now either by raising an issue on [GitHub][] or by reaching out to me during a live coding stream on [Twitch][].

## Table Of Contents

* [Getting Started][]
* [Overview][]
    * [Generic][]
    * [Meta][]
    * [Interfaces][]
    * [Modules][]
        * [Plugins & Unique Components][]
        * [Signals & Slots][]
        * [KeyValue][]
        * [Workgroups & Threading][]
        * [Event handling][]


### License

The Madgine is licensed under the MIT License, see [LICENSE.md][] for more information.

[Homepage]: http://www.madmanrises.com
[CMake]: https://www.cmake.org
[GitHub]: https://github.com/MadManRises/Madgine/issues
[Twitch]: https://www.twitch.tv/madmanrises
[Patreon]: https://www.patreon.com/madmanstudios
[Doxygen]: http://www.madmanrises.com/latest/doc/

[LICENSE.md]: ../LICENSE.md
[Getting Started]: gettingStarted.md

[Overview]: overview.md

[Generic]: generic.md

[Meta]: meta.md

[Interfaces]: interfaces.md

[Modules]: modules.md
[Plugins & Unique Components]: plugins.md
[Signals & Slots]: signalslot.md
[KeyValue]:keyvalue.md
[Workgroups & Threading]: workgroup.md
[Event handling]: event.md
