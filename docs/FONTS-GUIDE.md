# ImGuiX Fonts Guide

This guide explains how fonts work in ImGuiX today and how to use them safely from window code and controllers.

## What FontManager provides

- One owner for the Dear ImGui font atlas.
- Two setup modes:
  - Auto mode from `data/resources/fonts/fonts.json`.
  - Manual mode from window `onInit()` via `fonts*` helpers.
- Logical roles (`FontRole`): `Body`, `H1`, `H2`, `H3`, `Monospace`, `Bold`, `Italic`, `BoldItalic`, `Icons`, `Emoji`.
- Runtime rebuild path when configuration changes (`setLocale`, DPI, scale, markdown sizes).

## Who calls what

| Call context | Recommended API |
| --- | --- |
| `WindowInstance::onInit()` / initial atlas setup | `fontsBeginManual()`, `fontsSetRanges*()`, `fontsAdd*()`, `fontsBuildNow()` |
| Runtime hooks (`onBeforeLanguageApply`, service updates) | `fontsControl().set*()`, `fontsControl().rebuildIfNeeded()` |
| Controller render path (`drawUi()`, `drawContent()`) | `getFont(FontRole)` + `ImGui::PushFont/ImGui::PopFont` |

## API Context: Init-only vs Runtime-safe

### Init-only (window setup, typically in `WindowInstance::onInit()`)

Use these only while building initial atlas:

- `fontsBeginManual()`
- `fontsSetRangesPreset(...)`
- `fontsSetRangesExplicit(...)`
- `fontsClearRanges()`
- `fontsAddBody(...)`
- `fontsAddHeadline(...)`
- `fontsAddMerge(...)`
- `fontsBuildNow()`

### Runtime-safe (between frames)

Use runtime control facade when UI is already running:

- `fontsControl().setLocale(...)`
- `fontsControl().setDpi(...)`
- `fontsControl().setUiScale(...)`
- `fontsControl().setMarkdownSizes(...)`
- `fontsControl().rebuildIfNeeded()`
- `fontsControl().getFont(...)`

Read-only facade:

- `fontsView().getFont(...)`
- `fontsView().activeLocale()`
- `fontsView().params()`

From controllers, use `Controller::getFont(FontRole)` (delegates through `WindowInterface`).

## Quick Start: Manual mode

`FontFile` fields in positional form:

- `path` - font file path (absolute or relative to fonts base dir).
- `size_px` - font size at 96 DPI.
- `baseline_offset_px` - vertical glyph offset (`+` down, `-` up).
- `merge` - merge glyphs into previously added font chain.
- `freetype_flags` - optional FreeType builder flags.
- `extra_glyphs` - optional UTF-8 string with extra glyphs to include.

In most setups only `path` and `size_px` are required.

```cpp
void onInit() override {
    createController<MyController>();
    create(1280, 720);

    fontsBeginManual();
    fontsSetRangesPreset("Default+Cyrillic+Punct+PUA+LatinExtA");

    fontsAddBody({"NotoSans-Regular.ttf", 18.0f});
    fontsAddMerge(ImGuiX::Fonts::FontRole::Icons,
                  {"MaterialIcons-Regular.ttf", 18.0f, 3.0f, true});
    fontsAddHeadline(ImGuiX::Fonts::FontRole::H1,
                     {"NotoSans-Bold.ttf", 24.0f});

    const bool ok = fontsBuildNow();
    (void)ok;
}
```

## Quick Start: Auto mode (JSON)

If you do not call manual `fonts*` setup, `WindowManager` triggers auto init via `FontManager::initFromJsonOrDefaults()`.

Default config path:

- `data/resources/fonts/fonts.json`

Minimal example:

```json
{
  "base_dir": "data/resources/fonts",
  "markdown_sizes": { "body": 16, "h1": 24, "h2": 20, "h3": 18 },
  "locales": {
    "default": {
      "ranges": "Default+Punct+PUA",
      "roles": {
        "Body":  [{ "path": "NotoSans-Regular.ttf", "size_px": 16 }],
        "Icons": [{ "path": "MaterialIcons-Regular.ttf", "size_px": 16, "merge": true }]
      }
    },
    "ru": {
      "inherits": "default",
      "ranges": "Default+Cyrillic+Punct+PUA"
    }
  }
}
```

