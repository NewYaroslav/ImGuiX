# ImGuiX Smoke Build (Agent Playbook)

Use this guide to rebuild ImGuiX smoke examples with a known-good Windows MinGW setup.

## Mandatory toolchain policy

- Use CMake generator: `MinGW Makefiles`.
- Use MinGW compilers: `gcc`, `g++`.
- Do not use `Ninja`.
- Do not use `NMake` / `nmake`.

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

```powershell
cmake --build external/ImGuiX/build-mingw --target `
  corner_icon_area_off_demo `
  corner_icon_area_off_no_side_demo `
  corner_icon_area_demo `
  corner_icon_area_demo_v2 `
  corner_icon_area_demo_v3 `
  --parallel 8
```

### Context B: run from `external/ImGuiX` root

```powershell
cmake --build build-mingw --target `
  corner_icon_area_off_demo `
  corner_icon_area_off_no_side_demo `
  corner_icon_area_demo `
  corner_icon_area_demo_v2 `
  corner_icon_area_demo_v3 `
  --parallel 8
```

## Run artifacts

- Superproject context: `external/ImGuiX/build-mingw/examples/*.exe`
- Submodule-root context: `build-mingw/examples/*.exe`

## Troubleshooting

- If CMake keeps old generator/toolchain, remove only `build-mingw` in the current context and reconfigure.
- `-DNDEBUG` is intentional in this profile to avoid assertion-related breakage in current example stack.
- Warnings about deprecated `std::wstring_convert` are known and non-blocking for smoke builds.

