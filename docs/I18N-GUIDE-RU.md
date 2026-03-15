# ImGuiX i18n Guide (RU)

Этот гайд описывает текущую локализацию в ImGuiX: `LangStore`, `PluralRules` и события смены языка.

## Основные компоненты

- `ImGuiX::I18N::LangStore`
  - Загружает локализованные строки из JSON.
  - Лениво загружает markdown-документы (`.md`).
  - Поддерживает форматирование и множественные формы.
- `ImGuiX::I18N::PluralRules`
  - Вычисляет plural category (`one`, `few`, `many`, `other`, ...).
  - Может читать правила из JSON (`plurals.json`).
  - Имеет встроенный fallback для `en` и `ru`.
- `ImGuiX::Events::LangChangeEvent`
  - Запрашивает смену языка глобально или для конкретного окна.

## Пути и макросы

Из `imguix/config/i18n.hpp`:

- `IMGUIX_I18N_DIR` -> по умолчанию `data/resources/i18n`
- `IMGUIX_I18N_JSON_BASENAME` -> по умолчанию `strings.json` (подсказка имени; загрузчик читает все `*.json` в папке языка)
- `IMGUIX_I18N_PLURALS_FILENAME` -> по умолчанию `plurals.json`
- `IMGUIX_RESOLVE_PATHS_REL_TO_EXE` -> резолвить пути относительно exe при ненулевом значении

## Структура ресурсов

Практическая структура текущей реализации:

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

Важно:

- `LangStore` читает все `*.json` в `<base>/<lang>/` и мержит объекты.
- Markdown ищется как `<base>/<lang>/<doc_key>.md`.
- `doc("Docs.GettingStarted")` -> `Docs.GettingStarted.md`.

## Форматы JSON для коротких строк

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

### Массивы строк (конкатенация)

```json
{
  "Banner.Text": ["Hello", " ", "World"]
}
```

Перед парсингом `LangStore` удаляет JSON-комментарии, поэтому комментарии в файлах допустимы.

## API LangStore в ежедневном использовании

- `text(key)`
  - Возвращает локализованную короткую строку.
- `label(key)`
  - Возвращает кешированный `"<text>##<key>"` для стабильных ImGui ID.
- `doc(doc_key)`
  - Возвращает markdown-контент с fallback.
- `textf_key(key, ...)`
  - Форматирует локализованный шаблон.
- `text_plural(base_key, n)`
  - Выбирает pluralized ключ по категории.
- `textf_plural(base_key, n, ...)`
  - То же с форматированием.

Пример:

```cpp
ImGui::TextUnformatted(langStore().text("Menu.File").c_str());
ImGui::InputText(langStore().label("Settings.General.Username"), buf, IM_ARRAYSIZE(buf));
ImGui::Text("%s", langStore().textf_key("Build.Version", 1, 2, 3).c_str());
ImGui::Text("%s", langStore().textf_plural("Items", n, fmt::arg("n", n)).c_str());
```

## Fallback-цепочки

### text(key)

1. Карта текущего языка.
2. Карта языка по умолчанию (`en`).
3. Внутренний маркер отсутствия (`"##null"`).

### text_plural(base_key, n)

1. `<base_key>.<suffix>` в текущем языке.
2. `<base_key>.<suffix>` в языке по умолчанию.
3. `<base_key>` в текущем языке.
4. `<base_key>` в языке по умолчанию.
5. Маркер отсутствия.

## JSON-грамматика PluralRules

`plurals.json` содержит `cardinal`-массивы для каждого языка. Поддерживаемые ключи условий:

- `true`
- `all` (массив под-правил)
- `any` (массив под-правил)
- `not` (одно под-правило)
- `eq`
- `in` (список)
- `range` ([a, b])
- `mod_eq` (`{"mod": M, "eq": E}`)
- `mod_in_list` (`{"mod": M, "list": [...]}`)
- `mod_in_range` (`{"mod": M, "range": [a, b]}`)

Фрагмент:

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

## End-to-end смена языка

### 1) Отправить событие

```cpp
notify(ImGuiX::Events::LangChangeEvent::ForAll("ru"));
```

### 2) Что делает `WindowManager`

- Кладет `LangChangeEvent` в очередь.
- В frame-процессе применяет к окнам.
- Для каждого target вызывает:
  - `requestLanguageChange(lang)`
  - `applyPendingLanguageChange()`

### 3) Что делает `WindowInstance`

В `applyPendingLanguageChange()`:

- Вызывает hook `onBeforeLanguageApply(lang)`.
- Вызывает `m_lang_store.set_language(lang)`.
- Вызывает `m_font_manager.rebuildIfNeeded()`.

## Связка i18n + fonts

Смена языка и покрытие глифов - разные вещи:

- `LangStore` переключает источник текста.
- Font atlas должен содержать глифы нового языка.

Рекомендуемый hook в окне:

```cpp
void onBeforeLanguageApply(const std::string& lang) override {
    fontsControl().setLocale(lang); // пометить atlas как dirty
}
```

После этого `rebuildIfNeeded()` (он уже вызывается `WindowInstance`) выполнит нужную пересборку.

Если диапазоны глифов не покрывают язык, текст станет квадратами.

## Smoke-примеры

- `examples/smoke/i18n_demo.cpp`
- `examples/smoke/font_switch_demo.cpp`
- `examples/smoke/widgets_demo.cpp`

## Troubleshooting

### `text()` вернул маркер отсутствия

- Ключа нет в текущем и fallback языке.
- Проверьте загрузку JSON из нужного `<base>/<lang>/`.

### `doc()` пустой

- Нет `<doc_key>.md` в текущем и fallback языке.

### Неправильные plural forms

- Нет нужных ключей (`Items.few` и т.п.) для языка.
- Ошибка или отсутствие `plurals.json`.

### Язык сменился, но глифы сломаны

- Atlas не пересобран под новый locale/ranges.
- Обновите font locale (`fontsControl().setLocale(lang)`) и пересоберите.