## Range presets and semantics

Important behavior:

- `fontsSetRangesPreset(...)` and `fontsSetRangesExplicit(...)` are mutually exclusive.
- Last call wins.
- If neither preset nor explicit ranges are set in manual mode, fallback is locale-based ranges.

Common preset tokens:

- `Default`
- `Cyrillic`
- `Vietnamese`
- `Japanese`, `Chinese`, `ChineseFull`, `Korean`
- `Punct`
- `PUA` (`Icons`, `PrivateUse` aliases)
- `Latin1Sup`, `LatinExtA`, `LatinExtB`, `LatinExtAdditional`
- `MiscSymbols` (`Misc` alias)
- `Dingbats`
- `Arrows`
- `MiscTechnical` (`MiscTech` alias)
- `EmojiBasic`, `EmojiSupp`, `EmojiPict`, `EmojiTransport`, `EmojiExtA`, `EmojiAll`, `EmojiTGCore`

## H1 / Body font switching in controllers

Use role lookup and standard ImGui push/pop pattern:

```cpp
void drawUi() override {
    ImGui::Begin("Font Switch");

    ImGui::TextUnformatted("Default body text");

    if (ImFont* h1 = getFont(ImGuiX::Fonts::FontRole::H1)) {
        ImGui::PushFont(h1);
        ImGui::TextUnformatted("Headline rendered with H1");
        ImGui::PopFont();
    } else {
        ImGui::TextUnformatted("H1 missing, fallback to current font");
    }

    if (ImFont* body = getFont(ImGuiX::Fonts::FontRole::Body)) {
        ImGui::PushFont(body);
        ImGui::TextUnformatted("Explicit Body role");
        ImGui::PopFont();
    }

    ImGui::End();
}
```

If role is unavailable, `getFont(...)` returns `nullptr`. Always keep a fallback branch.
`nullptr` means the role is absent in the current atlas.

## Merge chain behavior (`Icons`/`Emoji`)

- `Body` is the root font for text.
- `Icons` and `Emoji` are merged into body chain (`merge=true` during build).
- Role lookup for `Icons`/`Emoji` returns the body chain pointer when merge exists.

## Runtime locale and rebuild

Preferred runtime pattern:

```cpp
auto& fc = fontsControl();
fc.setLocale("ru");
auto br = fc.rebuildIfNeeded();
(void)br;
```

For app-wide language switching via `LangChangeEvent`, hook font updates in window override:

```cpp
void onBeforeLanguageApply(const std::string& lang) override {
    fontsControl().setLocale(lang); // marks atlas dirty
}
```

`WindowInstance` then applies language and calls `m_font_manager.rebuildIfNeeded()`.

## Diagnostics and troubleshooting

### Squares instead of text

- Your ranges do not include the script you render.
- Add proper ranges (`Default+Cyrillic+...`) and rebuild.

### Icons missing

- Missing `PUA` in ranges, or icon font is not merged.
- Ensure both are true.

### Atlas too large

- Too many ranges / merged packs.
- Split locales or reduce rarely used blocks.
- Check atlas size via ImGui metrics (`io.Fonts->TexWidth`, `TexHeight`).

### Role unavailable (`nullptr`)

- Role was not loaded in current pack/manual setup.
- Fallback to current ImGui font and verify configuration.

### Init-only API called in runtime

- Avoid `fontsBeginManual()`, `fontsAdd*()`, `fontsBuildNow()` during active frame loop.
- Use `fontsControl().set*()` and `rebuildIfNeeded()` between frames.

## Useful paths

- Fonts directory: `data/resources/fonts`
- Default font config: `data/resources/fonts/fonts.json`
- i18n resources: `data/resources/i18n`

## Practical checklist

- Choose one setup path: auto JSON or manual.
- Set ranges to include all needed scripts and symbols.
- Merge icon font when using icon codepoints.
- Use `getFont(...)` + `PushFont/PopFont` with nullptr fallback.
- Rebuild only between frames via `rebuildIfNeeded()`.
