# C Render Engine
---

### Minimalistic render engine in C without using any libraries (almost)

## How to run
### Prerequisites:
- GTK+ needs to be installed for visualization. Without it, the visualization module will not work.
*(GTK+ is not required for proper function of the renderer module)*
### Windows
Just open the Visual Studio project `RenderingEngine.sln`

### Linux/Cross-platform
```bash
git clone https://github.com/valrotan/RenderEngine.git
cd RenderEngine/RenderEngine
mkdir build
cd build
cmake ..
make
./engineRun
```

## How to use the source code
The source code is split into 3 main modules.
- *math* - standalone module for geometric primitives, vector functions, and other geometric functions
- *visualizer* - standalone module to visualize renders using the GTK+ toolkit
- *renderer* - module responsible for tracking the scene state and performing raytracing. Depends on the *math* module.
- *fileReader* - module to read in the .obj file with triangle verticies.

## TODO + Bug/Modification tracker:
- [x] Camera orientations
- [x] Camera fov
- [x] Support multiple light sources
- [x] Spot lights
- [x] Directional lights
- [x] Get rid of excessive dynamic allocation
- [x] Reflections going the wrong way (see cube render)
- [x] Fix triangle points have to be clockwise
- [x] Create a 3d file reader module
- [ ] Add support for colors in the reader module
- [ ] Add the support for any amount of points in faces in the reader
- [x] Data structures for scene representation
- [ ] Clean source code from debugging code
- [x] Fix negative lighting bug
- [x] Triangle orientation - agnostic code
- [x] Do not convert float to char and back for reflections
- [x] Shadows
- [ ] Test with different view points
- [ ] Test with different lights in different locations
- [x] Check the performance of Windows devices with doubles
- [x] Try parametric ray-plane intersects
- [ ] Investigate potential bug on with shadows dependent on camera position

## Render times benchmarks:
- MacBookPro 2019 2.30 GHZ i5 : 52ms
- Dell XPS 15 2.20 GHZ i7: 426 ms
