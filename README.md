# OpenGL Playground

For now I am experimenting here with OpenGL, loosely based on https://learnopengl.com/

## Cloning

```sh
git clone --recurse-submodules link-to-this-repo
```

If you already cloned and forgot `--recurse-submodules` :

```sh
git submodule update --init --recursive
```

## Building

Requirements: C++ Compiler, CMake

```sh
cmake -B build
cmake --build build
```

## Run

```sh
./build/opengl-playground
```
