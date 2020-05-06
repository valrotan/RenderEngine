# 3D Raytracing Render Engine in C

### Minimalistic render engine in C without using any libraries (almost)
This is a simple ray-tracing rendering engine in C. It is partially inspired by/based on [Princeton's computer graphics class from 2000](https://www.cs.princeton.edu/courses/archive/fall00/cs426/).

The rendering engine supports most obj files and does not support color from materials yet. It does, however, have full rgb support if you hardcode the triangles or write the code to parse material files.

The only two non-standard C libraries used are pthread and gtk+. gtk is used to render output to the screen and is optional. pthread is to render faster, even though it is completely on a CPU and the goal is to demonstrate simple rendering techniques and not be high-performing.

![](https://github.com/valrotan/RenderEngine/blob/master/human.png)
![](https://github.com/valrotan/RenderEngine/blob/master/biglady.jpg)

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
./engineRun [path to obj]
```
Sample runs are located in the config subdirectory. For sample.sh, you should be able to simply run it `sh sample.sh`.

## How to use the source code
The source code is split into 4 main modules.
- *math* - standalone module for geometric primitives, vector functions, and other geometric functions
- *visualizer* - standalone module to visualize renders using the GTK+ toolkit
- *renderer* - module responsible for tracking the scene state and performing raytracing. Depends on the *math* module.
- *fileReader* - module to read in the .obj file with triangle verticies.
- main.c has a lot most of the needed sample code to get started with modifying the scene and extending the project
