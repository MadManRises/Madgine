# Madgine - Build your own game engine

[![Build Status](http://www.madman-studios.com/jenkins/buildStatus/icon?job=Madgine%2Fmaster)](http://www.madman-studios.com/jenkins/job/Madgine/job/master/)

## Description

The Madgine is an Engine Building Framework, that can be used as a functional engine from the get go. It is written purely in C++ and uses the C++17-Standard. Its main focus is to provide a basic engine, in which every component can be easily adjusted or even replaced. This leaves developers with the freedom to spend time on their favorite parts of engine development and not loosing time creating e.g. platform abstractions. 

That being said the target audience of this project are at the moment engine developers as opposed to game developers, as the provided implementations are minimal and lack useful tooling. Hopefully, as time passes the available set of tools and plugins will grow, such that the engine can be used as a general purpose engine for game progammers and designers without having to know all details of the engine itself.

To be able to achieve that all parts of the engine are available for you to use in any kind of software and redistribution you fancy. This also implies that I can not sell copies of my software to support sustained development. If you are using this framework for your own business, please consider supporting the development of this project on [Patreon][] or reach out to me on my business e-mail: madmanrises at web dot de

### Why do you want to use the Madgine?

You are unsatisfied with the flexibility of other game engines? You want to build your own engine, but you are afraid it would be too big of a task? Then this framework might be exactly what you are looking for. It comes bundled with a lot of useful implementations, that are commonly used in game development including platform abstraction und thread management. Having a preset of those implementations can save a lot of development and debug time and allows to spend that time on developing those features that make your engine the best in the world. For an overview of all the available implementations please check the [Modules][] page.

## Features

* Low level platform-/hardware-abstraction
* Powerful plugin system
* Robust Toolchain using [CMake][]

## Documentation

There is currently no documentation in code. Please refer to the explanations of general the concepts in this docs in combination to the implementation. There is also a doxygen page, that lists all the interfaces and APIs. You can find it on my [homepage][Homepage] by selecting the `docs` on the respective branch. If you need further help or if the documents are outdated or lacking, please let me now either by raising an issue on [GitHub][] or by reaching out to me during a live coding stream on [Twitch][].

## Table Of Contents

* [Getting Started][]
* [Overview][]
    * [Concepts][]
        * [Meta Information][]
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

[LICENSE.md]: ../LICENSE.md
[Getting Started]: gettingStarted.md

[Overview]: overview.md

[Concepts]: concepts.md
[Meta Information]: meta.md

[Interfaces]: interfaces.md

[Modules]: modules.md
[Plugins & Unique Components]: plugins.md
[Signals & Slots]: signalslot.md
[KeyValue]:keyvalue.md
[Workgroups & Threading]: workgroup.md
[Event handling]: event.md
