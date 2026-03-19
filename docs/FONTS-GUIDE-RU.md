# ImGuiX Fonts Guide (RU)

Этот гайд описывает текущую работу шрифтов в ImGuiX и безопасные сценарии использования из окна и контроллеров.

## Что делает FontManager

- Централизованно управляет atlas'ом Dear ImGui.
- Поддерживает два режима настройки:
  - Auto из `data/resources/fonts/fonts.json`.
  - Manual в `onInit()` окна через `fonts*` методы.
- Работает с ролями (`FontRole`): `Body`, `H1`, `H2`, `H3`, `Monospace`, `Bold`, `Italic`, `BoldItalic`, `Icons`, `Emoji`.
- Поддерживает runtime-пересборку после изменений конфигурации (`locale`, DPI, scale, markdown sizes).

## Кто что вызывает

| Контекст вызова | Рекомендуемый API |
| --- | --- |
| `WindowInstance::onInit()` / первичная сборка atlas | `fontsBeginManual()`, `fontsSetRanges*()`, `fontsAdd*()`, `fontsBuildNow()` |
| Runtime-хуки (`onBeforeLanguageApply`, сервисные обновления) | `fontsControl().set*()`, `fontsControl().rebuildIfNeeded()` |
| Рендер в контроллере (`drawUi()`, `drawContent()`) | `getFont(FontRole)` + `ImGui::PushFont/ImGui::PopFont` |

## Контекст API: init-only и runtime-safe

### Только на инициализации (обычно `WindowInstance::onInit()`)

Используйте эти методы только при первичной сборке atlas:

- `fontsBeginManual()` начинает новую manual-конфигурацию.
- `fontsSetRangesPreset(...)` разбирает preset-строку с `+`-разделителем, где можно смешивать именованные и числовые токены, например `Default+Cyrillic+0x2010-0x205E`.
- `fontsSetRangesExplicit(...)` задаёт диапазоны напрямую числовыми парами `[from, to]`.
- `fontsClearRanges()` сбрасывает preset и explicit ranges и возвращает fallback к locale-based ranges.
- `fontsAddBody(...)` задаёт корневой текстовый шрифт.
- `fontsAddHeadline(...)` задаёт шрифт для роли вроде `H1`.
- `fontsAddMerge(...)` мерджит glyph'ы иконок или emoji в body chain.
- `fontsBuildNow()` немедленно собирает atlas.

### Безопасно в runtime (между кадрами)

Используйте runtime control facade:

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

В контроллерах используйте `Controller::getFont(FontRole)`.

## Быстрый старт: Manual

Поля `FontFile` при позиционной инициализации:

- `path` - путь к файлу шрифта (абсолютный или относительно fonts base dir).
- `size_px` - размер шрифта при 96 DPI.
- `baseline_offset_px` - вертикальное смещение glyph'ов (`+` вниз, `-` вверх).
- `merge` - мердж glyph'ов в ранее добавленную цепочку шрифта.
- `freetype_flags` - дополнительные флаги билдера FreeType.
- `extra_glyphs` - UTF-8 строка дополнительных glyph'ов.

Обычно достаточно только `path` и `size_px`.

### Manual-настройка через mixed preset ranges

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

### Manual-настройка через explicit numeric ranges

Используйте `fontsSetRangesExplicit(...)`, когда нужен точный контроль, а не именованные токены:

