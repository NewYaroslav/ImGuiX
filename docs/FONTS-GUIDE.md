# ImGuiX Fonts: guide to FontManager and `fonts*` window methods

This document explains how to load and use fonts in ImGuiX via the `FontManager` and the `WindowInstance` helpers (`fontsBeginManual`, `fontsAddBody`, …). Two scenarios are covered: **JSON auto-config** and **manual assembly**.

> Requirements: ImGui 1.92+, optional ImGui FreeType; SFML/GLFW/SDL2 backend. Perform all calls touching `ImGuiIO::Fonts` **on the GUI thread between frames**.

**Call context:** unless stated otherwise, examples below run inside `WindowInstance::onInit()`. Runtime operations (e.g. `fontsSetLocale()` and `fontsControl().rebuildIfNeeded()`) are allowed between frames in the main loop after initialization.

---

## Basics

* **FontManager** — single point of control for the font atlas (adds TTF/OTF, merges icons/emoji, builds the atlas, updates backend texture).
* **FontRole** — logical roles: `Body`, `H1`, `H2`, `H3`, `Monospace`, `Bold`, `Italic`, `BoldItalic`, `Icons`, `Emoji`.
* **Two modes:**
  1. **Auto (JSON)** — config in `data/resources/fonts/fonts.json`. Suitable for most cases and hot‑reload from code.
  2. **Manual** — full control in `onInit()` through `fonts*` window wrappers.
* **m_dirty** — lazy rebuild flag: setters mark as dirty, `rebuildIfNeeded()` rebuilds at a safe moment.
* **FreeType flags** — aggregated across all files and applied to the entire atlas (`FontLoaderFlags`/`FontBuilderFlags`). Per-font flags are unavailable in this ImGui version.

---

## Quick start (Manual in `WindowInstance::onInit()`)

```cpp
void onInit() override {
    createController<I18nController>();
    create(id() == 0 ? 800 : 640, id() == 0 ? 600 : 480);
    setWindowIcon("data/resources/icons/icon.png");

    // Optional: locale influences fallback ranges in manual mode
    // if you don't set them explicitly (see below). Latin (Default) is always included.
    // fontsSetLocale("ru");

    fontsBeginManual();

    // Combined glyph ranges via preset:
    // Default (Latin) + Cyrillic + Vietnamese + useful punctuation
    fontsSetRangesPreset("Default+Cyrillic+Vietnamese+Punct");

    // Base text font
    fontsAddBody({ "Roboto-Medium.ttf", 18.0f });

    // Merge icons into the Body chain
    fontsAddMerge(ImGuiX::Fonts::FontRole::Icons,
                  { "forkawesome-webfont.ttf", 18.0f, /*merge*/ true });

    // H1 headline (can reuse the same TTF with a different size)
    fontsAddHeadline(ImGuiX::Fonts::FontRole::H1, { "Roboto-Bold.ttf", 24.0f });

    fontsBuildNow();
    // check res.success / res.message if desired
}
```

### When to set locale

* Manual + **preset/explicit ranges set** → locale optional.
* Manual + **no preset/explicit ranges** → locale fallback applies (`Default + locale-specific range`). For example, `ru` adds Cyrillic.

The window method `fontsSetLocale(std::string locale, bool rebuild_now = true)` is available for runtime changes (switch language and rebuild).

---

## JSON auto-config (Auto)

* Default path: `data/resources/fonts/fonts.json` (customizable via `setConfigPath`).
* Minimal `fonts.json` example:

```json
{
  "base_dir": "data/resources/fonts",
  "markdown_sizes": { "body": 16, "h1": 24, "h2": 20, "h3": 18 },
  "locales": {
    "default": {
      "ranges": "Default+Punct",
      "roles": {
        "Body":  [{ "path": "Roboto-Medium.ttf", "size_px": 16 }],
        "Icons": [{ "path": "forkawesome-webfont.ttf", "size_px": 16, "merge": true }]
      }
    },
    "ru": {
      "inherits": "default",
      "ranges": "Default+Cyrillic+Punct"
    }
  }
}
```

* In `onInit()` without manual steps:

```cpp
void onInit() override {
    createController<I18nController>();
    create(800, 600);
    setWindowIcon("data/resources/icons/icon.png");
    // Auto-load from JSON (or defaults if file missing)
    // fontsSetLocale("ru"); // (opt.) switch locale
}
```

### `ranges` in JSON

* String preset: `"Default+Cyrillic+Vietnamese+Punct"` — human-friendly way to combine sets.
* Array of pairs `[start, end, start, end, …, 0]` — explicit ranges. Trailing zero can be omitted—manager adds it.
* `inherits` — inherit roles/ranges from a base locale.

Supported preset tokens (currently):

* `Default` (Latin and basic UI symbols)
* `Cyrillic`, `Vietnamese`
* `Japanese`/`JapaneseFull`, `Chinese`/`ChineseFull`, `Korean` (beware: large sets)
* `Punct` — `– — … • “ ” ‘ ’` (always added and deduplicated)

