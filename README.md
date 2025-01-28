# PONG

Remake of the 80s classic game PONG using OpenGL and C++.

Local multiplayer supported.

[Gameplay video](https://youtu.be/SkBrM-LPmRU)

# Table of contents
- [Introduction](#introduction)
- [Dependencies](#dependencies)
- [Controls](#controls)
- [pkEngine](#pkengine)
- [Future Improvements](#future-improvements)

# Introduction

This project is a modern remake of the iconic 80s game **PONG**, developed as a hands-on learning experience to explore **OpenGL** techniques and fundamental game development concepts. Through this project, I gained practical knowledge in:


- **Rendering**: Drawing 2D graphics using OpenGL;
- **Shaders**: Writing and managing GLSL shaders for rendering effects;
- **Collisions handling**: Implementing collision detection and response;
- **Engine design**: Building a lightweight 2D game engine from scratch

## Dependencies

- [GLFW](https://www.glfw.org/) - Handles window creation and OpenGL context management
- [GLM](https://github.com/g-truc/glm) - Provides vector and matrix operations for 3D math
- [STB](https://github.com/nothings/stb) - Used for loading and decoding image files
- [FreeType](https://freetype.org/) - Enables rendering of TrueType fonts
- [FMOD](https://www.fmod.com/) - For sound effects and music

# Controls

- **Space**: Start a new match
- **W-S**: Control left paddle
- **KeyUp/KeyDown**: Control right paddle
- **Esc**: Close game

# pkEngine

pkEngine is a lightweight **2D game engine** developed during the creation of this project. 
While still in its early stages, it provides a robust framework for 2D game development, offering essential tools and utilities.

## Features

### Rendering

pkEngine includes classes for rendering 2D sprites and text efficiently:

- **Shader**: Manages **vertex** and **fragment** shaders, compiling and linking them into a shader program;
- **Texture**: Loads **image** files and creates OpenGL textures for rendering;
- **Font**: Loads TrueType fonts (TTF) and **renders text** as textures;
- **Renderer**: A manager class responsible for rendering sprites and text on the screen;

### Miscellaneous

pkEngine provides utility classes to simplify common tasks:

- **Window**: Handles the lifecycle of a **GLFW window** and provides utility functions for window handling;
- **AssetManager**: Loads and stores game **assets** (textures, fonts, sounds) to **avoid redundant** loading;
- **SoundEngine**: Integrates **FMOD** for audio playback, supporting WAV and other formats. Includes **jukebox** functions for volume and pitch control; 

### Game

pkEngine includes classes to handle gameplay and visual effects:

- **GameActor**: The base class for all **game objects**. It represents a 2D movable sprite with a transform, texture, and collision detection using **AABB**;
- **Emitter**: Renders textures as **particles**, supporting various **patterns** for dynamic effects;
- **EmitterPattern**: Define the behaviour of a particle
	* **LinearPattern**: Particles move in a linear direction, opposite to the attached actor;
	* **BouncePattern**: Simulates an explosion by spawning particles in multiple directions based on collision;

# Future improvements

- **Advanced Shaders**: Implement more complex shaders for post-processing effects;
- **UI Enhancements**: Improve the user interface with a proper main menu and settings;
- **Improve engine**: Extend the engine to support more type of collisions detection, batch rendering and so on;