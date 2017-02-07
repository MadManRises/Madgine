# Overview

The Madgine-Design can be split semantically into two parts:

- The generic template-based layer
- The logic layer

## The Generic Layer

The generic layer offers functionality, that is shared by many different components, that are distincted in the logic layer:

- Serialization
- Scripting APIs.

### Serialization

The serialization-component offers a collection of classes to enable serialization and synchronisation in a server-client manner. 
The aim is, that there has to be written exactly one version of the game logic, which will be used for any executables of the game (eg. client, headless server).
To make a custom class serializable it has to inherit the class SerializableUnit. 
The specification which data-members are used for serialization is done by using one of the template classes offered. 
Those include wrappers for single data members, aswell as implementations of the most common stl containers.   


On the other hand the logic layer models the different parts of the engine:

- GUI
- UI (Handlers)
- 3D-Scene
- Scripting
- Input

