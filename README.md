# ViscousMotion_Lab_5

**Laboratory work — Variant 13: Motion of a body in a viscous medium**

A C++17 / SFML 3 visualization of a body moving horizontally under linear viscous resistance. Built with CMake and vcpkg.

## Physics

Newton's second law and linear drag:

- `F_drag = -k * v`
- `m * dv/dt = -k * v`
- `v(t) = v0 * exp(-k*t/m)`
- `x(t) = v0*m/k * (1 - exp(-k*t/m))` for `k > 0`; otherwise `x(t) = v0*t`.

The simulation uses a fixed timestep `dt = 1/120 s` and an exact exponential update for velocity and displacement within each timestep. Increasing `k` causes faster speed decay; increasing `m` slows the decay. The model assumes constant mass, constant drag coefficient, and no additional external forces.

## Controls

| Key | Action |
| --- | --- |
| Space | Run / pause |
| R | Reset |
| Up / Down | Increase / decrease resistance `k` by 0.05 |
| Right / Left | Increase / decrease mass `m` by 0.25 |
| A / Z | Increase / decrease initial speed by 1 |
| Esc | Exit |

Changing a parameter resets the simulation and pauses it. The moving circle wraps visually when it reaches the edge of the track; the physical displacement continues accumulating without wrapping.

## Dashboard

The on-screen numbered fields are:

1. Mass `m` (kg)
2. Resistance `k` (kg/s)
3. Initial speed `v0` (m/s)
4. Elapsed time `t` (s)
5. Current speed `v` (m/s)
6. Acceleration `a` (m/s²)
7. Distance `x` (m)

The upper-right indicator is `1` while running, `0` while paused. The lower panel shows the speed-versus-time graph (last 10 seconds when simulation time exceeds 10 seconds). No external font is required: values use a built-in bitmap numeric font.

## Build (Windows)

Requirements: Visual Studio with MSVC, CMake, vcpkg, SFML 3 (x64-windows). Example installation:

```bat
C:\vcpkg\vcpkg.exe install "sfml[core,graphics,window]:x64-windows"
```

Configure and build:

```bat
cmake -S . -B out/build/x64-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build out/build/x64-debug
```

In Visual Studio, use the x64 Debug CMake preset and build/run `ViscousMotion_Lab_5.exe`. On systems with application-control policies, unsigned locally built executables or DLLs may be blocked; this is an environment restriction, not evidence of successful runtime testing.

## Numerical experiment (analytical reference)

For `m = 1 kg`, `v0 = 10 m/s`:

| t (s) | k=0.2 kg/s | k=0.5 kg/s | k=1.0 kg/s |
|---:|---:|---:|---:|
|0|10.00|10.00|10.00|
|1|8.19|6.07|3.68|
|2|6.70|3.68|1.35|
|3|5.49|2.23|0.50|
|5|3.68|0.82|0.07|

These values are calculated from the analytical formula, not recorded from a completed graphical run. **Conclusion:** higher resistance results in a faster exponential decline in speed.

## Project layout

```text
ViscousMotion_Lab_5/
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── .gitignore
└── ViscousMotion_Lab_5/
    └── ViscousMotion_Lab_5.cpp
```
