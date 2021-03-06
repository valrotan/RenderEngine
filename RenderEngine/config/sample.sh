../build/engineRun \
  -i ../input/torus.obj `#input file` \
  -o ../output.tga `#output file` \
  -a 3 `# aliasing sample grid size (3 means 3x3 grid so 9 samples)` \
  -b .15 `# ambient light` \
  -w 1280 `# width` \
  -h 720 `# height` \
  -s 1 `# scale` \
  -mx -30 `# model rotation around x axis` \
  -my 0 \
  -cx 0 `# camera rotation around x axis` \
  -cy 0 \
  -cz 0 \
  -t 1 `# recursive traces` \
  -ke .25 `# k emission (light emission from triangles)` \
  -kd .35 `# k diffuse of triangles (how affected by light)` \
  -ks .15 `# k specular for triangles (reflectivity)` \
  -br .4 `# bkg red` \
  -bg .4 `# bkg green` \
  -bb .4 `# bkg blue`
# Model: made on three.js, included with render engine already
