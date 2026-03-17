# Руководство по темам

English version: [THEMES.md](THEMES.md).

Этот документ перечисляет цветовые темы, поставляемые с ImGuiX,
и показывает, как создать и использовать собственные темы.

## Доступные темы

### Встроенные
- **Classic** – классическая цветовая схема ImGui с улучшенными отступами и скруглением от ImGuiX.
- **Light** – светлая палитра для светлых приложений.
- **Dark** – тёмная палитра, похожая на стандартный режим ImGui.

### Дополнительные темы
- **Corporate Grey** – сдержанная серо-тёмная палитра для бизнес-интерфейсов.
- **Dark Charcoal** – тёмные угольные тона с аккуратными отступами.
- **Dark Graphite** – тёмная схема в стиле графита.
- **Dark Teal** – тёмная база с бирюзовыми акцентами.
- **Deep Dark** – почти чёрная тема для высококонтрастных экранов.
- **Gold & Black** – чёрный фон с золотыми акцентами.
- **Green & Blue** – тёмный фон с зелёно‑синими акцентами.
- **Light Blue** – светлый фон с лёгкими голубыми оттенками.
- **Light Green** – светлый фон с мягкими зелёными оттенками.
- **OSX Light** – светлый стиль в духе macOS.
- **Pearl Light** – мягкая светлая тема с голубыми акцентами.
- **Slate Dark** – плотная тёмно‑сланцевая тема с синими акцентами.
- **Visual Studio Dark** – тема, вдохновлённая Visual Studio.
- **Y2K** – светлый хром/серебро, кобальтово‑фиолетовые акценты, глянцевые панели; дух ранних 2000‑х.
- **Cyber‑Y2K** – тёмная технобаза с неоновыми цианово‑маджентовыми и фиолетово‑кислотно‑зелёными акцентами; контрастная палитра с тонкими стеклянными панелями.
- **Cyberpunk UI** – монохромный HUD‑стиль с неоновыми циановыми акцентами.
- **Night Owl** – глубокий тёмно‑синий фон с голубыми/бирюзовыми акцентами.
- **Nord** – спокойная тёмная палитра Nord.
- **Tokyo Night** – тёмная тема с ночным «токийским» настроением и синими/бирюзовыми акцентами.
- **Tokyo Night Storm** – вариант Storm с той же набором акцентов.

## Создание собственной темы

Реализуйте интерфейс `ImGuiX::Themes::Theme` и переопределите методы `apply`.
Используйте `ApplyDefaultImGuiStyle`, чтобы задать общие значения отступов и скруглений.
Не вызывайте `ImGui::GetColorU32` вне активного контекста ImGui.
В конфигурациях храните «сырые» значения `ImVec4` или `ImU32` и переводите их в цвета при отрисовке.

```cpp
class MyTheme final : public ImGuiX::Themes::Theme {
public:
    void apply(ImGuiStyle& style) const override {
        ImGuiX::Themes::ApplyDefaultImGuiStyle(style);
        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
        // настройте остальные цвета...
        style.Colors[ImGuiCol_InputTextCursor] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // цвет курсора
    }
};
```

Зарегистрируйте тему в `ThemeManager`:

```cpp
ImGuiX::Themes::ThemeManager tm;
tm.registerTheme("my-theme", std::make_unique<MyTheme>());
tm.setTheme("my-theme");
// вызывать каждый кадр, чтобы применить при изменении
tm.updateCurrentTheme();
```

Все встроенные темы задают полный набор цветов `ImGuiCol_*`.

## JSON-override поверх базовой темы

Используйте `JsonBackedTheme`, когда встроенная тема должна оставаться базой, а цвета и style-поля должны храниться в текстовом JSON-файле.
Этот механизм намеренно не включён в `imguix/themes.hpp`; заголовок нужно подключать явно через `imguix/themes/JsonBackedTheme.hpp`.

```cpp
#include <imguix/themes/CorporateGreyTheme.hpp>
#include <imguix/themes/JsonBackedTheme.hpp>

auto& tm = themeManager();
ImGuiX::Themes::registerJsonBackedTheme<ImGuiX::Themes::CorporateGreyTheme>(
    tm,
    "corporate-grey",
    "data/themes/corporate-grey.json");
```

Поведение:

- Если файла нет, ImGuiX materialize-ит базовую тему и записывает полный JSON-снимок.
- Если файл уже существует, применяются только распознанные ключи.
- Лишние ключи, неподдерживаемые секции, пропущенные секции и некорректные значения игнорируются.
- JSON-комментарии поддерживаются, потому что файл предварительно проходит через `strip_json_comments(...)`.

Формат файла:

```json
{
  "meta": {
    "schemaVersion": 1
  },
  "imgui": {
    "colors": {
      "ImGuiCol_Text": [1.0, 1.0, 1.0, 1.0]
    },
    "style": {
      "WindowPadding": [8.0, 8.0],
      "WindowRounding": 6.0
    }
  },
  "implot": {
    "colors": {
      "ImPlotCol_AxisTick": [0.45, 0.45, 0.45, 0.70]
    },
    "style": {
      "PlotPadding": [10.0, 10.0]
    }
  },
  "implot3d": {
    "colors": {
      "ImPlot3DCol_FrameBg": [0.42, 0.42, 0.42, 0.54]
    },
    "style": {
      "PlotPadding": [10.0, 10.0]
    }
  }
}
```

