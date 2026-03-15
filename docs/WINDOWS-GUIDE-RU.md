# Гайд По Окнам

English version: [WINDOWS-GUIDE.md](WINDOWS-GUIDE.md).

Практический гайд по оконной модели ImGuiX с фокусом на:

- `include/imguix/core/window/WindowInstance.hpp`
- `include/imguix/core/window/WindowInterface.hpp`
- `include/imguix/windows/ImGuiFramedWindow.hpp`

Цель: быстро понять, как выбирать тип окна, настраивать layout/chrome, включать прозрачность и избегать частых артефактов.

## 1. Какой базовый класс выбирать

| Сценарий | Рекомендуемый класс | Почему |
| --- | --- | --- |
| Нужен полностью свой цикл отрисовки и свой chrome | `WindowInstance` | Низкоуровневый контроль окна и lifecycle. |
| Нужна готовая рамка: title bar, side panel, corner icon, системные кнопки | `Windows::ImGuiFramedWindow` | Поверх `WindowInstance`, добавляет готовую геометрию и флаги стиля. |

Правило:

- Обычно начинайте с `ImGuiFramedWindow`.
- Переходите на `WindowInstance`, только если framed-layout ограничивает задачу.

## 2. Lifecycle и pipeline кадра

Есть 2 уровня:

1. `WindowManager` управляет всеми окнами.
2. Каждый `WindowInstance` рендерится в своем frame-cycle.

### Lifecycle в WindowManager

Источник: `include/imguix/core/window/WindowManager.ipp`.

- `prepareFrame()`:
  - `flushPending()`
  - `initializePending()` -> `fontsStartInit()`, `onInit()`, `buildFonts()`
  - `removeClosed()`
  - `initializeControllers()`
- `processFrame()`:
  - `handleEvents()`
  - `tickAll()`
  - `drawContentAll()`
  - `drawUiAll()`
  - `presentAll()`
  - `loadIniAll()`
  - `saveIniAll()`

### Контракт кадра окна

Базовые хуки `WindowInstance`:

- `handleEvents()`
- `tick()`
- `drawContent()`
- `drawUi()`
- `present()`

`ImGuiFramedWindow` переопределяет `drawUi()` (backend-specific `.ipp`) и внутри выбирает:

- `drawClassicLayout(...)` без `HasCornerIconArea`
- `drawCornerLayout(...)` с `HasCornerIconArea`

## 3. Карта API WindowInterface / WindowInstance

### 3.1 Геометрия и состояние

- Идентичность: `id()`, `name()`
- Размеры: `width()`, `height()`, `setSize(int, int)`
- Фокус и видимость: `setVisible(bool)`, `setActive(bool)`, `isActive()`
- Жизненный цикл: `isOpen()`, `close()`
- Состояние окна: `minimize()`, `maximize()`, `restore()`, `isMaximized()`, `toggleMaximizeRestore()`

### 3.2 Theme / options / bus / registry

- Тема: `setTheme(std::string)`, `themeManager()`
- Опции: `options()`
- События: `eventBus()`
- Ресурсы: `registry()`
- Уведомления: `notifications()`

### 3.3 Шрифты и i18n

- `getFont(FontRole)`
- `fontsView()`, `fontsControl()`
- `langStore()`, `requestLanguageChange(...)`, `applyPendingLanguageChange()`
- Init-only API для ручной сборки atlas:
  - `fontsBeginManual()`
  - `fontsSetLocale(...)`
  - `fontsSetRangesPreset(...)`
  - `fontsSetRangesExplicit(...)`
  - `fontsClearRanges()`
  - `fontsAddBody(...)`
  - `fontsAddHeadline(...)`
  - `fontsAddMerge(...)`
  - `fontsBuildNow()`

### 3.4 ImGui ini

- `iniPath()`
- `initIni()`
- `loadIni()`
- `saveIniNow()`

## 4. Модель ImGuiFramedWindow

Главные override-хуки:

- `drawTitleBarText()`
- `drawSidePanel()`
- `drawCornerIcon()`
- `drawMenuBar()`

Режимы layout:

1. Classic mode:
- без `WindowFlags::HasCornerIconArea`.
- title row есть всегда.
- side panel только если `side_panel_width > 0`.

2. Corner mode:
- с `WindowFlags::HasCornerIconArea`.
- выделяется top-left зона под corner icon.
- ширина side panel:
  - `side_panel_width > 0` -> явная
  - `side_panel_width <= 0` -> auto.

## 5. Матрица WindowFlags

Источник: `include/imguix/windows/window_flags.hpp`.

