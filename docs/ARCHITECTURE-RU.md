# Архитектура

English version: [ARCHITECTURE.md](ARCHITECTURE.md).

ImGuiX сочетает подход **Immediate Mode GUI** из Dear ImGui с классическими
объектно‑ориентированными паттернами. Фреймворк организует приложение UI в
чётко определённые компоненты и каналы взаимодействия, благодаря чему большие
проекты остаются поддерживаемыми.

## Структура каталогов

```
.
├── include/            # публичные заголовки
│   └── imguix/         # основные заголовки библиотеки
│       ├── config/     # вспомогательные настройки
│       ├── controllers/  # утилиты контроллеров
│       ├── core/       # базовые модули фреймворка
│       │   ├── application/ # приложение и контекст
│       │   ├── controller/  # базовый класс контроллера
│       │   ├── events/      # встроенные типы событий
│       │   ├── fonts/       # менеджер шрифтов
│       │   ├── i18n/        # интернационализация
│       │   ├── model/       # базовые классы моделей
│       │   ├── notify/      # уведомления
│       │   ├── options/     # хранилище опций
│       │   ├── pubsub/      # шина событий
│       │   ├── resource/    # реестр ресурсов
│       │   ├── themes/      # менеджер тем
│       │   └── window/      # интерфейсы окон
│       ├── extensions/  # утилитарные расширения
│       ├── themes/      # встроенные темы
│       ├── utils/       # вспомогательные функции
│       ├── widgets/     # переиспользуемые виджеты
│       └── windows/     # утилиты для окон
├── docs/               # документация проекта
├── examples/           # примеры приложений
│   └── quickstart/     # минимальный стартовый проект
├── libs/               # включённые зависимости
├── src/                # исходники библиотеки
└── tests/              # тесты и демо
```

## Основные компоненты
- **Application** — владеет глобальными сервисами и главным циклом.
- **WindowManager** — создаёт и отслеживает окна.
- **WindowInstance** — представляет отдельное окно и его контекст рендеринга.
- **Controller** — объединяет покадровую логику и отрисовку.
- **FeatureModel** — лёгкая модель, принадлежащая контроллеру и хранящаяся в реестре.
- **Model** — пользовательские данные или бэкенды вроде `OptionsStore`.
- **EventBus** — асинхронный узел Publisher–Subscriber для развязанного обмена.
- **ResourceRegistry** — потокобезопасный доступ к общим ресурсам (шрифты,
  темы, виджеты и т.п.).

## Архитектурные паттерны
- **Immediate‑Mode MVC**: `WindowInstance` выступает в роли View, подклассы
  `Controller` совмещают логику и отображение, а модели хранят постоянное
  состояние.
- **Событийное взаимодействие**: компоненты отправляют события в `EventBus`;
  слушатели получают уведомления во время `EventBus::process()`.
- **Локальные модели контроллера**: контроллеры могут хранить небольшие
  модели-фичи в типобезопасном реестре; они выполняются в UI-потоке и не
  вызывают ImGui напрямую.
- **Жизненный цикл / Template Method**: окна и контроллеры предоставляют хуки
  `onInit`, `drawContent` и `drawUi`, вызываемые циклом приложения в фиксированном
  порядке.
- **Фабрики**: контроллеры и модели создаются через фабричные функции.
  `WindowInstance::createController<T>()` возвращает ограниченный
  `WindowInterface&`, сохраняя инварианты.
- **Стратегии и расширяемость**: темы, шрифты и виджеты регистрируются динамически.
  `StrategicController` выбирает стратегии, а `ExtendedController` агрегирует
  дочерние элементы.
- **Посредник**: `EventMediator` упрощает управление подписками для всех
  контроллеров и моделей.
- **Реестр ресурсов**: почти синглтон‑реестр; повторная регистрация одного типа
  вызывает ошибку.
- **Контракт событий**: каждое событие наследует `Pubsub::Event` и реализует
  методы `type()` и `name()`.
- **Ограничения моделей**: прямые синхронные вызовы `notify` удалены; вне
  `process()` используйте `notifyAsync`. Внутри `process()` доступен переданный
  `SyncNotifier`.

## Локальные модели (FeatureModel)
`FeatureModel` — компактная модель, принадлежащая одному контроллеру.
Используйте её, когда контроллеру нужно хранить собственное состояние или
выполнять фоновую задачу, а глобальная модель избыточна.

- Хранится в типобезопасном реестре контроллера.
- `process()` выполняется каждый кадр в UI-потоке.
- Избегает прямых вызовов ImGui; при необходимости обменивается событиями.

Контроллеры наследуют `FeatureAccessMixin` для управления моделями-фичами:

```cpp
struct Counter : model::FeatureModel {
    using FeatureModel::FeatureModel;
    int value = 0;
    void process(Pubsub::SyncNotifier&) override { ++value; }
};

class DemoController : public Controller {
public:
    using Controller::Controller;

    void drawContent() override {
        const auto& c = feature<Counter>(
            [&]{ return std::make_unique<Counter>(eventBus()); });
        ImGui::Text("Frames %d", c.value);
        withFeature<Counter>(
            [&]{ return std::make_unique<Counter>(eventBus()); },
            [](Counter& c){ if(ImGui::Button("Reset")) c.value = 0; });
    }
};
```

Для остановки фоновой работы вызовите `requestClose()`,
а для удаления модели — `resetFeature<Counter>()`.

## Обзор системы
```mermaid
graph TD
    A[Application]
    WM[WindowManager]
    W[WindowInstance]
    C[Controller]
    M[Model]
    EB[EventBus]
    RR[ResourceRegistry]

    A-->WM
    A-->M
    A-->EB
    A-->RR
    WM-->W
    W-->C
    C-->EB
    M-->EB
    C-->RR
    M-->RR
```

## Поток событий
```mermaid
sequenceDiagram
    participant Model
    participant EventBus
    participant Controller
    Model->>EventBus: notifyAsync(Event)
    Note right of EventBus: queued
    EventBus-->>EventBus: process()
    EventBus->>Controller: notify(Event)
```

## Структура модулей
```mermaid
graph LR
    core[core]
    windows[windows]
    controllers[controllers]
    widgets[widgets]
    extensions[extensions]
    utils[utils]

    core --> windows
    core --> controllers
    windows --> controllers
    controllers --> widgets
    controllers --> extensions
    controllers --> utils
```

## Этапы жизненного цикла
1. `Application` инициализирует сервисы и `WindowManager`.
2. `WindowManager` создаёт объекты `WindowInstance`.
3. `WindowInstance::onInit()` строит контроллеры через `createController<T>()`.
4. Каждый кадр:
   - события ввода ставятся в очередь `EventBus`;
   - `EventBus::process()` доставляет сообщения;
   - `WindowInstance` вызывает хуки контроллеров (`drawContent`, `drawUi`).
5. Завершение работы выполняет хуки в обратном порядке.

Такая структура сохраняет модульность кода и простоту Immediate Mode‑рендеринга.
