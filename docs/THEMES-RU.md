# Руководство по темам

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

## Создание собственной темы

Реализуйте интерфейс `ImGuiX::Themes::Theme` и переопределите методы `apply`.
Используйте `applyDefaultImGuiStyle`, чтобы задать общие значения отступов и скруглений.

```cpp
class MyTheme final : public ImGuiX::Themes::Theme {
public:
    void apply(ImGuiStyle& style) const override {
        ImGuiX::Themes::applyDefaultImGuiStyle(style);
        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
        // настройте остальные цвета...
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

