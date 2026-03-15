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

Используйте только при первичной сборке atlas:

- `fontsBeginManual()`
- `fontsSetRangesPreset(...)`
- `fontsSetRangesExplicit(...)`
- `fontsClearRanges()`
- `fontsAddBody(...)`
- `fontsAddHeadline(...)`
- `fontsAddMerge(...)`
- `fontsBuildNow()`

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
`{ path, size_px, baseline_offset_px, merge, freetype_flags, extra_glyphs }`.
Обычно достаточно только `path` и `size_px`.

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

## Быстрый старт: Auto (JSON)

Если manual-методы не вызваны, `WindowManager` запускает auto-init через `FontManager::initFromJsonOrDefaults()`.

Путь по умолчанию:

- `data/resources/fonts/fonts.json`

Минимальный пример:

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

## Семантика ranges/preset

Ключевые правила:

- `fontsSetRangesPreset(...)` и `fontsSetRangesExplicit(...)` взаимоисключающие.
- Побеждает последний вызов.
- Если в manual не задан ни preset, ни explicit, берутся locale-based ranges.

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

## Как использовать H1 в `Text` из контроллера

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

Если роль не загружена, `getFont(...)` вернет `nullptr`.
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
- Добавьте диапазоны (`Default+Cyrillic+...`) и пересоберите.

### Пропали иконки

- Нет `PUA` в ranges или не замержен icon-font.
- Нужны оба условия.

### Atlas слишком большой

- Слишком много диапазонов/merge-паков.
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
