# Labyrinth #
Labyrinth is a game engine built for designing and creating 2D games.
**It was created as a learning experiment** for me to gain understanding about the many aspects of game and game engine development. It is still early in development. It comes with a work-in-progress editor called *Enigma*.

  ### Getting Started ###
  To use Labyrinth clone the repo using git or download it as a zip. Once downloaded run scripts/Setup.bat, or scripts/SetupVS22 if you have Visual Studio 2022 to generate visual studio files on Windows. 
  From there build and run and the Enigma editor should launch.

## What I've Learned ##
  
  ### Project Management ###
  
  During the course of developing this project, I have gained experience with version control software by using Git and GitHub to manage my code. 
  Originally used as a means for me to switch between different computers more easily, I have expanded it's use to include using issues, pull requests, and branches to 
  effectively manage my workflow and to keep track of planned features and fixes using GitHub projects.
  
  I have also learnt how to use Premake as a build management tool, allowing me to more easily manage my projects across different platforms, rather than being 
  restricted to Visual Studio. Implementing this early has made my attempts to add Linux support much less painful. I intend to add support for CMake to further
  extend my knowledge in this area.
  
  On a simpler level, I have also gained much more understanding about how to plan and structure the project itself. For example, the earliest iterations of this engine were
  more like a game than a game engine in structure. Since then, I have learnt how to properly separate different elements of the engine, as well future proofing 
  interfaces by making them purely virtual and adding platform specific implementations, which will allow me to more easily add implementations for different platforms down the line.
  
  ### Graphics ###
  
  By far the area in which I have learnt the most is how computer graphics work. I have created a simple 2D renderer using OpenGL, and in doing so have gained experience 
  of each stage in the graphics pipeline. I have learnt about Vertex and Index buffers, Vertex arrays, how to write Vertex and Pixel shaders in GLSL, how to create and
  use textures, and how to create and use framebuffers.
  
  In Labyrinth, I have created a generic rendering API using purely virtual base classes, and then created OpenGL specific implementations for each aspect of the
  graphics pipeline. This means that the user is able to use an API that doesn't require them to see or understand an OpenGL code, and also allows me to reasonably
  easily add support for different graphics APIs later on, such as DirectX 11/12, Vulkan, or Metal. As well as this, it means that once multiple graphics APIs are 
  supported, we should be easily able to switch between them at runtime.
  
  ### Game Design ###
  
  During development, I have picked up several useful things of note when it comes to game design. For instance, I have learned about the Entity-Component-System 
  design pattern and the reasons it can be powerful during game development, from performance, such as minimising CPU cache misses, to design freedom, by removing the
  complexity and dependencies created when using a purely OOP approach to designing games.
  
  ### Linux Development ###
  
  Labyrinth is supported on both Windows and Linux (Ubuntu tested only), and this has given me lots of experience with many facets of Linux development.
  This includes understanding how GCC differs from the Visual C++ compiler, and how to effectively structure my code to handle the differences.
  I have also gained a vast amount of experience with general linux and command line usage, such as familiarity with common bash commands as well as writing scripts to improve productivity.
  
  ### Networking ###
  
  Labyrinth comes with a fairly simple networking layer API, which allows users to create server and client applications and easily customise communication.
  To define new messages, create an enum in a new namespace (e.g. NetMessages) with no name that has the type `MessageType`. 
  Write to the `Message` object by using the `<<` operator, however the input data must satisfy `std::is_standard_layout<DataType>::value` where `DataType` is your input type.
  Then use `ClientLayer::Send(const Message& msg)` or `ServerLayer::Send(const Message& msg)` to send messages to a connected client/server.
  
  ### Data Formatting ###
  
  Labyrinth uses/used various data formatting and markup languages for different functionalities. 
  Previously Tiled XML tilemap data was imported, and I wrote a simple API to convert layered tilemaps and collision data to SDL objects. See https://github.com/amayesingnathan/TMXtoSDL.
  Labyrinth Scene files are also serialised/deserialed using YAML, allowing users to save and load scenes from their local system. 
  
  ### C++ ###
  
  This project has greatly increased my proficiency in using modern C++. I am comfortable with many of newer features of C++ such as lambdas,
  smart pointers, move semantics, as well as effective metaprogramming.
  