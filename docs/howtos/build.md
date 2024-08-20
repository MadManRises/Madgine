\page Build Build Instructions

# Build Instructions

This article describes all the neccessary steps to compile your first application with the Madgine.


## Dependencies

The Madgine is built using CMake. It can be downloaded [here](https://cmake.org/download/). 
All source and library dependencies are included in the repository itself.
A compiler supporting C++20 is necessary.


## Setup The Madgine

Download the source code from [GitHub](https://github.com/MadManRises/Madgine).

There are two ways to use and compile the engine. 

### Setup Build From The Repository

The first one being to build the project using CMake from the top-level folder of the engine. This is useful if you intend to modify the engine code itself. No further CMake scripts are needed. 

### Setup Build From An External Project

The second method is to create a separate CMake project and reference the engine code from there. This allows you to separate your own work from the engine code, but keeps the benefit of building the engine source code together with your game/plugin code, allowing for better whole program optimization. All the necessary utility scripts for this method are provided. To setup the engine this way, create your own CMakeLists.txt for your game/plugin and add the following:
```cmake
include(<root folder of local madgine repository>/cmake/MadgineSDK.cmake)
```
To create the executable add the following line to the end of your CmakeLists.txt:
```cmake
create_launcher(<name>)
```

## Generating Project Files

Generate the build files using CMake for your preferred compiler/IDE. 

### Generating Using Visual Studio

Starting with Visual Studio 2019 the IDE has its own CMake tools. If you installed the plugin you can simply use "File > Open > Folder..." and select the project folder. After that 'MadgineLauncher.exe' should be the startup project by default. If you do not have the plugin installed you can use the standalone CMake installation to generate a solution file. 

There are no additional CMake settings necessary. Build the solution and start the executable (it is called what you provided to `create_launcher` or MadgineLauncher if building from the repository itself), which can be found in the `bin` folder of your build directory. 
Congratulations, you successfully compiled the Madgine Framework. 


## Working With The Engine

Once the engine successfully built, there are two possible ways of action. If you intend to work in the engine and start familiarizing yourself with it, please refer to the [User Manual](@ref Manual). If you want to learn how to write game code for the engine, please refer to [Writing your first UniqueComponent](@ref FirstUniqueComponent).


\addtogroup Howtos
\ref Build

