# Labyrinth #
Labyrinth is a game engine built for building 2D games.
Originally created as a learning experiment following along to The Cherno's Game Engine Series on YouTube in late 2021, it has since
diverged quite significantly to become a fun personal project which has become a platform for me to learn new technology,
libraries, and coding techniques, and much more.

Labyrinth is early in development but supports many core engine features including an event system, rendering, scene management, 
scripting, project management, asset management, physics, and networking, as well as many other minor features. It also 
comes with an editor, Enigma, that supports these features. There are still several core features still to add however, such as sound and 
animation, as well as a runtime in order to export projects as full standalone games.

***

  ### Prerequisites ###
  
    1. Python3 - This project uses python scripts in order to setup the project and download dependencies.
    2. premake5 - Used for build system - binary will be downloaded by the initial setup script.
    3. OpenGL - Labyrinth requires OpenGL 4.0 support as a minimum.
    4. Vulkan SDK - Vulkan is used for SPIR-V shader compilation - will be installed by the initial setup script
    5. C++20 Compiler - This project is written using C++20, and makes use of new language features such as concepts. Other modern features from C++14 and C++17 are also heavily used.

  ### Getting Started ###
  An early aim of this project was to provide support both Windows and Linux development, however most testing is done on Windows.

  <ins>**1. Downloading the repository:**</ins>


  Start by cloning the repository with `git clone --recursive https://github.com/amayesingnathan/LabyrinthEngine.git`.


  If the repository was cloned non-recursively previously, use `git submodule update --init --recursive` to clone the necessary submodules.


  <ins>**2. Configuring the dependencies:**</ins>


  1. Run the setup script file found in the `scripts` folder for your target platform. This will download the required prerequisites for the project if they are not present yet.
  2. One prerequisite is the Vulkan SDK. If it is not installed, the script will execute the `VulkanSDK.exe` file, and will prompt the user to install the SDK.
  3. After installation, run the setup script again. If the Vulkan SDK is installed properly, it will then download the Vulkan SDK Debug libraries. (This may take a longer amount of time)
  4. After downloading and unzipping the files, a project generation script will get executed automatically, which will then generate Visual Studio or Make files depending on your platform.


  If changes are made, or if you want to regenerate project files, rerun the script generation file for your platform found in the `scripts/gen-projects` folder.
    
***

