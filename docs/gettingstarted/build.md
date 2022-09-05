\page Build Build Instructions

# Build Instructions

This article describes all the neccessary steps to compile your first application with the Madgine.


## Dependencies

The Madgine is built using CMake. It can be downloaded [here](https://cmake.org/download/). 
All source and library dependencies are included in the repository itself.
A compiler supporting C++20 is necessary.


## Building The Madgine

Download the source code from [GitHub](https://github.com/MadManRises/Madgine).

Even though you can compile the code directly from within the repository, it is recommended to use the Madgine as a library from source. All the necessary utility scripts are provided. To setup the library as source library, create your own CMakeLists.txt for your application and add the following:
```cmake
include(<root folder of local madgine repository>/cmake/MadgineSDK.cmake)
```
To create the executable add the following line to the end of your CmakeLists.txt:
```cmake
create_launcher(<name>)
```

Generate the build files using CMake for your preferred compiler/IDE. There are no additional CMake settings necessary. Build the solution and start the executable (it is called what you provided to `create_launcher` or MadgineLauncher if building from the repository itself), which can be found in the `bin` folder of your build directory. 
Congratulations, you successfully compiled the Madgine Framework. 


## Working With The Engine

Once the engine successfully built, there are two possible ways of action. If you intend to work in the engine and start familiarizing yourself with it, please refer to the [User Manual](@ref Manual). If you want to learn how to write code for the engine, please refer to [Writing your first UniqueComponent](@ref FirstUniqueComponent).
