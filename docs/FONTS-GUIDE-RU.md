# ImGuiX Fonts: руководство по FontManager и `fonts*` методам окна

Этот документ объясняет, как подключать и использовать шрифты в ImGuiX c помощью `FontManager` и обёрток `WindowInstance` (`fontsBeginManual`, `fontsAddBody`, …). Описаны два сценария: **автоконфиг из JSON** и **ручная сборка**.

> Требования: ImGui 1.92+, опционально ImGui FreeType; SFML/GLFW/SDL2 backend. Все вызовы, затрагивающие `ImGuiIO::Fonts`, выполняйте **на GUI-потоке между кадрами**.

**Контекст вызовов:** если не указано иначе, примеры ниже выполняются в методе `WindowInstance::onInit()`. Рантайм‑операции (например, `fontsSetLocale()` и `fontsControl().rebuildIfNeeded()`) допустимы между кадрами в основном цикле после инициализации.

---

## Базовые понятия

* **FontManager** — единая точка управления атласом шрифтов (добавляет TTF/OTF, мерджит иконки/эмодзи, строит атлас, обновляет текстуру бэкенда).
* **FontRole** — логические роли: `Body`, `H1`, `H2`, `H3`, `Monospace`, `Bold`, `Italic`, `BoldItalic`, `Icons`, `Emoji`.
* **Два режима**:

  1. **Auto (JSON)** — конфиг в `data/resources/fonts/fonts.json`. Подходит для большинства случаев и хот‑reload из кода.
  2. **Manual** — полный контроль в `onInit()` через обёртки окна `fonts*`.
* **m\_dirty** — ленивый флаг пересборки: сеттеры помечают «грязно», `rebuildIfNeeded()` пересобирает в безопасный момент.
* **FreeType флаги** — агрегируются по всем файлам и применяются к атласу (`FontLoaderFlags`/`FontBuilderFlags`). Пер-шрифт флагов в вашей версии ImGui нет.

---

## Быстрый старт (Manual, в `WindowInstance::onInit()`)

```cpp
void onInit() override {
    createController<I18nController>();
    create(id() == 0 ? 800 : 640, id() == 0 ? 600 : 480);
    setWindowIcon("data/resources/icons/icon.png");

    // Необязательно: локаль влияет на fallback диапазонов в manual,
    // если вы не зададите их явно (см. ниже). Латиница (Default) включена всегда.
    // fontsSetLocale("ru");

    fontsBeginManual();

    // Объединённые диапазоны глифов пресетом:
    // Default (латиница) + кириллица + вьетнамский + полезная пунктуация
    fontsSetRangesPreset("Default+Cyrillic+Vietnamese+Punct");

    // Базовый текстовый шрифт
    fontsAddBody({ "Roboto-Medium.ttf", 18.0f });

    // Иконки мерджим в цепочку Body
    fontsAddMerge(ImGuiX::Fonts::FontRole::Icons,
                  { "forkawesome-webfont.ttf", 18.0f, /*merge*/ true });

    // Заголовок H1 (можно переиспользовать тот же TTF с другим размером)
    fontsAddHeadline(ImGuiX::Fonts::FontRole::H1, { "Roboto-Bold.ttf", 24.0f });

    fontsBuildNow();
    // при желании проверьте res.success / res.message
}
```

### Когда задавать локаль

* Manual + **заданы пресеты/явные диапазоны** → локаль не обязательна.
* Manual + **нет пресетов/явных** → применяется fallback по локали (`Default + специфический диапазон для locale`). Например, `ru` добавит кириллицу.

Под рукой есть метод окна `fontsSetLocale(std::string locale, bool rebuild_now = true)` — для рантайма (смена языка и пересборка).

---

## Автоконфиг из JSON (Auto)

* Путь по умолчанию: `data/resources/fonts/fonts.json` (настраивается `setConfigPath`).
* Пример минимального `fonts.json`:

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

* В `onInit()` без manual:

```cpp
void onInit() override {
    createController<I18nController>();
    create(800, 600);
    setWindowIcon("data/resources/icons/icon.png");
    // Автозагрузка по JSON (или дефолты, если файла нет)
    // fontsSetLocale("ru"); - (опц.) сменить локаль
}
```

### `ranges` в JSON

* Строка‑пресет: `"Default+Cyrillic+Vietnamese+Punct"` — человеческий способ объединять наборы.
* Массив пар `[start, end, start, end, …, 0]` — явное указание диапазонов. Ноль в конце можно опустить — менеджер добавит.
* `inherits` — можно наследовать роли/диапазоны от базовой локали.

Поддерживаемые токены пресета (на сегодня):

* `Default` (латиница и базовые символы UI)
* `Cyrillic`, `Vietnamese`
* `Japanese`/`JapaneseFull`, `Chinese`/`ChineseFull`, `Korean` (осторожно: крупные наборы)
* `Punct` — `– — … • “ ” ‘ ’` (в любом случае добавляется и дедуплицируется)

---

## Обёртки `WindowInstance`: что есть

**Инициализация (только в `onInit()`; защищены фазой init):**

