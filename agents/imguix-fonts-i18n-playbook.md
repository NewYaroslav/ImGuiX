# ImGuiX Fonts + i18n Playbook (Agent)

Use this playbook when you need to review or update docs around font loading, role switching, and localization.

## 1) Source of truth (read first)

- Fonts API:
  - `include/imguix/core/fonts/font_types.hpp`
  - `include/imguix/core/fonts/FontManager.hpp`
  - `include/imguix/core/fonts/FontManager.ipp`
- i18n API:
  - `include/imguix/core/i18n/LangStore.hpp`
  - `include/imguix/core/i18n/LangStore.ipp`
  - `include/imguix/core/i18n/PluralRules.hpp`
  - `include/imguix/core/i18n/PluralRules.ipp`
- Window/controller integration:
  - `include/imguix/core/window/WindowInterface.hpp`
  - `include/imguix/core/window/WindowInstance.hpp`
  - `include/imguix/core/window/WindowInstance.ipp`
  - `include/imguix/core/window/WindowManager.ipp`
  - `include/imguix/core/controller/Controller.hpp`

## 2) Required examples to cross-check

- `examples/smoke/font_switch_demo.cpp`
- `examples/smoke/i18n_demo.cpp`
- `examples/smoke/widgets_demo.cpp`

## 3) Font docs must state explicitly

- Init-only methods: `fontsBeginManual`, `fontsAdd*`, `fontsBuildNow`, ranges setters.
- Runtime-safe methods: `fontsControl().set*`, `rebuildIfNeeded`, `getFont`.
- Controller usage pattern: `getFont(FontRole::H1)` + `ImGui::PushFont/PopFont` with `nullptr` fallback.
- `Icons`/`Emoji` merge into body chain.
- Ranges semantics: preset vs explicit, last call wins.

## 4) i18n docs must state explicitly

- `LangStore` API: `text`, `label`, `doc`, `textf_key`, `text_plural`, `textf_plural`.
- Fallback chains for strings and plurals.
- Resource layout and file naming (`<base>/<lang>/*.json`, `<base>/<lang>/<doc_key>.md`, `plurals.json`).
- JSON merge caveat: keys should be unique across files in one language folder.
- Plural rules authoring basics for `plurals.json`:
  - `cardinal` exists per language.
  - each rule has `cat`.
  - final catch-all `other` rule is present.
  - emitted categories match `base_key.<cat>` keys in language JSON.
- `LangChangeEvent` -> `WindowManager` -> `WindowInstance::applyPendingLanguageChange()` flow.
- i18n + fonts coupling: language switch does not guarantee glyph coverage without proper font locale/ranges.

## 5) Mid-level sanity checklist

- Call-context boundaries are explicit:
  - init-only (`fontsBeginManual/fontsAdd*/fontsBuildNow`) vs runtime (`fontsControl().set*`, `rebuildIfNeeded`).
- Controller snippets use `getFont(FontRole::...)` with `ImGui::PushFont/PopFont` and `nullptr` fallback.
- i18n layout explains `<base>/<lang>/*.json` + `<doc_key>.md` + `plurals.json`.
- i18n docs mention duplicate-key risk across JSON files and warn against relying on file order.
- i18n docs explain `plurals.json` rule order and first-match behavior.
- i18n docs show how plural categories map to localized keys (`Items.one/few/many/other`).
- Language switch flow includes `onBeforeLanguageApply(lang) -> fontsControl().setLocale(lang)`.

Common anti-pattern:

- `plurals.json` updated, but localized keys for emitted categories were not added (`Items.<cat>` missing).

## 6) Anti-pattern grep checks

Run before commit:

```powershell
rg -n "FontRole::Symbols|oversample=|fontsSetLocale\(.*bool" README*.md docs
```

Expected: no matches.

## 7) Minimal doc validation checklist

- All links to guides are valid and use correct language target.
- README has both `Internationalization` and `Fonts` pointers.
- EN/RU guides are aligned in behavior and terminology.
- No source code changes in docs-only task.
