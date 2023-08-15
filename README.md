# Blockgame

Yeah this is just a Minecraft clone.

Currently it just resembles the original Cave Game Tech Test,
I might add features when I feel like it.

## Controls

WASD to move around

Left click to destroy block

Right click to place brick

## Build

Dependencies:

[GLFW](https://glfw.org)

[glm](https://github.com/g-truc/glm)

[glad](https://glad.dav1d.de/) (included)

[stb_image](https://github.com/nothings/stb/blob/master/stb_image.h) (included)

```
mkdir build
cd build
cmake ..
make
ln -sf ../assets
./blockgame
```
