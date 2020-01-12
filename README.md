# C Render Engine
---

### Minimalistic render engine in C without using any libraries (almost)

## TODO:
- [ ] Init camera at any position
- [ ] Support multiple light sources
- [ ] Spot lights
- [ ] Directional lights
- [ ] Get rid of excessive dynamic allocation

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
