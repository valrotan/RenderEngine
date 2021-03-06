../build/engineRun \
  -i ../input/spheres.obj `#input file` \
  -o output.tga `#output file` \
  -a 2 `# aliasing sample grid size (3 means 3x3 grid so 9 samples)` \
  -b 0 `# ambient light` \
  -w 1280 `# width` \
  -h 720 `# height` \
  -s 1.5 `# scale` \
  -t 20 `# recursive traces` \
  -ke 0 `# k emission (light emission from triangles)` \
  -kd .4 `# k diffuse of triangles (how affected by light)` \
  -ks .7 `# k specular for triangles (reflectivity)` \
  -br .4 `# bkg red` \
  -bg .4 `# bkg green` \
  -bb .4 `# bkg blue`
# Model: made on three.js
