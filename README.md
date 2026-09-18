# RayTracing

A real-time CPU ray tracer written from scratch in C++17, rendering into an
interactive Dear ImGui viewport.

Every pixel is traced on the CPU each frame: a ray is constructed through the
pixel, intersected analytically against scene geometry, and the resulting colour
is written into a buffer that is uploaded to the GPU as a texture for display.
The UI reports the time taken by the last frame so the cost of a change is
visible immediately.

## Status

Work in progress. The intersection core works: the renderer solves the
ray-sphere equation per pixel in real time and resizes with the viewport.
Shading is the next step, so the sphere currently renders as a flat silhouette
rather than a lit surface. See the roadmap below.

## How it works

A ray is defined as `P(t) = a + b*t`, where `a` is the ray origin and `b` its
direction. Substituting that into the equation of a sphere of radius `r`
centred at the origin, `x^2 + y^2 + z^2 = r^2`, and collecting terms in `t`
gives a quadratic:

```
(b . b) t^2 + 2 (a . b) t + (a . a - r^2) = 0
```

The discriminant `B^2 - 4AC` is then enough to answer the only question that
matters per pixel: a negative value means the ray misses the sphere, and a
non-negative value means it hits. Solving for the smaller root of `t` yields
the nearest hit distance, which is what the shading work below builds on.

Pixel coordinates are first normalised to `[0, 1]`, then remapped to `[-1, 1]`
so the viewport is centred on the origin regardless of its size.

## Building

Requires Windows, Visual Studio 2022, and the
[Vulkan SDK](https://vulkan.lunarg.com/) (used by the Walnut framework for
rendering the UI).

```bash
git clone --recursive https://github.com/nRaqu1n/RayTracing
cd RayTracing
```

If you cloned without `--recursive`, fetch the framework with
`git submodule update --init --recursive`.

Then run `scripts/Setup.bat` to generate the Visual Studio solution with
premake, open `RayTracing.sln`, and build. The solution and project files are
generated rather than committed, so they are not in this repository.

## Project structure

```
RayTracing/          the ray tracer
  src/WalnutApp.cpp  application layer: UI, viewport, frame timing
  Renderer.h/.cpp    the renderer: ray construction and per-pixel tracing
  premake5.lua       project build configuration
experiments/         early exploratory code, not part of the build
Walnut/              application framework (git submodule)
scripts/Setup.bat    generates the Visual Studio solution
premake5.lua         workspace build configuration
```

## Roadmap

- [ ] Shade hits from the surface normal instead of a flat colour
- [ ] Directional light with Lambertian diffuse shading
- [ ] Move the hardcoded sphere into a scene description with multiple objects
- [ ] Movable camera with per-pixel ray directions cached on resize
- [ ] Materials, reflections and multi-bounce accumulation
- [ ] Multithread the pixel loop

## Credits

Built on [Walnut](https://github.com/TheCherno/Walnut) by Studio Cherno (MIT), a
Vulkan and Dear ImGui application framework, included here as a submodule. The
ray tracing implementation in this repository is my own, written while working
through the underlying mathematics.

## License

MIT - see [LICENSE](LICENSE).
