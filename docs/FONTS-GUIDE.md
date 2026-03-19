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

Use these only while building the initial atlas:

- `fontsBeginManual()` starts a new manual configuration buffer.
- `fontsSetRangesPreset(...)` parses a `+`-separated preset string with named and numeric tokens such as `Default+Cyrillic+0x2010-0x205E`.
- `fontsSetRangesExplicit(...)` supplies numeric `[from, to]` pairs directly.
- `fontsClearRanges()` removes both preset and explicit ranges and falls back to locale-based ranges.
- `fontsAddBody(...)` defines the root text font.
- `fontsAddHeadline(...)` defines a role-specific font such as `H1`.
- `fontsAddMerge(...)` merges icon or emoji glyphs into the body chain.
- `fontsBuildNow()` builds the atlas immediately.

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

### Manual setup with mixed preset ranges

```cpp
void onInit() override {
    createController<MyController>();
    create(1280, 720);

    fontsBeginManual();
    fontsSetRangesPreset("Default+Cyrillic+LatinExtA+0x2010-0x205E+0xE000-0xF8FF");

    fontsAddBody({"NotoSans-Regular.ttf", 18.0f});
    fontsAddMerge(ImGuiX::Fonts::FontRole::Icons,
                  {"MaterialIcons-Regular.ttf", 18.0f, 3.0f, true});
    fontsAddHeadline(ImGuiX::Fonts::FontRole::H1,
                     {"NotoSans-Bold.ttf", 24.0f});

    const bool ok = fontsBuildNow();
    (void)ok;
}
```

### Manual setup with explicit numeric ranges

Use `fontsSetRangesExplicit(...)` when you want exact control instead of named tokens:

```cpp
void onInit() override {
    createController<MyController>();
    create(1280, 720);

    fontsBeginManual();
    fontsSetRangesExplicit({
        0x0020, 0x00FF, // Basic Latin + Latin-1 Supplement
        0x0100, 0x017F, // Latin Extended-A
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2010, 0x205E, // punctuation used in UI text
        0xE000, 0xF8FF  // Private Use Area for icon fonts
    });

    fontsAddBody({"NotoSans-Regular.ttf", 18.0f});
    fontsAddMerge(ImGuiX::Fonts::FontRole::Icons,
                  {"MaterialIcons-Regular.ttf", 18.0f, 3.0f, true});
    fontsAddHeadline(ImGuiX::Fonts::FontRole::H1,
                     {"NotoSans-Bold.ttf", 24.0f});

    const bool ok = fontsBuildNow();
    (void)ok;
}
```

Notes:

- `+` is the top-level separator between preset tokens.
- Preset strings may mix named tokens with numeric codepoints and numeric ranges.
- Supported numeric forms are `0x2026`, `8230`, `0xE000-0xF8FF`, and `57344-63743`.
- `U+...` syntax is not supported inside preset strings because `+` is already the token separator.
- Keep raw UTF-8 glyph additions in `extra_glyphs`, not in the preset string.
- For large or mostly numeric definitions, `fontsSetRangesExplicit(...)` is still the clearer API.
- Format is consecutive pairs: `[from1, to1, from2, to2, ...]`.
- Optional trailing `0` is allowed but not required.
- In C++ examples hex notation is usually easier to read than decimal.
- Ranges above `U+FFFF` require `IMGUI_USE_WCHAR32`. ImGuiX enables it in its config, but custom integrations should verify it before adding emoji or other supplementary-plane ranges.

### Using `extra_glyphs` for a few concrete symbols

`extra_glyphs` is a `FontFile` field, not a separate API call. Use it when your normal preset or explicit ranges are intentionally narrow, but you still need a few isolated UTF-8 symbols such as `u8"✓℃"`.

```cpp
void onInit() override {
    createController<MyController>();
    create(1280, 720);

    fontsBeginManual();
    fontsSetRangesPreset("Default+Cyrillic+LatinExtA+0x2010-0x205E");

    ImGuiX::Fonts::FontFile body_font{};
    body_font.path = "NotoSans-Regular.ttf";
    body_font.size_px = 18.0f;

    ImGuiX::Fonts::FontFile symbols_font{};
    symbols_font.path = "NotoSansSymbols2-Regular.ttf";
    symbols_font.size_px = 18.0f;
    symbols_font.merge = true;
    symbols_font.extra_glyphs = u8"\u2713\u2103";

    fontsAddBody(body_font);
    fontsAddMerge(ImGuiX::Fonts::FontRole::Emoji, symbols_font);
    fontsAddHeadline(ImGuiX::Fonts::FontRole::H1,
                     {"NotoSans-Bold.ttf", 24.0f});

    const bool ok = fontsBuildNow();
    (void)ok;
}
```

