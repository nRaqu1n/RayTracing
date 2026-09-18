# RayTracing

A real-time CPU ray tracer written from scratch in C++17, rendering into an
interactive Dear ImGui viewport.

Every pixel is traced on the CPU each frame: a ray is constructed through the
pixel, intersected analytically against scene geometry, and the resulting colour
is written into a buffer that is uploaded to the GPU as a texture for display.
The UI reports the time taken by the last frame so the cost of a change is
visible immediately.

## Status

The renderer traces a scene of several spheres, shades them from their
surface normal under a directional light, and multithreads the pixel loop.
See the roadmap below for what's still ahead (movable camera, materials,
reflections).

## How it works

A ray is defined as `P(t) = a + b*t`, where `a` is the ray origin and `b` its
direction. Substituting that into the equation of a sphere of radius `r`
centred at the sphere's position, and collecting terms in `t`, gives a
quadratic:

```
(b . b) t^2 + 2 (a . b) t + (a . a - r^2) = 0
```

The discriminant `B^2 - 4AC` answers whether the ray hits: negative means a
miss, non-negative means a hit. Solving for the smaller root of `t` gives the
nearest hit distance along the ray.

Each ray is tested against every sphere in the scene, keeping only the
closest positive hit. At that hit point, the surface normal is just the
normalised vector from the sphere's centre to the hit point
(`normalize(hitPoint - center)`). A single directional light shades the
surface with a Lambertian diffuse term, `max(0, dot(normal, -lightDir))`,
multiplied by the sphere's albedo — this is what turns the flat silhouette
into something that reads as a lit, 3D object.

Pixel coordinates are first normalised to `[0, 1]`, then remapped to `[-1, 1]`
so the viewport is centred on the origin regardless of its size. Rows of
pixels are handed to `std::for_each(std::execution::par, ...)` so the scene
is traced across all available cores instead of one.

## Performance

Multithreading the pixel loop (`std::for_each` with `std::execution::par`
over image rows, instead of a plain nested loop) measured on this machine,
tracing the 3-sphere scene at 1280x720, averaged over 10 runs of the renderer
core in a Release build:

| | Sequential | `std::execution::par` |
|---|---|---|
| Time per frame | 30.5 ms | 2.0 ms |

That's about a 15x speedup from a one-line change to the pixel loop, quoted
here from an isolated benchmark of the renderer core rather than a screen
capture of the running app. Run it yourself and swap in your own numbers:
the "Last Render" timer in the Settings panel reads directly off
`Timer::ElapsedMillis()` around the same `Render()` call.

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

- [x] Shade hits from the surface normal instead of a flat colour
- [x] Directional light with Lambertian diffuse shading
- [x] Move the hardcoded sphere into a scene description with multiple objects
- [x] Multithread the pixel loop
- [ ] Movable camera with per-pixel ray directions cached on resize
- [ ] Materials, reflections and multi-bounce accumulation

## Credits

Built on [Walnut](https://github.com/TheCherno/Walnut) by Studio Cherno (MIT), a
Vulkan and Dear ImGui application framework, included here as a submodule. The
ray tracing implementation in this repository is my own, written while working
through the underlying mathematics.

## License

MIT - see [LICENSE](LICENSE).
