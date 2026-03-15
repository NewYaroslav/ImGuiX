# ImGuiX i18n Guide

This guide explains how localization works in ImGuiX with `LangStore`, `PluralRules`, and language-change events.

## Core components

- `ImGuiX::I18N::LangStore`
  - Loads localized strings from JSON.
  - Loads markdown documents (`.md`) lazily.
  - Supports formatting and plural forms.
- `ImGuiX::I18N::PluralRules`
  - Resolves plural category (`one`, `few`, `many`, `other`, ...).
  - Can load rules from JSON (`plurals.json`).
  - Has built-in fallback for `en` and `ru`.
- `ImGuiX::Events::LangChangeEvent`
  - Requests language switch globally or for one window.

## Default paths and config macros

From `imguix/config/i18n.hpp`:

- `IMGUIX_I18N_DIR` -> default `data/resources/i18n`
- `IMGUIX_I18N_JSON_BASENAME` -> default `strings.json` (name hint only; loader reads all `*.json` in lang directory)
- `IMGUIX_I18N_PLURALS_FILENAME` -> default `plurals.json`
- `IMGUIX_RESOLVE_PATHS_REL_TO_EXE` -> resolve paths relative to executable when nonzero

## Resource layout

Practical layout used by current implementation:

```text
data/resources/i18n/
  plurals.json
  en/
    ui_core.json
    ui_format.json
    Docs.GettingStarted.md
    About.App.md
  ru/
    ui_core.json
    Docs.GettingStarted.md
    About.App.md
```

Notes:

- `LangStore` loads every `*.json` file inside `<base>/<lang>/` and merges objects.
- Keep keys unique across files in the same language folder.
- Do not rely on cross-file override order when duplicate keys exist.
- Markdown docs are resolved as `<base>/<lang>/<doc_key>.md`.
- `doc("Docs.GettingStarted")` looks for `Docs.GettingStarted.md`.

## JSON formats for short strings

### Monolingual

```json
{
  "Menu.File": "File",
  "Menu.Edit": "Edit"
}
```

### Polyglot object

```json
{
  "Polyglot.Example": {
    "en": "Example",
    "ru": "Пример"
  }
}
```

### String arrays (concatenated)

```json
{
  "Banner.Text": ["Hello", " ", "World"]
}
```

`LangStore` strips JSON comments before parsing, so comments are allowed in practice.

## LangStore API in daily use

- `text(key)`
  - Returns localized short string.
- `label(key)`
  - Returns cached `"<text>##<key>"` for stable ImGui IDs.
- `doc(doc_key)`
  - Returns markdown content from language folder with fallback.
- `textf_key(key, ...)`
  - Formats localized template string.
- `text_plural(base_key, n)`
  - Resolves pluralized key by category.
- `textf_plural(base_key, n, ...)`
  - Same as above with formatting.

Example:

```cpp
ImGui::TextUnformatted(langStore().text("Menu.File").c_str());
ImGui::InputText(langStore().label("Settings.General.Username"), buf, IM_ARRAYSIZE(buf));
ImGui::Text("%s", langStore().textf_key("Build.Version", 1, 2, 3).c_str());
ImGui::Text("%s", langStore().textf_plural("Items", n, fmt::arg("n", n)).c_str());
```

## Fallback chains

### text(key)

1. Current language map.
2. Default language map (`en` by default).
3. Internal missing marker (`"##null"`).

### text_plural(base_key, n)

1. `<base_key>.<suffix>` in current language.
2. `<base_key>.<suffix>` in default language.
3. `<base_key>` in current language.
4. `<base_key>` in default language.
5. Missing marker.

## PluralRules JSON grammar

`plurals.json` uses per-language `cardinal` arrays. Supported condition keys:

- `true`
- `all` (array of sub-rules)
- `any` (array of sub-rules)
- `not` (single sub-rule)
- `eq`
- `in` (list)
- `range` ([a, b])
- `mod_eq` (`{"mod": M, "eq": E}`)
- `mod_in_list` (`{"mod": M, "list": [...]}`)
- `mod_in_range` (`{"mod": M, "range": [a, b]}`)

Example fragment:

```json
{
  "en": {
    "cardinal": [
      { "cat": "one", "eq": 1 },
      { "cat": "other", "true": true }
    ]
  }
}
```

## Plural rules authoring (`plurals.json`)

Use `data/resources/i18n/plurals.json` (or your configured
`IMGUIX_I18N_PLURALS_FILENAME`) to define per-language plural categories.

Authoring rules:

