# Инструкция по началу работы

Демо показывает локализацию коротких строк, markdown-документов, форматируемых сообщений и форм множественного числа.

## 1) Запуск
- Соберите и запустите пример.
- Откроются два окна («main» и «child»). В главном окне выберите **Language** (`ru` / `en`).

## 2) Структура ресурсов
Корень по умолчанию — `IMGUIX_I18N_DIR` (может резолвиться относительно исполняемого файла):
```
<i18n root>/
  plurals.json              (опционально)
  en/
    strings.json
    md/
      Docs.GettingStarted.md
      About.App.md
  ru/
    strings.json
    md/
      Docs.GettingStarted.md
      About.App.md
```

## 3) Короткие строки и документы
- Используются ключи `text("Menu.File")`, `text("Menu.Edit")` и т.д.  
- Заголовки берутся из `strings.json`, длинный текст — из `md/<Key>.md`.  
  Пример: секция «Getting Started» рендерит заголовок из `Docs.GettingStarted`, а тело — из `md/Docs.GettingStarted.md`.

## 4) Форматирование и плейсхолдеры
- Именованный плейсхолдер: `{name}`  
- Позиционные плейсхолдеры: `{0}`, `{1}`, …
- Примеры вызовов:
  - `textf_key("Errors.ServerUnreachable", fmt::arg("host", host), 503)`
  - `textf_key("Build.Version", 1, 2, 3)`

## 5) Плюрализация
- Для английского: `Items.one`, `Items.other`, `Notifications.one`, `Notifications.other`.  
- Для русского: формы `one / few / many`.  
- В демо значение **n** задаётся полями ввода, вывод:  
  `textf_plural("Items", n, fmt::arg("n", n))`.

## 6) Базовые настройки в UI
- **Settings.General.Username** — текстовое поле.  
- **Settings.Network.Server** — предустановки или **Custom** с произвольным адресом.  
- **Buttons.ResetDefaults** — сброс к настройкам по умолчанию (демонстрация).

> При необходимости меняйте макросы: `IMGUIX_I18N_DIR`, `IMGUIX_RESOLVE_PATHS_REL_TO_EXE`, а также имена файлов JSON/MD.