* `fontsBeginManual()` — старт ручной сессии.
* `fontsAddBody(FontFile)` — базовый шрифт.
* `fontsAddHeadline(FontRole::H1|H2|H3, FontFile)` — заголовки.
* `fontsAddMerge(FontRole::Icons|Emoji, FontFile)` — мердж шрифтов иконок/эмодзи в цепочку Body.
* `fontsBuildNow()` — сборка атласа и обновление текстуры.
* `fontsSetRangesPreset(std::string)` — задать диапазоны пресетом (manual).
* `fontsSetRangesExplicit(std::vector<ImWchar>)` — явные пары диапазонов (manual).
* `fontsClearRanges()` — убрать ручные диапазоны (вернуться к локали).

**Рантайм‑фасады (безопасные между кадрами):**

* `fontsSetLocale(std::string locale, bool rebuild_now = true)` — сменить локаль; опц. сразу пересобрать.
* `fontsControl()` — CRTP‑контрол: `setDpi`, `setUiScale`, `setMarkdownSizes`, `rebuildIfNeeded`, `getFont`, `activeLocale`, `params`.
* `fontsView()` — CRTP‑view: только доступ к `getFont`, `activeLocale`, `params`.

> Опасных методов в рантайме нет: `beginManual/addFont*/buildNow/clearPacks/setBaseDir` доступны только в `onInit()` через сам `FontManager`/обёртки окна.

---

## Нюансы и инварианты

* **Поток/фаза:** любые операции со шрифтами — только на GUI‑треде, между кадрами. `buildNow()`/`rebuildIfNeeded()` недопустимы внутри активного `NewFrame/Render`.
* **m\_dirty:** сеттеры (`setLocale`, `setDpi`, `setUiScale`, `setMarkdownSizes`, `setRanges*`) помечают конфиг грязным; сборка делается один раз при `rebuildIfNeeded()`/`buildNow()`.
* **FreeType:**

  * Переключение билдера включается автоматически, если собран с `IMGUI_ENABLE_FREETYPE` и `use_freetype = true`.
  * Флаги копятся из `FontFile::freetype_flags` и выставляются на весь атлас (`FontLoaderFlags`/`FontBuilderFlags`).
* **Диапазоны:** CJK заметно увеличивают атлас — разумно держать отдельные локали/пакеты и переключать по требованию.
* **`extra_glyphs`:** UTF‑8 строка дополнительных символов (например, спец‑пунктуация). В C++17 `u8"…"` — `const char*` (ОК).

---

## Частые ошибки

* **Передача "ru,en" в `setLocale`** — неверно; локаль одна. Для объединения используйте пресет диапазонов (`fontsSetRangesPreset`).
* **Забыли вызвать `fontsBuildNow()` в manual** — шрифты не загрузятся, `io.FontDefault` будет `nullptr`.
* **Сборка в неправильной фазе** — артефакты или падение. Стройте атлас между кадрами.
* **Ожидание пер‑шрифтовых FreeType‑флагов** — в этой версии они применяются ко всему атласу.

---

## Полезные фрагменты

### Минимальный manual без пресетов

```cpp
fontsBeginManual();
fontsSetLocale("ru");               // даст Default + кириллица
fontsAddBody({"Roboto-Medium.ttf", 16.0f});
fontsAddMerge(FontRole::Icons, {"forkawesome-webfont.ttf", 16.0f, true});
fontsBuildNow();
```

### Переиспользование Body TTF для H1/H2/H3

```cpp
fontsBeginManual();
fontsAddBody({"Roboto-Medium.ttf", 16.0f});
// Если не задать явный путь, FontManager возьмёт путь Body и изменит размер
fontsAddHeadline(FontRole::H1, {"", 24.0f});
fontsAddHeadline(FontRole::H2, {"", 20.0f});
fontsAddHeadline(FontRole::H3, {"", 18.0f});
fontsBuildNow();
```

### Смена DPI/масштаба в рантайме

```cpp
auto& fc = fontsControl();
fc.setDpi(120.0f);
fc.setUiScale(1.0f);
auto br = fc.rebuildIfNeeded();
```

---

## Где лежат файлы

* Шрифты: `data/resources/fonts` (по умолчанию). База меняется `setBaseDir()`.
* i18n: `data/resources/i18n` (в т.ч. `plurals.json`).
* Конфиг шрифтов (JSON): `data/resources/fonts/fonts.json` по умолчанию; путь меняется `setConfigPath()`.

---

## Диагностика

* `BuildResult { success, message, fonts }` возвращается из `buildNow()`/`initFromJsonOrDefaults()`/`rebuildIfNeeded()`.
* При ошибке сборки: проверьте пути к TTF (абсолютные/относительные к `base_dir`), корректность диапазонов, наличие FreeType и совместимость версии ImGui.

---

## Чек‑лист интеграции

* [ ] Подключён `FontManager.hpp` (или `IMGUIX_HEADER_ONLY` для `.ipp`).
* [ ] В `onInit()` выбран режим: auto (JSON) **или** manual (`fontsBeginManual` → `fontsAdd*` → `fontsBuildNow`).
* [ ] Диапазоны заданы: пресетом/явно/локалью.
* [ ] Для рантайма используете только `fontsControl()/fontsView()` и `fontsSetLocale()`.
* [ ] Пересборка — только между кадрами (`rebuildIfNeeded()`/`buildNow()`).
