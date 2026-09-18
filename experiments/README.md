# Experiments

Early, exploratory code kept for reference. **None of it is part of the build.**

Before the renderer settled on solving the ray-sphere quadratic inline
(`RayTracing/Renderer.cpp`), this was a first attempt at modelling the problem
with separate `Sphere` and `Line` types and a free `draw_function`. It was
abandoned in favour of the vector-based approach, and it contains at least one
known error: `Draw.cpp` computes the discriminant as `b - 4ac` rather than
`b^2 - 4ac`.

Left in the repository as a record of the approach, not as working code.