| Флаг | Эффект | Важно |
| --- | --- | --- |
| `HasMenuBar` | Включает menu row и вызов `drawMenuBar()` | В corner mode позиция задается `corner_menu_bar_placement`. |
| `EnableTransparency` | Включает прозрачный native/clear path | Backend-зависимо, см. раздел 7. |
| `DisableBackground` | Делает фон root ImGui host прозрачным | Рамка/бордер при этом остаются видимыми. |
| `ShowControlButtons` | Показывает min/max/close | Геометрия зависит от style и stroke config. |
| `MacStyledControlButtons` | Круглые кнопки в стиле macOS | Взаимоисключается с другими style-флагами. |
| `ImGuiStyledControlButtons` | Кнопки через `Widgets::SystemButton` | Используется как release fallback при конфликте style-флагов. |
| `ClassicStyledControlButtons` | Текстовые `Button`-кнопки | Взаимоисключается с другими style-флагами. |
| `HasCornerIconArea` | Переключает на corner-layout с icon slot | Меняет геометрию title/side. |
| `CornerModeRounding` | Скругление title/side в corner mode | Радиус из `corner_icon_mode_rounding_radius`. |
| `CornerModeBorder` | Включает border-штрихи title/side в corner mode | Толщина из `frame_stroke_thickness`. |
| `DefaultControlButtons` | `ShowControlButtons | ImGuiStyledControlButtons` | Shortcut. |

Политика конфликта стилей кнопок (`resolveControlButtonsStyle()`):

- Debug: assert при нескольких style-флагах.
- Release: fallback к ImGui style.

## 6. Матрица ImGuiFramedWindowConfig

Источник: `include/imguix/windows/ImGuiFramedWindow.hpp`.

### 6.1 Геометрия и рамка

| Поле | Назначение | Default / auto |
| --- | --- | --- |
| `min_width`, `min_height` | Минимальный размер client area | `640x480` |
| `frame_corner_radius` | Радиус native region + округление root host | `8` |
| `resize_border` | Толщина manual resize hit-test | `8` |
| `title_bar_height` | Высота title row | `32` |
| `side_panel_width` | Classic: `<=0` выключает side; Corner: `<=0` auto-width | `0` |
| `frame_stroke_thickness` | Толщина разделителей title/side | `1.0f` |
| `frame_outer_stroke_thickness` | Толщина внешнего host-frame штриха | `2.0f` |
| `frame_inner_stroke_thickness` | Толщина внутреннего host-frame штриха | `1.0f` |

### 6.2 Insets контента

| Поле | Назначение | Auto-режим |
| --- | --- | --- |
| `title_content_left_inset` | Базовый левый inset для `drawTitleBarText` | `<0` -> auto по style |
| `side_panel_content_left_inset` | Базовый левый inset для `drawSidePanel` | `<0` -> auto по style |

### 6.3 Corner-параметры

| Поле | Назначение | Важно |
| --- | --- | --- |
| `corner_icon_mode_rounding_radius` | Радиус скругления corner title/side | Работает при `CornerModeRounding` |
| `corner_rounding_style` | Политика скругления (`Legacy`, `NoTopLeftOnTitleAndSide`) | Работает при `CornerModeRounding` |
| `corner_icon_mode_area_width` | Ширина зоны corner icon | `<0` -> fallback |
| `corner_icon_mode_icon_size` | Размер иконки в corner slot | `<0` -> auto-fit с frame-aware компенсацией |
| `corner_icon_mode_gap` | Gap между icon area и title/side | `<0` -> `style.WindowPadding.x` |
| `corner_menu_bar_placement` | `MainRegion`, `InTitleBar`, `BelowTitleBar` | Имеет смысл только с `HasMenuBar` |

### 6.4 Кнопки и clear color

- `close_button_text`, `minimize_button_text`, `maximize_button_text`: label + hidden ImGui ID для classic text buttons.
- `clear_color`: цвет очистки при выключенной прозрачности.

## 7. Прозрачность и фон: что чем управляет

Часто путают 3 механизма:

1. `WindowFlags::EnableTransparency`
- backend/native-уровень прозрачности.
- В SFML/Win32 включает DWM-путь из `SfmlImGuiFramedWindow.ipp`.

2. `WindowFlags::DisableBackground`
- прозрачный root ImGui host background.

3. `setDisableBackground(bool)`
- runtime toggle того же поведения.

Практика:

- Сначала проверяйте визуал на `DisableBackground`.
- Добавляйте `EnableTransparency`, только если нужна именно native прозрачность backend-окна.

## 8. Backend differences

### SFML (`SfmlImGuiFramedWindow.ipp`)

