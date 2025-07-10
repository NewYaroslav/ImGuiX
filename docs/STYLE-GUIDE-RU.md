# Стиль кодирования для библиотеки ImGuiX

## Основные принципы
1. **Совместимость с Dear ImGui**: Максимальное соответствие стилю оригинальной библиотеки
2. **Читаемость**: Четкое визуальное разделение типов и экземпляров
3. **Практичность C++**: Следование общепринятым конвенциям C++ сообщества
4. **Гибридный подход**: Баланс между стилем ImGui и предпочтениями разработчика

## Оглавление
1. [Правила именования](#правила-именования)
2. [Организация кода](#организация-кода)
3. [Форматирование](#форматирование)
4. [Рекомендации](#рекомендации)

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
  - Приватные члены: `m_` префикс обязательно (`m_event_hub`, `m_task_manager`)
  - Публичные переменные: допускаются без префикса
  - Параметры функций: `snake_case`, допускается `p_`/`str_` если параметров больше 5 и они разных типов
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