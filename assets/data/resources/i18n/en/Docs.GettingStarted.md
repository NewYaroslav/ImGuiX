# Setup Instructions

This demo shows how to localize short strings, markdown docs, formatted messages, and plural forms.

## 1) Run
- Build and run the sample app.
- Two windows open (“main” and “child”). In the main window select **Language** (`ru` / `en`).

## 2) Resource layout
Default root is `IMGUIX_I18N_DIR` (resolved relative to the executable if enabled):
```
<i18n root>/
  plurals.json              (optional)
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

## 3) Short strings & docs
- Use `text("Menu.File")`, `text("Menu.Edit")`, etc.  
- Headings come from `strings.json`, long text from `md/<Key>.md`.  
  Example: the “Getting Started” section renders heading from `Docs.GettingStarted` and body from `md/Docs.GettingStarted.md`.

## 4) Formatting & placeholders
- Named placeholder: `{name}`  
- Positional placeholders: `{0}`, `{1}`, …
- Example calls:
  - `textf_key("Errors.ServerUnreachable", fmt::arg("host", host), 503)`
  - `textf_key("Build.Version", 1, 2, 3)`

## 5) Pluralization
- English keys: `Items.one`, `Items.other`, `Notifications.one`, `Notifications.other`.
- Russian uses `one / few / many`.  
- The demo varies **n** with inputs and renders:  
  `textf_plural("Items", n, fmt::arg("n", n))`.

## 6) Common settings used in UI
- **Settings.General.Username** — text input.  
- **Settings.Network.Server** — pre-set values or **Custom** with a free-form field.  
- **Buttons.ResetDefaults** — resets to defaults (demo only).

> You can override defaults via macros: `IMGUIX_I18N_DIR`, `IMGUIX_RESOLVE_PATHS_REL_TO_EXE`, filenames for JSON/MD if needed.