- Win32 path использует:
  - borderless окно + manual hit-test
  - `SetWindowRgn(CreateRoundRectRgn(...))`
  - DWM path при прозрачности
- `setWindowIcon(...)` в `ImGuiFramedWindow` грузит native icon и texture для corner icon.

Следствие:

- Вне `SetWindowRgn` пикселей нет. Если бордер "пропадает" в углах, геометрию штриха надо держать внутри usable region.

### GLFW / SDL2

- Нет SFML/Win32 rounded-region path.
- `EnableTransparency` влияет на OpenGL clear color, но не на SFML-specific native region.
- `ImGuiFramedWindow::setWindowIcon(...)` сейчас возвращает `false` вне SFML.

## 9. Практические рецепты

### 9.1 Classic: title + menu + side

```cpp
using namespace ImGuiX::Windows;

ImGuiFramedWindowConfig cfg{};
cfg.title_bar_height = 40;
cfg.side_panel_width = 48;

WindowFlags flags =
    WindowFlags::HasMenuBar |
    WindowFlags::DefaultControlButtons;
```

Override:

- `drawTitleBarText()`
- `drawMenuBar()`
- `drawSidePanel()`

### 9.2 Corner V3: icon + side, без menu

```cpp
using namespace ImGuiX::Windows;

ImGuiFramedWindowConfig cfg{};
cfg.title_bar_height = 40;
cfg.side_panel_width = 0; // corner auto-width
cfg.corner_menu_bar_placement = CornerMenuBarPlacement::MainRegion;

WindowFlags flags =
    WindowFlags::DefaultControlButtons |
    WindowFlags::HasCornerIconArea |
    WindowFlags::CornerModeRounding |
    WindowFlags::CornerModeBorder;
```

Для SFML corner-иконки вызывайте `setWindowIcon("...")` в `onInit()`.

### 9.3 Corner без top-left скругления у title/side

```cpp
cfg.corner_rounding_style = CornerRoundingStyle::NoTopLeftOnTitleAndSide;
```

Нужен включенный `WindowFlags::CornerModeRounding`.

### 9.4 Mac-style кнопки

```cpp
WindowFlags flags =
    WindowFlags::ShowControlButtons |
    WindowFlags::MacStyledControlButtons |
    WindowFlags::HasCornerIconArea |
    WindowFlags::CornerModeRounding |
    WindowFlags::CornerModeBorder;
```

Не комбинируйте несколько style-флагов кнопок одновременно.

## 10. Карта smoke-примеров

Источник: `examples/smoke/`.

| Пример | Что демонстрирует |
| --- | --- |
| `corner_icon_area_off_demo` | Classic layout, menu + side panel |
| `corner_icon_area_off_no_side_demo` | Classic layout, menu, side panel off |
| `corner_icon_area_off_no_menu_demo` | Classic layout только с title row |
| `corner_icon_area_off_no_menu_odd_title_demo` | Проверка выравнивания при odd высоте title |
| `corner_icon_area_demo` | Corner layout + menu in title bar |
| `corner_icon_area_demo_v2` | Corner layout + menu below title |
| `corner_icon_area_demo_v3` | Corner layout baseline без menu |
| `corner_icon_area_demo_v3_no_top_left` | Corner layout с `NoTopLeftOnTitleAndSide` |
| `corner_icon_area_demo_v3_mac` | Corner layout + Mac control buttons |

Сборка одного smoke-target из корня:

```powershell
cmake --build external/ImGuiX/build-mingw --target corner_icon_area_demo_v3 --parallel 36
```

Полный профиль: `agents/imguix-smoke-build.md`.

## 11. Частые gotchas

1. Клиппинг углов при SFML/Win32 rounded region
- Вне `SetWindowRgn` пикселей нет.

2. Разное поведение `setWindowIcon`
- В `ImGuiFramedWindow` meaningful `true` сейчас только на SFML.

3. Разная семантика `side_panel_width`
- Classic: `<=0` выключает side panel.
- Corner: `<=0` включает auto-width.

4. `corner_menu_bar_placement` без `HasMenuBar` не работает
- Placement учитывается только при активном menu flag.

5. Конфликты style-флагов control buttons
- Debug assert + release fallback.

## 12. Короткий чеклист для нового окна

1. Выбрать базовый класс (`WindowInstance` или `ImGuiFramedWindow`).
2. Сначала определить `WindowFlags` (режим layout и стиль кнопок).
3. Настроить геометрию/stroke в `ImGuiFramedWindowConfig`.
4. Переопределить только нужные hooks.
5. Проверить как минимум на одном classic и одном corner smoke-сценарии.
6. Для SFML/Win32 проверить углы и бордер во время resize.
