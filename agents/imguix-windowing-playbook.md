# ImGuiX Windowing Playbook (Agent)

Use this playbook when tasks touch window behavior, framed layout, or backend-specific window rendering.

## When to use

Use this file if the task changes or documents any of:

- `include/imguix/core/window/*`
- `include/imguix/windows/*`
- `examples/smoke/corner_icon_area*`
- `README*`, `docs/*`, or `AGENTS.md` sections about windowing

## Source of truth paths

- Core window API:
  - `include/imguix/core/window/WindowInterface.hpp`
  - `include/imguix/core/window/WindowInstance.hpp`
  - `include/imguix/core/window/WindowInstance.ipp`
  - `include/imguix/core/window/WindowManager.hpp`
  - `include/imguix/core/window/WindowManager.ipp`
- Framed window API:
  - `include/imguix/windows/ImGuiFramedWindow.hpp`
  - `include/imguix/windows/ImGuiFramedWindow.ipp`
  - `include/imguix/windows/window_flags.hpp`
  - `include/imguix/windows/corner_layout_options.hpp`
- Backend framed implementations:
  - `include/imguix/windows/SfmlImGuiFramedWindow.ipp`
  - `include/imguix/windows/GlfwImGuiFramedWindow.ipp`
  - `include/imguix/windows/Sdl2ImGuiFramedWindow.ipp`

## Mandatory checks before editing

1. Confirm layout mode and flag semantics:
- `HasCornerIconArea` changes classic vs corner layout.
- Control button style flags are mutually exclusive.

2. Confirm config auto semantics:
- `side_panel_width <= 0` differs in classic and corner mode.
- `< 0` inset/size fields use auto behavior.

3. Confirm backend behavior:
- SFML/Win32 has native rounded region path (`SetWindowRgn`).
- GLFW/SDL framed paths do not implement the same native rounded-region behavior.

4. Keep docs aligned in both languages:
- English is canonical.
- Update RU mirror when onboarding/operational behavior changes.

## Minimal regression checklist

Build at least representative smoke targets after windowing changes:

```powershell
cmake --build external/ImGuiX/build-mingw --target \
  corner_icon_area_off_demo \
  corner_icon_area_off_no_side_demo \
  corner_icon_area_demo \
  corner_icon_area_demo_v2 \
  corner_icon_area_demo_v3 \
  --parallel 8
```

Also run the target directly related to the change (for example `corner_icon_area_demo_v3_no_top_left` or `corner_icon_area_demo_v3_mac`).

## Documentation sync checklist

When changing windowing docs:

1. Update `docs/WINDOWS-GUIDE.md` first.
2. Update `docs/WINDOWS-GUIDE-RU.md` as behavior mirror.
3. Ensure `README.md` and `README-RU.md` keep only short navigation pointers.
4. Ensure `AGENTS.md` and `agents/README.md` include this playbook link.

## Anti-patterns

Do not:

1. Document backend behavior as universal when it is SFML/Win32-only.
2. Mix classic and corner semantics for `side_panel_width`.
3. Ignore style-flag conflicts for control buttons.
4. Place long implementation details in README; keep deep details in docs guide.
5. Change vendor-independent API docs without checking actual `.ipp` behavior.
