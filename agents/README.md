# agents/

Agent-focused playbooks for this ImGuiX submodule.

Files:

- `imguix-smoke-build.md` - Known-good commands to configure and rebuild smoke examples with MinGW.

Toolchain policy:

- Use CMake generator `MinGW Makefiles`.
- Use MinGW compilers `gcc` and `g++`.
- Do not use `Ninja`.
- Do not use `NMake` / `nmake`.

