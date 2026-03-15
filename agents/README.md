# agents/

Agent-focused playbooks for this ImGuiX submodule.

Files:

- `imguix-smoke-build.md` - Known-good commands to configure and rebuild smoke examples with MinGW.
- `imguix-fonts-i18n-playbook.md` - Quick operational checklist for documenting and validating fonts + i18n behavior.

When to use `imguix-fonts-i18n-playbook.md`:

- You update `README*`, `docs/FONTS-GUIDE*`, `docs/I18N-GUIDE*`, or agent docs touching fonts/i18n behavior.
- You need to verify docs against current APIs (`FontManager`, `LangStore`, `WindowInterface`, `Controller`).
- You run a docs-only QA pass and need fast anti-pattern checks before commit.

Expected outputs:

- API-aligned snippets for font role switching and i18n language flow.
- No stale signatures/roles in docs.
- EN/RU guides consistent in behavior and terminology.

Toolchain policy:

- Use CMake generator `MinGW Makefiles`.
- Use MinGW compilers `gcc` and `g++`.
- Do not use `Ninja`.
- Do not use `NMake` / `nmake`.