Правила:

- Ключи цветов используют полные имена enum: `ImGuiCol_*`, `ImPlotCol_*`, `ImPlot3DCol_*`.
- Ключи `style` должны точно совпадать с именами публичных полей, например `WindowPadding`, `FrameRounding` или `LegendSpacing`.
- `ImVec2` и `ImVec4` хранятся как массивы.
- `float`, `int` и `bool` хранятся как скаляры.
- Enum- и flag-поля сериализуются как целые числа.
- Секции `implot` и `implot3d` опциональны и имеют эффект только если соответствующая поддержка собрана.

## Пользовательские значения темы

Используйте корневую секцию `custom`, когда теме нужны UI-специфичные значения, которые не совпадают со встроенными цветами `ImGuiCol_*` или публичными style-полями.
Это удобно для цветов и параметров, относящихся к вашим собственным виджетам или экранам.
`ImGuiX` предоставляет `ThemeValueSource`, `ResolvedThemeValue<T>`, `tryExtractThemeValue(...)`,
`resolveCurrentThemeCustomColor(...)` и `resolveCurrentThemeCustomValue(...)` в
`imguix/core/themes/theme_value_utils.hpp`.

- `custom.colors` предназначена только для пользовательских цветов `ImVec4`.
- `custom.values` предназначена для пользовательских значений `float`, `int`, `bool`, `ImVec2` и `ImVec4`.
- `by_name` хранит человекочитаемые ключи вроде `SidebarAccountOnline`.
- `by_id` хранит числовые fallback-ключи в виде десятичных строк, например `"1001"`.
- Комбинированный lookup сначала проверяет строковый ключ и только потом использует числовой id, если строкового ключа нет.
- Некорректные размеры массивов, неподдерживаемые JSON-типы, сломанные числовые id и отсутствующие ключи игнорируются без падения загрузки темы.
- Уже существующий валидный JSON темы автоматически не переформатируется.

Пример секции `custom`:

```json
"custom": {
    "colors": {
        "by_name": {
            "SidebarAccountOnline": [0.35, 0.74, 0.39, 1.0]
        },
        "by_id": {
            "1001": [0.95, 0.71, 0.22, 1.0]
        }
    },
    "values": {
        "by_name": {
            "BrokerBadgeRounding": 6.0,
            "UseCompactHeader": true,
            "WidgetPadding": [10.0, 6.0],
            "WidgetCornerVector": [2.0, 2.0, 6.0, 6.0]
        },
        "by_id": {
            "2001": 12.0,
            "2002": false,
            "2003": [16.0, 10.0]
        }
    }
}
```

Примеры lookup-вызовов:

```cpp
const auto color = ImGuiX::Themes::resolveCurrentThemeCustomColor(
    themeManager(),
    "SidebarAccountOnline",
    1001,
    ImVec4(0.40f, 0.40f, 0.40f, 1.0f));

const auto rounding = ImGuiX::Themes::resolveCurrentThemeCustomValue(
    themeManager(),
    "BrokerBadgeRounding",
    2001,
    4.0f);

const auto compact_header = ImGuiX::Themes::resolveCurrentThemeCustomValue(
    themeManager(),
    "UseCompactHeader",
    false);

const auto padding = ImGuiX::Themes::resolveCurrentThemeCustomValue(
    themeManager(),
    2003,
    ImVec2(8.0f, 4.0f));
```

Замечания:

- `custom.colors.*` игнорирует всё, кроме массивов из четырёх компонентов.
- `custom.values.*` игнорирует неподдерживаемые JSON-типы.
- `resolveCurrentThemeCustomColor(...)` работает только с `custom.colors`.
- `resolveCurrentThemeCustomValue(...)` работает только с `custom.values`.
- `float` принимает `float` и `int`; `int` принимает только `int`.
- Несовпадение типа ведёт себя как отсутствующий ключ и сохраняет fallback вызывающего кода.
- Комбинированный lookup предпочитает валидное значение по строковому ключу перед числовым fallback.
- Невалидный тип по строковому ключу не блокирует числовой fallback.
- Используйте `.value` для самого результата и `.source`, чтобы понять, пришёл ли он из строкового ключа, числового id или локального fallback.
- См. `examples/smoke/theme_custom_values_demo.cpp` для runnable-примера.

## Применение тем

### Через `ThemeManager`

```cpp
auto& tm = themeManager();
ImGuiX::Themes::registerCorporateGreyTheme(tm); // или другие register* функции
tm.setTheme(IMGUIX_THEME_CORPORATE_GREY);
tm.updateCurrentTheme();
```

### Через виджеты

```cpp
auto& tm = themeManager();
ImGuiX::Themes::registerCorporateGreyTheme(tm);
ImGuiX::Widgets::ApplyStoredTheme(this);

if (ImGui::CollapsingHeader("Theme", ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGuiX::Widgets::ThemePicker("demo.theme", this);
}
```

`ThemePicker` выводит комбобокс для выбора зарегистрированных тем, а
`ApplyStoredTheme` восстанавливает последнюю выбранную тему, используя настройку `IMGUIX_THEME_STORAGE_KEY`.