```cpp
void onInit() override {
    createController<MyController>();
    create(1280, 720);

    fontsBeginManual();
    fontsSetRangesExplicit({
        0x0020, 0x00FF, // Basic Latin + Latin-1 Supplement
        0x0100, 0x017F, // Latin Extended-A
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2010, 0x205E, // punctuation для UI-текста
        0xE000, 0xF8FF  // Private Use Area для icon-font
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

Замечания:

- `+` - это верхнеуровневый разделитель токенов preset-строки.
- В preset-строке можно смешивать именованные токены с числовыми codepoint'ами и диапазонами.
- Поддерживаются формы `0x2026`, `8230`, `0xE000-0xF8FF` и `57344-63743`.
- Синтаксис `U+...` внутри preset-строки не поддерживается, потому что `+` уже занят как разделитель токенов.
- Сырые UTF-8 glyph'ы в preset-строке не поддерживаются; для них используйте `extra_glyphs`.
- Для больших или в основном числовых наборов `fontsSetRangesExplicit(...)` всё ещё понятнее.
- Формат такой: `[from1, to1, from2, to2, ...]`.
- Завершающий `0` допустим, но не обязателен.
- В C++ hex-нотация обычно читается лучше, чем decimal.
- Диапазоны выше `U+FFFF` требуют `IMGUI_USE_WCHAR32`. В ImGuiX это уже включено в конфиге, но в кастомной интеграции это стоит проверить перед добавлением emoji и других supplementary-plane ranges.

### Использование `extra_glyphs` для нескольких конкретных символов

`extra_glyphs` - это поле `FontFile`, а не отдельный API-вызов. Используйте его, когда основной preset или explicit ranges специально остаются узкими, но нужно добавить несколько отдельных UTF-8 символов, например `u8"✓℃"`.

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

Замечания:

- `extra_glyphs` добавляется поверх preset, explicit или locale-based ranges.
- Это хороший вариант для нескольких точечных символов, но не для целых Unicode-блоков.
- `extra_glyphs` изо всех настроенных файлов объединяются в итоговом наборе glyph'ов atlas'а.
- Шрифтовой файл должен содержать эти символы. `extra_glyphs` только просит atlas builder включить их в сборку.
- Если основная задача - именно числовое покрытие, то `fontsSetRangesExplicit(...)` всё ещё остаётся лучшим выбором.

## Быстрый старт: Auto (JSON)

Если manual-методы не вызваны, `WindowManager` запускает auto-init через `FontManager::initFromJsonOrDefaults()`.

Путь по умолчанию:

- `data/resources/fonts/fonts.json`

### Минимальный JSON с mixed preset ranges

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

### JSON с explicit numeric ranges

Поле `ranges` может быть и числовым массивом:

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

Замечания:

- JSON использует тот же формат пар: `[from1, to1, from2, to2, ...]`.
- В JSON нужно писать обычные числа. Hex-литералы вроде `0xE000` не являются валидным JSON.
- Завершающий `0` поддерживается, но обычно не нужен.

### JSON с `extra_glyphs`

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

`extra_glyphs` в JSON используется точно так же, как и в manual-настройке: для короткого UTF-8 списка конкретных символов поверх выбранных ranges.

## Семантика ranges/preset

Ключевые правила:

- `fontsSetRangesPreset(...)` и `fontsSetRangesExplicit(...)` взаимоисключающие.
- Побеждает последний вызов.
- `fontsClearRanges()` сбрасывает и preset, и explicit ranges, после чего снова работает locale-based fallback.
- Если в manual не задан ни preset, ни explicit, берутся locale-based ranges.
- В одной preset-строке можно смешивать именованные токены с числовыми codepoint'ами и диапазонами.

Частые токены:

- `Default`
- `Cyrillic`
- `Vietnamese`
- `Japanese`, `Chinese`, `ChineseFull`, `Korean`
- `Punct`
- `PUA` (алиасы `Icons`, `PrivateUse`)
- `Latin1Sup`, `LatinExtA`, `LatinExtB`, `LatinExtAdditional`
- `MiscSymbols` (`Misc`)
- `Dingbats`
- `Arrows`
- `MiscTechnical` (`MiscTech`)
- `EmojiBasic`, `EmojiSupp`, `EmojiPict`, `EmojiTransport`, `EmojiExtA`, `EmojiAll`, `EmojiTGCore`

Примеры числовых токенов:

- `0x2026`
- `8230`
- `0x2010-0x205E`
- `57344-63743`

Preset-строки удобны, когда нужна читаемость и компактное смешивание именованных блоков с несколькими точечными numeric ranges. `extra_glyphs` тоже уместен, когда не хватает всего нескольких конкретных UTF-8 символов и расширять весь preset нецелесообразно. Explicit ranges лучше, когда нужен жёсткий контроль размера atlas, большие числовые списки или полный отказ от строкового парсинга.

## Как использовать H1 и Body из контроллера

```cpp
void drawUi() override {
    ImGui::Begin("Font Switch");

    ImGui::TextUnformatted("Обычный body текст");

    if (ImFont* h1 = getFont(ImGuiX::Fonts::FontRole::H1)) {
        ImGui::PushFont(h1);
        ImGui::TextUnformatted("Заголовок H1");
        ImGui::PopFont();
    } else {
        ImGui::TextUnformatted("H1 недоступен, fallback на текущий шрифт");
    }

    if (ImFont* body = getFont(ImGuiX::Fonts::FontRole::Body)) {
        ImGui::PushFont(body);
        ImGui::TextUnformatted("Явный Body role");
        ImGui::PopFont();
    }

    ImGui::End();
}
```

Если роль не загружена, `getFont(...)` вернёт `nullptr`.
`nullptr` означает, что роль отсутствует в текущем atlas.

## Merge-цепочка (`Icons`/`Emoji`)

- `Body` - корневой шрифт текста.
- `Icons` и `Emoji` мержатся в body chain (`merge=true` во время build).
- Для `Icons`/`Emoji` lookup возвращает указатель на body chain при наличии merge.

## Runtime locale + rebuild

Рекомендуемый runtime-паттерн:

```cpp
auto& fc = fontsControl();
fc.setLocale("ru");
auto br = fc.rebuildIfNeeded();
(void)br;
```

Для глобальной смены языка через `LangChangeEvent` можно обновлять font locale в window hook:

```cpp
void onBeforeLanguageApply(const std::string& lang) override {
    fontsControl().setLocale(lang); // помечает atlas dirty
}
```

`WindowInstance` затем применит язык и вызовет `m_font_manager.rebuildIfNeeded()`.

## Диагностика

### Квадраты вместо текста

- Не хватает нужных диапазонов для скрипта.
- Добавьте подходящие preset-токены или explicit numeric ranges и пересоберите atlas.

### Пропали иконки

- Нет `PUA` в ranges или не замержен icon-font.
- Нужны оба условия.

### Atlas слишком большой

- Слишком много диапазонов или merge-паков.
- Разделите локали или уберите редко используемые блоки.
- Смотрите размер atlas через ImGui metrics (`io.Fonts->TexWidth`, `TexHeight`).

### Роль недоступна (`nullptr`)

- Роль не загружена в текущем pack/manual-конфиге.
- Делайте fallback и проверяйте конфигурацию.

### Init-only API вызван в runtime

- Не вызывайте `fontsBeginManual()`, `fontsAdd*()`, `fontsBuildNow()` в активном frame loop.
- Между кадрами используйте `fontsControl().set*()` и `rebuildIfNeeded()`.

## Полезные пути

- Шрифты: `data/resources/fonts`
- JSON-конфиг шрифтов: `data/resources/fonts/fonts.json`
- i18n ресурсы: `data/resources/i18n`

## Краткий checklist

- Выберите один путь: auto JSON или manual.
- Убедитесь, что ranges покрывают все нужные алфавиты и символы.
- Мержите icon-font для иконок.
- В контроллерах используйте `getFont(...)` + `PushFont/PopFont` с fallback.
- Пересобирайте atlas только между кадрами через `rebuildIfNeeded()`.
