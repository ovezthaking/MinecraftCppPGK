# MinecraftCppPGK
Minecraft project in cpp

![image](https://github.com/user-attachments/assets/868cd3ad-cf97-4175-b192-4bdb853620e5)

<b> Overview </b> <br>
This project is a simple Minecraft-like game built using C++, SFML, and OpenGL. It features a procedurally generated voxel world, dynamic chunk management, and basic player interactions such as block placement and removal.
<br><br>
<b> Features </b>
- <b>Procedural World Generation:</b> Uses Perlin Noise to generate realistic terrain.
- <b>Chunk-Based Rendering:</b> Dynamically loads and unloads chunks based on player position to optimize performance.
- <b>Block Interaction:</b> Players can add and remove blocks using raycasting.
- <b>Crosshair Rendering:</b> A simple crosshair is displayed using OpenGL shaders.
- <b>Smooth Camera Controls:</b> First-person camera movement with mouse look.
<br><br>
<b> Technologies Used </b> <br>
- <b>C++:</b> Core programming language.
- <b>SFML:</b> Used for window management and handling user input.
- <b>OpenGL & GLAD:</b> Handles 3D rendering.
- <b>GLM:</b> Used for vector and matrix math operations.
- <b>Perlin Noise:</b> Generates procedural terrain.
<br><br>
<b>How to Build & Run</b><br><br>
<b>Prerequisites</b><br>
- C++17 or later
- GLAD
- OpenGL 3.3+
- CMake (optional, for easier compilation)
  <br><br>
  <b>Building</b> <br>
  1. Clone the repository
  2. Compile the project using your preferred method (e.g., CMake or manual compilation). (or just use Visual Studio 2022)
  3. Run the executable.
  <br><br>
  
  <b>Controls</b><br>
- <b>WASD</b> - Move forward, backward, left, and right.
- <b>Mouse Movement</b> - Look around.
- <b>Left Click</b> - Remove a block.
- <b>Right Click</b> - Place a block.
- <b>Space</b> - Move up.
- <b>Shift</b> - Move down.
- <b>Escape</b> - Exit the game.
<br><br>
<b>Future Improvements</b><br>
- Advanced world generation (biomes, caves, trees, etc.)
- Better chunk meshing and optimization
- Texture mapping for different block types
- Multiplayer support
<br><br>
<b>License</b><br>
This project is open-source and available under the MIT License.