## Features ##
  
  ### Rendering ###
  
  Labyrinth uses an OpenGL batch renderer, and provides a straightforward API for submitting work to the renderer.
  User defined texture types can easily be submitted to the renderer by inheriting from the `IRenderable` interface and providing implementations for the relevant functions. See one of the Texture2D, SubTexture2D, or Tilemap classes for examples. There are render functions for drawing circles, lines, rectangles, and rotated and/or textured quads, 
  
  ### Events ###
  
  The event system originally used a blocking polymorphic event architecture that dispatched all events to all layers of the core application stack. This was rewritten in favour of a non-blocking subscription-based architecture that makes use of `std::variant`. I preferred the use of `std::variant` as there is a closed system of known events, and it provides a more cache-friendly approach to handling events.
  
  Users can easily add listeners to the system by inheriting from the `IEventListener` class and must implement `void onEvent(Event& e)`. Layers in the application automatically inherit from IEventListener but do not implement onEvent. Any class that inherits from IEventListener (including Layers) must specify the individual events to listen for using the `LISTENING_EVENTS(...)` macro, which takes in a variable number of values from the `EventType::Flag` enum, separated by commas, e.g. `LISTENING_EVENTS(WindowResize, MouseMoved)`.
  
  A local event dispatcher is also included to allow users to simplify the `onEvent` function of any listener. This takes a reference to an event as its constructor. Specific event types can easily be dispatched to other functions by doing `dispatcher.dispatch<KeyPressedEvent>(LAB_BIND_EVENT_FUNC(FooBar))` where FooBar is a member function that takes a `KeyPresedEvent&` as a parameter.
  
  ### Scene Management ###
  
  Labyrinth uses the Entity-Component-System (ECS) architecture to manage scenes. This a data-oriented approach in which entities are represented by simple IDs, and are given behaviour and properties by attaching components to an ID. Components are then stored contiguously in memory, and loop over each component type at a time in the main game loop. This produces a very cache-friendly solution to handling game logic. 
  
  I have used the EnTT library to handle the ECS as it is a proven and reliable solution, however Labyrinth wraps this library by providing an Entity class which is a wrapper over the Entity ID that allows a cleaner interface for managing components on an Entity. The Scene class contains the EnTT context and provides helper functions to expose the ECS where necessary. Labyrinth uses YAML to create human readable serialised scenes which allow for saving and loading of different scenes.
  
  Working with EnTT inspired me to create my own sparse set implementation of an ECS, [found here](github.com/amayesingnathan/LEntity).
  
  ### Projects and Assets ###
  
  Labyrinth comes with a project system that allows users to maintain their work in a structured format. Part of the project structure includes an asset registry and directory, which contains the assets used within that project. The asset manager automatically detects any new assets moved to the asset directory and saves them to the registry if they don't exist when a project is unloaded. New assets can easily be created and will be automatically added to the registry.
  
  When an asset is requested, the asset manager will check if it has already been loaded, and if not it will import the asset from the registry. One an asset has been requested, it will be kept loaded by the asset manager in anticipation of reuse. Currently all assets persist until the project is unloaded, but I intend to add an asset handler that unloads assets that have been unused for a set period of time.
  
  ### Scripting ###
  
  Scripting is a vital part of a game engine, as it allows users to add complex behaviour to their game. Choice of language is important, as a balance must be struck between performance and ease of use. I chose C# to use as a scripting language, as whilst it is not as fast as a language such as lua, it is much more fully featured, and so is much easier to create more complex behaviour with.
  
  Labyrinth ships with a core assembly that provides C# wrappers around core engine parts, such as the ECS, Assets, and Physics. Users are then able to create their own assembly per project which can then be used in the editor to assign behaviour to different entities. Projects handle generation of project files (e.g. VS .sln) for the user assembly, and Labyrinth will refresh the assembly when any changes occur.
  
  I have implemented this using Mono, which is an open source implementation of the Common Language Runtime, suitable for embedding within a C/C++ application.
  
  ### Physics ###
  
  Labyrinth simulates physics using Box2D. The key elements of simulation using Box2D, such as bodies and shapes, have analogous ECS components which cause these elements to be created in the Box2D world. Labyrinth supports static, kinematic, and dynamic rigid bodies, and can be affixed with box, circle, or chain shapes in order to simulate collisions. Rigid bodies that also contain scripts will generate collision events in the managed runtime to integrate physics and behaviour.
  
  ### Networking ###
  
  Labyrinth comes with a fairly simple but easily extensible networking API that uses asio, which allows users to create server and client applications and customise communication. To add networking to an application, use `SetClient(ClientLayer*)` or`SetServer(ServerLayer*)` in the constructor of a Labyrinth application. 
  
  SetClient does not necessarily need a parameter, because ClientLayer fully implements all pure virtual inherited functions, however clients will only connect locally unless `void onAttach()` is overriden. Users must subclass ServerLayer and implement `void send(const Message&)` to define communication logic. ClientLayer will by default send all messages directly to the server, however users can implement their own overrides using a number of helper functions.
  
  To define new message types, create an anonymous enum in a new namespace (e.g. NetMessages) with the underlying `MessageType` type. Write data to the `Message` object by using the `<<` operator, however the input data must satisfy `std::is_standard_layout<DataType>::value` where `DataType` is your input type. Then use `Application::SendNetMessage(const Message&)` to send messages to a connected client/server.
  
  ### Tilemaps ###
  
  As Labyrinth and Enigma are designed for use creating 2D games, another significant engine feature is tilemaps. Users can attach spritesheets to a tilemap in order to use textures from that sheet to create the map's texture. Enigma provides a tilemap editing tool which allows for easy painting with the attached spritesheets. 
  
  Tilemaps also provide a way to add behaviour and physics to the map. By setting the status of different tiles in the map to solid, the tilemap will generate physics bodies for any contiguous solid shapes at runtime. Scripts can also be set for individual tiles to create trigger points or other behaviour.