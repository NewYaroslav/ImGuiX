# ImGuiX Smoke Build (Agent Playbook)

Use this guide to rebuild ImGuiX smoke examples with a known-good Windows MinGW setup.

## Mandatory toolchain policy

- Use CMake generator: `MinGW Makefiles`.
- Use MinGW compilers: `gcc`, `g++`.
- Do not use `Ninja`.
- Do not use `NMake` / `nmake`.

## Machine-local parallelism from superproject

- When running inside the `mgc-platform` superproject, first read `agents/local-machine-settings.json` from the superproject root.
- If that file is missing on Windows, create it with `scripts/detect-machine-settings.ps1` from the superproject root.
- Use `preferred_build_parallelism` for `cmake --build ... --parallel N`.
- Use `preferred_submodule_jobs` for `git submodule update ... --jobs N`.
- If the superproject-local file is unavailable, detect logical processors on the current Windows machine and fall back to `8` only if detection fails.
- On the current superproject machine, the detected value is `36`.

## Build directory reuse policy

Decision table:

- Compatible `build-mingw` exists: reuse it.
- Incompatible `build-mingw` exists: recreate that same `build-mingw`.
- Explicit ImPlot / ImPlot3D validation requested: use `build-mingw-implot`.
- No compatible canonical build tree exists: create the canonical one, not an ad-hoc directory.

Additional rules:

- Prefer reconfiguring in place before rebuilding.
- Do not create `build`, `build-sfml`, or `build-sfml-ninja` unless the user explicitly requested a separate profile.
- Do not run multiple independent `cmake --build` processes concurrently against the same build tree.

## Configure

Choose one context and run from that working directory.

### Context A: run from superproject root

```powershell
cmake -S external/ImGuiX -B external/ImGuiX/build-mingw -G "MinGW Makefiles" `
  -DCMAKE_C_COMPILER=gcc `
  -DCMAKE_CXX_COMPILER=g++ `
  -DCMAKE_SKIP_INSTALL_RULES=ON `
  -DCMAKE_CXX_FLAGS=-DNDEBUG `
  -DIMGUIX_USE_SFML_BACKEND=ON `
  -DIMGUIX_USE_GLFW_BACKEND=OFF `
  -DIMGUIX_USE_SDL2_BACKEND=OFF `
  -DIMGUIX_BUILD_EXAMPLES=ON `
  -DIMGUIX_BUILD_TESTS=OFF
```

### Context B: run from `external/ImGuiX` root

```powershell
cmake -S . -B build-mingw -G "MinGW Makefiles" `
  -DCMAKE_C_COMPILER=gcc `
  -DCMAKE_CXX_COMPILER=g++ `
  -DCMAKE_SKIP_INSTALL_RULES=ON `
  -DCMAKE_CXX_FLAGS=-DNDEBUG `
  -DIMGUIX_USE_SFML_BACKEND=ON `
  -DIMGUIX_USE_GLFW_BACKEND=OFF `
  -DIMGUIX_USE_SDL2_BACKEND=OFF `
  -DIMGUIX_BUILD_EXAMPLES=ON `
  -DIMGUIX_BUILD_TESTS=OFF
```

## Build smoke demos

### Context A: run from superproject root

Current machine example:

```powershell
cmake --build external/ImGuiX/build-mingw --target `
  corner_icon_area_off_demo `
  corner_icon_area_off_no_side_demo `
  corner_icon_area_demo `
  corner_icon_area_demo_v2 `
  corner_icon_area_demo_v3 `
  --parallel 36
```

### Context B: run from `external/ImGuiX` root

Current machine example:

```powershell
cmake --build build-mingw --target `
  corner_icon_area_off_demo `
  corner_icon_area_off_no_side_demo `
  corner_icon_area_demo `
  corner_icon_area_demo_v2 `
  corner_icon_area_demo_v3 `
  --parallel 36
```

## Run artifacts

- Superproject context: `external/ImGuiX/build-mingw/examples/*.exe`
- Submodule-root context: `build-mingw/examples/*.exe`

## Troubleshooting

- If CMake keeps old generator/toolchain, remove only `build-mingw` in the current context and reconfigure.
- `-DNDEBUG` is intentional in this profile to avoid assertion-related breakage in current example stack.
- Warnings about deprecated `std::wstring_convert` are known and non-blocking for smoke builds.
