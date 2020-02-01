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
cmake -S . -B build
cd build
make
./engineRun
```

## How to use the source code
The source code is split into 3 main modules.
- *math* - standalone module for geometric primitives, vector functions, and other geometric functions
- *visualizer* - standalone module to visualize renders using the GTK+ toolkit
- *renderer* - module responsible for tracking the scene state and performing raytracing. Depends on the *math* module.


## TODO + Bug/Modification tracker:
- [x] Camera orientations
- [x] Camera fov
- [x] Support multiple light sources
- [x] Spot lights
- [x] Directional lights
- [x] Get rid of excessive dynamic allocation
- [x] Reflections going the wrong way (see cube render)
- [x] Fix triangle points have to be clockwise
- [ ] Create a 3d file reader module
- [x] Data structures for scene representation
- [ ] Clean source code from debugging code
- [x] Fix negative lighting bug
- [x] Triangle orientation - agnostic code
- [x] Do not convert float to char and back for reflections
- [ ] Anti aliasing
- [x] Shadows
- [ ] Test with different view points
- [ ] Test with different lights in different locations
- [ ] Check the performance of Windows devices with doubles
- [x] Try parametric ray-plane intersects
- [ ] Investigate potential bug on with shadows dependent on camera position

## Render times benchmarks:
- MacBookPro 2019 2.30 GHZ i5 : 83ms
- Dell XPS 15 2.20 GHZ i7: 1520ms ***not updated***