Notes:

- `extra_glyphs` is added on top of preset, explicit, or locale-based ranges.
- It is best for a few concrete symbols, not for whole Unicode blocks.
- `extra_glyphs` from all configured files are unioned into the final atlas glyph set.
- The font file must actually contain those symbols. `extra_glyphs` only asks the atlas builder to include them.
- If you mainly need numeric coverage, `fontsSetRangesExplicit(...)` is still the better fit.

## Quick Start: Auto mode (JSON)

If you do not call manual `fonts*` setup, `WindowManager` triggers auto init via `FontManager::initFromJsonOrDefaults()`.

Default config path:

- `data/resources/fonts/fonts.json`

### Minimal JSON with mixed preset ranges

```json
{
  "base_dir": "data/resources/fonts",
  "markdown_sizes": { "body": 16, "h1": 24, "h2": 20, "h3": 18 },
  "locales": {
    "default": {
      "ranges": "Default+LatinExtA+0x2010-0x205E+0xE000-0xF8FF",
      "roles": {
        "Body":  [{ "path": "NotoSans-Regular.ttf", "size_px": 16 }],
        "Icons": [{ "path": "MaterialIcons-Regular.ttf", "size_px": 16, "merge": true }]
      }
    },
    "ru": {
      "inherits": "default",
      "ranges": "Default+Cyrillic+LatinExtA+0x2010-0x205E+0xE000-0xF8FF"
    }
  }
}
```

### JSON with explicit numeric ranges

`ranges` may also be a numeric array:

```json
{
  "base_dir": "data/resources/fonts",
  "locales": {
    "default": {
      "ranges": [32, 255, 256, 383, 1024, 1327, 8208, 8286, 57344, 63743],
      "roles": {
        "Body":  [{ "path": "NotoSans-Regular.ttf", "size_px": 16 }],
        "Icons": [{ "path": "MaterialIcons-Regular.ttf", "size_px": 16, "merge": true }]
      }
    }
  }
}
```

Notes:

- JSON uses the same pair format: `[from1, to1, from2, to2, ...]`.
- Use plain numbers in JSON. Hex literals such as `0xE000` are not valid JSON syntax.
- Optional trailing `0` is accepted but usually unnecessary.

### JSON with `extra_glyphs`

```json
{
  "base_dir": "data/resources/fonts",
  "locales": {
    "default": {
      "ranges": "Default+LatinExtA+0x2010-0x205E",
      "roles": {
        "Body": [
          { "path": "NotoSans-Regular.ttf", "size_px": 16 }
        ],
        "Emoji": [
          {
            "path": "NotoSansSymbols2-Regular.ttf",
            "size_px": 16,
            "merge": true,
            "extra_glyphs": "✓℃"
          }
        ]
      }
    }
  }
}
```

Use JSON `extra_glyphs` the same way as in manual mode: for a short UTF-8 list of concrete symbols that should be included on top of the selected ranges.

## Range presets and semantics

Important behavior:

- `fontsSetRangesPreset(...)` and `fontsSetRangesExplicit(...)` are mutually exclusive.
- Last call wins.
- `fontsClearRanges()` removes both preset and explicit values and returns to locale-based fallback.
- If neither preset nor explicit ranges are set in manual mode, fallback is locale-based ranges.
- One preset string may combine named tokens with numeric codepoints and numeric ranges.

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

Numeric token examples:

- `0x2026`
- `8230`
- `0x2010-0x205E`
- `57344-63743`

Use preset strings when readability matters and you want a compact mix of named blocks plus a few extra numeric fragments. Use `extra_glyphs` when only a few concrete UTF-8 symbols are missing and widening the preset would be overkill. Use explicit ranges when you need strict atlas sizing, want to document large numeric lists, or want to avoid string parsing entirely.

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
- Add proper preset tokens or explicit numeric ranges and rebuild.

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
