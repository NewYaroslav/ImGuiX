# Coding Style for the ImGuiX Library

## Core Principles
1. **Compatibility with Dear ImGui**: Follow the style of the original library as closely as possible
2. **Readability**: Clear visual separation of types and instances
3. **C++ Practicality**: Follow widely accepted C++ conventions
4. **Hybrid approach**: Balance between the ImGui style and developer preferences

## Table of Contents
1. [Naming Rules](#naming-rules)
2. [Code Organization](#code-organization)
3. [Formatting](#formatting)
4. [Recommendations](#recommendations)

## Naming Rules

### Namespaces
- **Style**: PascalCase
- **Example**:
  ```cpp
  namespace ImGuiX {
  namespace Internal {
  ```

### Types (classes, structs, enums, aliases)
- **Style**: PascalCase
- **Examples**:
  ```cpp
  class WidgetManager;
  struct RenderState;
  enum class WidgetType;
  using WidgetPtr = std::shared_ptr<Widget>;
  ```

### Methods and functions (inside classes)
- **Style**: camelCase
- **Examples**:
  ```cpp
  void initialize();
  void renderFrame();
  bool isVisible() const;
  ```

### Variables and parameters
- **Style**: snake_case
- **Special cases**:
  - Private members: must use the `m_` prefix (`m_event_hub`, `m_task_manager`)
  - Public variables: allowed without prefix
  - Function parameters: `snake_case`, `p_`/`str_` prefixes allowed if there are more than 5 parameters of different types
- **Examples**:
  ```cpp
  int m_item_count;
  float window_width;
  std::string str_title;
  void setPosition(int x_pos, int y_pos);
  ```

### Boolean variables
- **Style**: start with `is`, `has`, `use`, `enable`
- Private: `m_is_`, `m_has_`, etc.
- ❌ Do not use `b_`, `n_`, `f_` prefixes
- **Examples**:
  ```cpp
  bool m_is_active;
  bool has_focus;
  bool enable_shadow;
  ```

### Constants
- **Style**: `kPascalCase` or `UPPER_CASE`
- **Recommendation**: use `kPascalCase` for `constexpr`
- **Examples**:
  ```cpp
  constexpr int kMaxWidgets = 100;
  constexpr float kAnimationSpeed = 0.15f;
  #define IMGUIX_API_VERSION "1.0"
  ```

### Events (EventBus)
- **Style**: PascalCase with the `Event` suffix
- **Examples**:
  ```cpp
  class ButtonClickEvent;
  struct ValueChangedEvent;
  ```

### Header guard macros
- **Format**: `_PROJECT_MODULE_FILENAME_HPP_INCLUDED`
- **Example**:
  ```cpp
  #ifndef _IMGUIX_CORE_WIDGET_WIDGET_MANAGER_HPP_INCLUDED
  #define _IMGUIX_CORE_WIDGET_WIDGET_MANAGER_HPP_INCLUDED
  ```

### Complete example
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

## Rationale
- **ImGuiX (PascalCase)**:
  - Matches the library brand
  - Creates visual connection with `ImGui`
  - Reduces the chance of conflicts

- **Hybrid style**:
  - Compatible with ImGui: `ImGui::Button()` vs `ImGuiX::button()`
  - Differentiates types (PascalCase) from instances (snake_case)
  - Easy to read and familiar style

### Exceptions
- STL-style names: `widget_manager`, `text_buffer`
- Interfaces: `IWidgetHandler`
- Macros: `UPPER_CASE` for compatibility with ImGui
