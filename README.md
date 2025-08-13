# Donut Engine

A modern C++ game engine with OpenGL rendering and ImGUI integration.

## Features

- **OpenGL Rendering**: Modern OpenGL 4.6 with GLAD loader
- **GLFW Window Management**: Cross-platform window and input handling
- **ImGUI Integration**: Immediate mode GUI for debugging and configuration
- **State Management**: Flexible state system for different application modes
- **Event System**: Comprehensive event handling for input and window events

## States

The engine uses a state-based architecture with the following states:

### Menu State
- Main menu interface
- Navigation to other states
- Application exit functionality

### Config State
- Simulation configuration
- Parameter adjustment (simulation speed, particle count, gravity)
- Settings reset functionality

### Simulation State
- Real-time simulation controls
- Performance monitoring (FPS, frame time)
- Debug information display
- Time scale adjustment

## Controls

- **Key 1**: Switch to Config State
- **Key 2**: Switch to Simulation State  
- **Key 3**: Switch to Main Menu
- **Mouse**: Navigate ImGUI interfaces
- **ImGUI Buttons**: State switching and configuration

## Building

The project uses Premake5 for build configuration:

```bash
# Generate Visual Studio 2022 project
premake5 vs2022

# Generate other build systems as needed
premake5 gmake2
premake5 xcode4
```

## Dependencies

- **GLFW**: Window management and input handling
- **GLAD**: OpenGL function loading
- **ImGUI**: Immediate mode GUI
- **GLM**: Mathematics library
- **Premake5**: Build system generation

## Architecture

The engine follows a modular architecture:

- **Core**: Application, Window, Event, State management
- **Platform**: Renderer API abstraction (OpenGL, Vulkan)
- **Rendering**: High-level rendering interface
- **States**: Application state implementations

## ImGUI Integration

ImGUI is fully integrated into the engine with:

- Automatic initialization after OpenGL context setup
- Per-state UI rendering through `OnImUIRender()` virtual function
- Proper frame management and cleanup
- Support for docking and viewports 