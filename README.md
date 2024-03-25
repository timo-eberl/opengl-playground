# Ron the Renderer

OpenGL 3D Rendering library written in C++

## Roadmap

- [x] Basic Blinn-Phong rendering of triangle meshes with albedo, normal, metallic and roughness textures
- [x] glTF 2.0 Import
- [x] Render multiple scenes
- [x] Tangent Generation
- [x] Shader and texture hot reload
- [ ] Multiple, configurable Lights
- [ ] PBR Lighting
- [ ] Emissive materials
- [ ] Transparent materials
- [ ] Post-Processing

## Cloning

```sh
git clone --recurse-submodules link-to-this-repo
```

If you already cloned and forgot `--recurse-submodules` :

```sh
git submodule update --init --recursive
```

## Build and run the example applicaton

Requirements: C++ Compiler, CMake

```sh
cmake -B build
cmake --build build

./build/ron_example
```

## Usage

Initialization:

```CPP
auto scene = ron::gltf::import("models/default/cube/cube.gltf");
auto camera = ron::PerspectiveCamera(45.0f, 1920.0f/1080.0f, 0.1f, 1000.0f);

OpenGLRenderer renderer = {};
renderer.preload(scene);
```

Rendering Loop:

```CPP
renderer.render(scene, camera);
```

> For a complete example, see `src/example/main.cpp`