---

## WindowInstance wrappers: what's available

**Initialization only (inside `onInit()`; guarded by init phase):**

* `fontsBeginManual()` — start manual session.
* `fontsAddBody(FontFile)` — base font.
* `fontsAddHeadline(FontRole::H1|H2|H3, FontFile)` — headlines.
* `fontsAddMerge(FontRole::Icons|Emoji, FontFile)` — merge icon/emoji fonts into the Body chain.
* `fontsBuildNow()` — build atlas and update texture.
* `fontsSetRangesPreset(std::string)` — set ranges via preset (manual).
* `fontsSetRangesExplicit(std::vector<ImWchar>)` — explicit range pairs (manual).
* `fontsClearRanges()` — remove manual ranges (back to locale).

**Runtime facades (safe between frames):**

* `fontsSetLocale(std::string locale, bool rebuild_now = true)` — change locale; optionally rebuild immediately.
* `fontsControl()` — CRTP control: `setDpi`, `setUiScale`, `setMarkdownSizes`, `rebuildIfNeeded`, `getFont`, `activeLocale`, `params`.
* `fontsView()` — CRTP view: access to `getFont`, `activeLocale`, `params` only.

> No unsafe methods at runtime: `beginManual/addFont*/buildNow/clearPacks/setBaseDir` are init-only via `FontManager`/window wrappers.

---

## Nuances and invariants

* **Thread/phase:** any font operations only on the GUI thread between frames. `buildNow()`/`rebuildIfNeeded()` are invalid inside active `NewFrame/Render`.
* **m_dirty:** setters (`setLocale`, `setDpi`, `setUiScale`, `setMarkdownSizes`, `setRanges*`) mark config dirty; rebuild occurs once during `rebuildIfNeeded()`/`buildNow()`.
* **FreeType:**
  * Builder switches automatically if compiled with `IMGUI_ENABLE_FREETYPE` and `use_freetype = true`.
  * Flags accumulate from `FontFile::freetype_flags` and apply to the whole atlas (`FontLoaderFlags`/`FontBuilderFlags`).
* **Ranges:** CJK ranges inflate atlas size—keep separate locales/packs and switch on demand.
* **`extra_glyphs`:** UTF‑8 string of extra characters (e.g. special punctuation). In C++17 `u8"…"` is `const char*` (OK).

---

## Common pitfalls

* **Passing "ru,en" to `setLocale`** — invalid; only one locale. Use range presets (`fontsSetRangesPreset`) to combine.
* **Forgot to call `fontsBuildNow()` in manual mode** — fonts not loaded, `io.FontDefault` remains `nullptr`.
* **Building in wrong phase** — artifacts or crash. Build atlas between frames.
* **Expecting per-font FreeType flags** — in this version they apply to the entire atlas.

---

## Useful snippets

### Minimal manual without presets

```cpp
fontsBeginManual();
fontsSetLocale("ru");               // gives Default + Cyrillic
fontsAddBody({"Roboto-Medium.ttf", 16.0f});
fontsAddMerge(FontRole::Icons, {"forkawesome-webfont.ttf", 16.0f, true});
fontsBuildNow();
```

### Reusing Body TTF for H1/H2/H3

```cpp
fontsBeginManual();
fontsAddBody({"Roboto-Medium.ttf", 16.0f});
// If path is empty, FontManager reuses Body path with a different size
fontsAddHeadline(FontRole::H1, {"", 24.0f});
fontsAddHeadline(FontRole::H2, {"", 20.0f});
fontsAddHeadline(FontRole::H3, {"", 18.0f});
fontsBuildNow();
```

### Changing DPI/scale at runtime

```cpp
auto& fc = fontsControl();
fc.setDpi(120.0f);
fc.setUiScale(1.0f);
auto br = fc.rebuildIfNeeded();
```

---

## File locations

* Fonts: `data/resources/fonts` (default). Base dir changes via `setBaseDir()`.
* i18n: `data/resources/i18n` (including `plurals.json`).
* Font config (JSON): `data/resources/fonts/fonts.json` by default; path changes via `setConfigPath()`.

---

## Diagnostics

* `BuildResult { success, message, fonts }` returned from `buildNow()`/`initFromJsonOrDefaults()`/`rebuildIfNeeded()`.
* On build error: verify TTF paths (absolute/relative to `base_dir`), range correctness, presence of FreeType and ImGui version compatibility.

---

## Integration checklist

* [ ] `FontManager.hpp` included (or `IMGUIX_HEADER_ONLY` for `.ipp`).
* [ ] In `onInit()` pick mode: auto (JSON) **or** manual (`fontsBeginManual` → `fontsAdd*` → `fontsBuildNow`).
* [ ] Ranges set: preset/explicit/locale.
* [ ] For runtime use only `fontsControl()/fontsView()` and `fontsSetLocale()`.
* [ ] Rebuild only between frames (`rebuildIfNeeded()`/`buildNow()`).
