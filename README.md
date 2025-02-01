# MinecraftCppPGK
Minecraft project in cpp <br><br>

# Minecraft Clone in C++ with SFML and OpenGL

## Overview
This project is a simple Minecraft-like game built using C++, SFML, and OpenGL. It features a procedurally generated voxel world, dynamic chunk management, and basic player interactions such as block placement and removal.

## Features
- **Procedural World Generation**: Uses Perlin Noise to generate realistic terrain.
- **Chunk-Based Rendering**: Dynamically loads and unloads chunks based on player position to optimize performance.
- **Block Interaction**: Players can add and remove blocks using raycasting.
- **Crosshair Rendering**: A simple crosshair is displayed using OpenGL shaders.
- **Smooth Camera Controls**: First-person camera movement with mouse look.

## Technologies Used
- **C++**: Core programming language.
- **SFML**: Used for window management and handling user input.
- **OpenGL & GLAD**: Handles 3D rendering.
- **GLM**: Used for vector and matrix math operations.
- **Perlin Noise**: Generates procedural terrain.

## How to Build & Run
### Prerequisites
- C++17 or later
- SFML
- GLAD
- OpenGL 3.3+
- CMake (optional, for easier compilation)

### Building
1. Clone the repository:
   ```sh
   git clone https://github.com/ovezthaking/MinecraftCppPGK.git
   cd MinecraftCppPGK
   ```
2. Compile the project using your preferred method (e.g., CMake or manual compilation).
3. Run the executable.

## Controls
- **WASD** - Move forward, backward, left, and right.
- **Mouse Movement** - Look around.
- **Left Click** - Remove a block.
- **Right Click** - Place a block.
- **Space** - Move up.
- **Shift** - Move down.
- **Escape** - Exit the game.

## Future Improvements
- Advanced world generation (biomes, caves, trees, etc.)
- Better chunk meshing and optimization
- Texture mapping for different block types
- Multiplayer support

## License
This project is open-source and available under the MIT License.


![image](https://github.com/user-attachments/assets/868cd3ad-cf97-4175-b192-4bdb853620e5)
