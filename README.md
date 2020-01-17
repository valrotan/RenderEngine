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
- [ ] Camera orientations
- [ ] Camera fov
- [ ] Support multiple light sources
- [ ] Spot lights
- [ ] Directional lights
- [ ] Get rid of excessive dynamic allocation
- [x] Reflections going the wrong way (see cube render)
- [ ] Fix triangle points have to be clockwise
- [ ] Create a 3D shape to triangles module
- [ ] Data structures for scene representation
- [ ] Clean source code from debugging code
- [ ] High level API for scene creation