- Top-level keys are language codes (`"en"`, `"ru"`, ...).
- Each language provides a `cardinal` array.
- Each item must contain `cat` and a condition (`eq`, `in`, `range`, `mod_*`, `all/any/not`, or `true`).
- Rules are evaluated top-to-bottom.
- First matching rule wins.
- Always keep a catch-all tail rule: `{ "cat": "other", "true": true }`.

Category-to-text contract:

- `text_plural("Items", n)` resolves a category and then looks up `Items.<cat>`.
- If your rules emit `one/few/many/other`, your language JSON must provide these keys.

Failure behavior:

- If `plurals.json` is missing or invalid, loaded custom rules are skipped.
- Plural category falls back to built-in behavior (`en`, `ru`, then English-like default).
- `LangStore` fallback chain still applies for missing localized keys.

Example `plurals.json`:

```json
{
  "en": {
    "cardinal": [
      { "cat": "one", "eq": 1 },
      { "cat": "other", "true": true }
    ]
  },
  "ru": {
    "cardinal": [
      {
        "cat": "one",
        "all": [
          { "mod_eq": { "mod": 10, "eq": 1 } },
          { "not": { "mod_eq": { "mod": 100, "eq": 11 } } }
        ]
      },
      {
        "cat": "few",
        "all": [
          { "mod_in_list": { "mod": 10, "list": [2, 3, 4] } },
          { "not": { "mod_in_range": { "mod": 100, "range": [12, 14] } } }
        ]
      },
      {
        "cat": "many",
        "any": [
          { "mod_eq": { "mod": 10, "eq": 0 } },
          { "mod_in_range": { "mod": 10, "range": [5, 9] } },
          { "mod_in_range": { "mod": 100, "range": [11, 14] } }
        ]
      },
      { "cat": "other", "true": true }
    ]
  }
}
```

Matching short-string keys:

```json
{
  "Items.one": "{n} item",
  "Items.few": "{n} items",
  "Items.many": "{n} items",
  "Items.other": "{n} items"
}
```

## End-to-end language switching

### 1) Emit event

```cpp
notify(ImGuiX::Events::LangChangeEvent::ForAll("ru"));
```

### 2) What `WindowManager` does

- Queues `LangChangeEvent`.
- During frame processing, applies event to all windows or target window.
- Calls for each target:
  - `requestLanguageChange(lang)`
  - `applyPendingLanguageChange()`

### 3) What `WindowInstance` does

Inside `applyPendingLanguageChange()`:

- Calls `onBeforeLanguageApply(lang)` hook.
- Calls `m_lang_store.set_language(lang)`.
- Calls `m_font_manager.rebuildIfNeeded()`.

## Minimal end-to-end recipe

1. Create resources under `data/resources/i18n/<lang>/` (`*.json` + optional `<doc_key>.md`), keep keys unique.
2. Read localized text in controllers/widgets via `langStore().text(...)`, `label(...)`, `doc(...)`, `text_plural(...)`.
3. Emit language change event: `notify(ImGuiX::Events::LangChangeEvent::ForAll("ru"));`.
4. In window hook, map language to fonts locale:
   `onBeforeLanguageApply(lang) { fontsControl().setLocale(lang); }`.
5. Let `WindowInstance` call `rebuildIfNeeded()`; atlas rebuild applies new glyph coverage.

## i18n + fonts interaction

Language change and font glyph coverage are separate concerns:

- `LangStore` switches text source.
- Font atlas must still contain glyphs for the new language.

Recommended window hook:

```cpp
void onBeforeLanguageApply(const std::string& lang) override {
    fontsControl().setLocale(lang); // mark font atlas dirty
}
```

Then `rebuildIfNeeded()` (already called by `WindowInstance`) rebuilds atlas when needed.

If locale glyph ranges are not present, text can appear as squares.

## Smoke demos to inspect

- `examples/smoke/i18n_demo.cpp`
- `examples/smoke/font_switch_demo.cpp`
- `examples/smoke/widgets_demo.cpp`

## Troubleshooting

### `text()` returns missing marker

- Key is absent in current and default language maps.
- Verify JSON files are loaded from expected `<base>/<lang>/` path.

### Markdown doc is empty

- Missing `<doc_key>.md` in current and fallback language folders.

### Plurals look wrong

- Missing category keys (`Items.few`, etc.) for target language.
- Broken or missing `plurals.json`.

### Language switched but glyphs are broken

- Font atlas not rebuilt for new locale ranges.
- Update font locale (`fontsControl().setLocale(lang)`) and rebuild.
