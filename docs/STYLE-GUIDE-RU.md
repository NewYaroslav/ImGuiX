# Стиль кодирования для библиотеки ImGuiX

## Основные принципы
1. **Совместимость с Dear ImGui**: Максимальное соответствие стилю оригинальной библиотеки
2. **Читаемость**: Четкое визуальное разделение типов и экземпляров
3. **Практичность C++**: Следование общепринятым конвенциям C++ сообщества
4. **Гибридный подход**: Баланс между стилем ImGui и предпочтениями разработчика

## Оглавление
1. [Правила именования](#правила-именования)
2. [Форматирование](#форматирование)
3. [Рекомендации](#рекомендации)
4. [Обоснование](#обоснование)

## Правила именования

### Пространства имен
- **Стиль**: PascalCase
- **Пример**:
  ```cpp
  namespace ImGuiX {
  namespace Internal {
  ```

### Типы (классы, структуры, перечисления, псевдонимы)
- **Стиль**: PascalCase
- **Примеры**:
  ```cpp
  class WidgetManager;
  struct RenderState;
  enum class WidgetType;
  using WidgetPtr = std::shared_ptr<Widget>;
  ```

### Методы и функции (внутри классов)
- **Стиль**: camelCase
- **Примеры**:
  ```cpp
  void initialize();
  void renderFrame();
  bool isVisible() const;
  ```

### Переменные и параметры
- **Стиль**: snake_case
- **Особые случаи**:
  - Приватные члены: префикс `m_` обязателен (`m_event_hub`, `m_task_manager`)
  - Публичные переменные: допускаются без префикса
  - Параметры функций: `snake_case`, допускаются `p_`/`str_`, если параметров больше 5 и они разных типов
- **Примеры**:
  ```cpp
  int m_item_count;
  float window_width;
  std::string str_title;
  void setPosition(int x_pos, int y_pos);
  ```

### Булевые переменные
- **Стиль**: начинать с `is`, `has`, `use`, `enable`
- Приватные: `m_is_`, `m_has_` и т.д.
- ❌ Не использовать префиксы `b_`, `n_`, `f_`
- **Примеры**:
  ```cpp
  bool m_is_active;
  bool has_focus;
  bool enable_shadow;
  ```

### Константы
- **Стиль**: `kPascalCase` или `UPPER_CASE`
- **Рекомендация**: `kPascalCase` для `constexpr`
- **Примеры**:
  ```cpp
  constexpr int kMaxWidgets = 100;
  constexpr float kAnimationSpeed = 0.15f;
  #define IMGUIX_API_VERSION "1.0"
  ```

### События (EventBus)
- **Стиль**: PascalCase с суффиксом `Event`
- **Примеры**:
  ```cpp
  class ButtonClickEvent;
  struct ValueChangedEvent;
  ```

### Макросы защиты заголовочных файлов
- **Формат**: `_PROJECT_MODULE_FILENAME_HPP_INCLUDED`
- **Пример**:
  ```cpp
  #ifndef _IMGUIX_CORE_WIDGET_WIDGET_MANAGER_HPP_INCLUDED
  #define _IMGUIX_CORE_WIDGET_WIDGET_MANAGER_HPP_INCLUDED
  ```

### Полный пример
```cpp
namespace ImGuiX {

class WidgetController : public BaseController {
public:
    void initialize() override {
        m_is_active = true;
    }

    Event<WidgetEvent> widget_created;
    int widget_id = 0;

protected:
    void renderWidget() {
        if (m_show_debug_info) {
            drawDebugInfo(m_widget_position);
        }
    }

private:
    bool m_is_active = false;
    glm::vec2 m_widget_position;
    bool m_show_debug_info = false;
    static constexpr float kWidgetPadding = 5.0f;
};

} // namespace ImGuiX
```

## Форматирование

### Отступы и пробелы
- Используйте 4 пробела; табы запрещены.
- Внутри `namespace` весь код должен иметь дополнительный уровень отступа.
- Мягкое ограничение строки: 120 символов.

### Фигурные скобки
- Стиль K&R: открывающая скобка на той же строке.

### Сигнатуры функций (много параметров)
- Если сигнатура превышает лимит строки, переносите после `(` и размещайте каждый параметр на своей строке с двойным отступом (8 пробелов).
- Закрывающий `) {` выравнивайте с отступом в 4 пробела.

### Объявления классов — множественное наследование
- `:` остаётся на строке с именем класса.
- Каждый базовый класс — на новой строке с двойным отступом (8 пробелов), запятая после каждого, кроме последнего.
- Открывающая `{` остаётся на той же строке, что и последний базовый класс.

```cpp
namespace ImGuiX {

    class FontManager :
        private FontManagerViewCRTP<FontManager>,
        private FontManagerControlCRTP<FontManager> {
    public:
        // ...
    };

} // namespace ImGuiX
```

### Вызовы функций — много аргументов и тернарный оператор
- Переносите после `(` и помещайте каждый аргумент на отдельной строке с двойным отступом (8 пробелов).
- Если аргумент использует тернарный оператор `?:`, переносите после `?` и перед `:`, выравнивая оба варианта под началом тернарного выражения тем же двойным отступом.
- Заключительную `)` размещайте на отдельной строке, выровненной с началом вызова, затем `);`.

**Хорошо:**
```cpp
return ImGui::GetIO().Fonts->AddFontFromFileTTF(
        resolved.c_str(),
        eff_px,
        &cfg,
        ranges.empty() ?
            nullptr :
            ranges.data()
);
```

**Плохо (закрывающая скобка выровнена как аргументы):**
```cpp
return ImGui::GetIO().Fonts->AddFontFromFileTTF(
        resolved.c_str(),
        eff_px,
        &cfg,
        ranges.empty() ?
            nullptr :
            ranges.data()
    );
```

### Стиль указателей и ссылок
- Привязывайте символ к типу: `ImFont*`, `const Foo&`.

### Doxygen
- Используйте комментарии `///` на английском с краткими тегами `@brief`, `@param`, `@return`, `@note`.

### Подключение файлов
- Порядок: заголовок текущего файла, затем заголовки стандартной библиотеки `<...>`, затем внешние или проектные заголовки `"..."`.
- Разделяйте группы пустой строкой.

## Рекомендации
- Предпочитайте ранний возврат, чтобы уменьшить вложенность.
- Делайте функции короткими и сфокусированными.
- Избегайте скрытого глобального состояния; отдавайте предпочтение явным зависимостям.
- Используйте `enum class` для ограниченных перечислений.
- Избегайте макросов в публичном API, если только они не нужны для переносимости.

## Обоснование
- **ImGuiX (PascalCase)**:
  - Соответствует бренду библиотеки
  - Создаёт визуальную связь с `ImGui`
  - Снижает вероятность конфликтов

- **Гибридный стиль**:
  - Совместим с ImGui: `ImGui::Button()` vs `ImGuiX::button()`
  - Отличие типов (PascalCase) от экземпляров (snake_case)
  - Удобство чтения и привычный стиль

### Исключения
- STL-стиль: `widget_manager`, `text_buffer`
- Интерфейсы: `IWidgetHandler`
- Макросы: `UPPER_CASE` для совместимости с ImGui
