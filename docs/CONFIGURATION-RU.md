# Конфигурация и макросы

English version: [CONFIGURATION.md](CONFIGURATION.md).

ImGuiX использует ряд макросов для подключения дополнительных модулей и задания значений по умолчанию. Ниже приведены основные из них.

## Макросы сборки

Эти макросы включаются автоматически при активации соответствующих опций CMake.

- `IMGUI_ENABLE_FREETYPE` — использование FreeType для растеризации шрифтов (`IMGUIX_IMGUI_FREETYPE`).
- `IMGUI_ENABLE_IMPLOT` — интеграция [ImPlot](https://github.com/epezent/implot) (`IMGUIX_USE_IMPLOT`).
- `IMGUI_ENABLE_IMPLOT3D` — интеграция [ImPlot3D](https://github.com/jimgries/implot3d) (`IMGUIX_USE_IMPLOT3D`).

## Конфигурационные заголовки

В папке `include/imguix/config/` находятся заголовки, задающие пути по умолчанию, цвета, иконки и другие настройки. Подключите `<imguix/config.hpp>`, чтобы получить их все. Значения можно переопределить, определив макросы до подключения или через опции компилятора.

### Пути

- `IMGUIX_CONFIG_DIR` — директория для конфигурационных файлов. По умолчанию `data/config`.
- `IMGUIX_OPTIONS_FILENAME` — имя файла с опциями. По умолчанию `options.json`.

### Шрифты

- `IMGUIX_FONTS_DIR` — директория со шрифтами по умолчанию.
- `IMGUIX_FONTS_CONFIG` — файл конфигурации шрифтов.
- `IMGUIX_FONTS_CONFIG_BASENAME` — базовое имя файла конфигурации.
- `IMGUIX_FONTS_FALLBACK_BODY_BASENAME` — запасной шрифт для текста.
- `IMGUIX_FONTS_FALLBACK_ICONS_BASENAME` — запасной иконный шрифт.

### Локализация

- `IMGUIX_I18N_DIR` — директория ресурсов локализации.
- `IMGUIX_I18N_JSON_BASENAME` — базовое имя файла локализации.
- `IMGUIX_I18N_PLURALS_FILENAME` — файл правил множественного числа.
- `IMGUIX_RESOLVE_PATHS_REL_TO_EXE` — при ненулевом значении пути считаются относительно исполняемого файла.

### Опции

- `IMGUIX_OPTIONS_SAVE_DELAY_SEC` — задержка перед сохранением опций в секундах.

### Шаблоны размеров

- `IMGUIX_SIZING_TIME_SIGNED` — пример строки времени со знаком.
- `IMGUIX_SIZING_TIME_UNSIGNED` — пример строки времени без знака.
- `IMGUIX_SIZING_HMS` — пример строки часов/минут/секунд.
- `IMGUIX_SIZING_DATE` — пример строки даты.
- `IMGUIX_SIZING_YEAR_SIGNED` — пример строки года со знаком.
- `IMGUIX_SIZING_YEAR_UNSIGNED` — пример строки года без знака.
- `IMGUIX_SIZING_WEEKDAYS` — список дней недели через запятую.

### Цвета

- `IMGUIX_COLOR_ERROR` — цвет ошибки.
- `IMGUIX_COLOR_WARNING` — цвет предупреждения.
- `IMGUIX_COLOR_INFO` — информационный цвет.
- `IMGUIX_COLOR_SUCCESS` — цвет успешных действий.
- `IMGUIX_COLOR_OHLC_BULL` — цвет роста OHLC-графиков.
- `IMGUIX_COLOR_OHLC_BEAR` — цвет падения OHLC-графиков.

### Иконки

- `IMGUIX_ICON_KEYBOARD`, `IMGUIX_ICON_HELP`, `IMGUIX_ICON_WARNING`, `IMGUIX_ICON_INFO`, `IMGUIX_ICON_SUCCESS`.
- `IMGUIX_ICON_ADD`, `IMGUIX_ICON_REMOVE`, `IMGUIX_ICON_EYE_SHOW`, `IMGUIX_ICON_EYE_HIDE`, `IMGUIX_ICON_COPY`, `IMGUIX_ICON_PASTE`.

### Уведомления

- `IMGUIX_NOTIFY_ICON_SUCCESS`, `IMGUIX_NOTIFY_ICON_WARNING`, `IMGUIX_NOTIFY_ICON_ERROR`, `IMGUIX_NOTIFY_ICON_INFO`, `IMGUIX_NOTIFY_ICON_CLOSE`.
- `IMGUIX_NOTIFY_COLOR_SUCCESS`, `IMGUIX_NOTIFY_COLOR_WARNING`, `IMGUIX_NOTIFY_COLOR_ERROR`, `IMGUIX_NOTIFY_COLOR_INFO`, `IMGUIX_NOTIFY_COLOR_DEFAULT`.

### Константы темы

`config/theme_config.hpp` содержит константы `ImGuiX::Config` для отступов, скруглений и других метрик интерфейса.

