# Fonts / CREDITS

This folder contains TTF/OTF font files used in the project. Each font is included as-is and licensed according to its original license.

## Font List and Licenses

| Filename                 | Font Family                | License                     | Copyright / Source |
|---------------------------|----------------------------|-----------------------------|--------------------|
| fa-regular-400.ttf        | Font Awesome (fonts only)  | SIL Open Font License 1.1   | © Fonticons, Inc. / fontawesome.com |
| fontawesome-webfont.ttf   | Font Awesome (legacy)      | SIL Open Font License 1.1   | © Fonticons, Inc. / fontawesome.com |
| forkawesome-webfont.ttf   | Fork Awesome               | SIL Open Font License 1.1   | © Fork Awesome authors / forkaweso.me |
| fontaudio.ttf             | Fontaudio                  | SIL Open Font License 1.1   | © Federico Fontana / github.com/fefanto/fontaudio |
| MaterialIcons-Regular.ttf | Material Icons             | Apache License 2.0          | © Google LLC / fonts.google.com/icons |
| Roboto-Bold.ttf           | Roboto                     | Apache License 2.0          | © Google LLC / fonts.google.com |
| Roboto-Italic.ttf         | Roboto                     | Apache License 2.0          | © Google LLC / fonts.google.com |
| Roboto-Light.ttf          | Roboto                     | Apache License 2.0          | © Google LLC / fonts.google.com |
| Roboto-LightItalic.ttf    | Roboto                     | Apache License 2.0          | © Google LLC / fonts.google.com |
| Roboto-Medium.ttf         | Roboto                     | Apache License 2.0          | © Google LLC / fonts.google.com |
| Roboto-Regular.ttf        | Roboto                     | Apache License 2.0          | © Google LLC / fonts.google.com |
| NotoSans-Regular.ttf      | Noto Sans (LGC/VI)         | SIL Open Font License 1.1   | © The Noto Project Authors / github.com/notofonts/latin-greek-cyrillic |
| NotoSans-Bold.ttf         | Noto Sans (LGC/VI)         | SIL Open Font License 1.1   | © The Noto Project Authors / github.com/notofonts/latin-greek-cyrillic |
| NotoSans-Italic.ttf       | Noto Sans (LGC/VI)         | SIL Open Font License 1.1   | © The Noto Project Authors / github.com/notofonts/latin-greek-cyrillic |
| NotoSans-Light.ttf        | Noto Sans (LGC/VI)         | SIL Open Font License 1.1   | © The Noto Project Authors / github.com/notofonts/latin-greek-cyrillic |
| NotoSans-Medium.ttf       | Noto Sans (LGC/VI)         | SIL Open Font License 1.1   | © The Noto Project Authors / github.com/notofonts/latin-greek-cyrillic |
| NotoEmoji-Regular.ttf     | Noto Emoji                 | SIL Open Font License 1.1   | © The Noto Project Authors / github.com/googlefonts/noto-emoji |
| NotoColorEmoji.ttf             | Noto Color Emoji       | SIL Open Font License 1.1 | © The Noto Project Authors / github.com/googlefonts/noto-emoji (releases), fonts.google.com |
| NotoEmoji-Light.ttf            | Noto Emoji (mono)      | SIL Open Font License 1.1 | © The Noto Project Authors / github.com/googlefonts/noto-emoji, fonts.google.com |
| NotoEmoji-Regular.ttf          | Noto Emoji (mono)      | SIL Open Font License 1.1 | © The Noto Project Authors / github.com/googlefonts/noto-emoji, fonts.google.com |
| NotoEmoji-Medium.ttf           | Noto Emoji (mono)      | SIL Open Font License 1.1 | © The Noto Project Authors / github.com/googlefonts/noto-emoji, fonts.google.com |
| NotoEmoji-SemiBold.ttf         | Noto Emoji (mono)      | SIL Open Font License 1.1 | © The Noto Project Authors / github.com/googlefonts/noto-emoji, fonts.google.com |
| NotoEmoji-Bold.ttf             | Noto Emoji (mono)      | SIL Open Font License 1.1 | © The Noto Project Authors / github.com/googlefonts/noto-emoji, fonts.google.com |
| NotoSansSymbols2-Regular.ttf   | Noto Sans Symbols 2    | SIL Open Font License 1.1 | © The Noto Project Authors / notofonts.github.io/symbols, fonts.google.com |
| OpenMoji-black-glyf.ttf        | OpenMoji (Black)       | CC BY-SA 4.0              | © HfG Schwäbisch Gmünd / openmoji.org, github.com/hfg-gmuend/openmoji |

> If you later add East Asian or complex-script families, list them too:
> - `NotoSansCJKsc/…tc/…jp/…kr-*.otf` — SIL OFL 1.1 — © The Noto Project Authors — github.com/notofonts/noto-cjk  
> - `NotoSansArabic-*.ttf`, `NotoNaskhArabic-*.ttf`, `NotoNastaliqUrdu-*.ttf`, `NotoSansDevanagari-*.ttf`, `NotoSansThai-*.ttf` — SIL OFL 1.1 — © The Noto Project Authors — github.com/notofonts

**Download sources (official):**

* **Noto Emoji / Noto Color Emoji:** GitHub → `github.com/googlefonts/noto-emoji` (Releases); Google Fonts → `fonts.google.com` (search “Noto Emoji” / “Noto Color Emoji”); Linux: package `fonts-noto-color-emoji`.
* **Noto Sans Symbols 2:** Noto site → `notofonts.github.io/symbols`; Google Fonts (search “Noto Sans Symbols 2”).
* **OpenMoji Black:** `openmoji.org` → Download; GitHub → `github.com/hfg-gmuend/openmoji` (assets/ttf).

Примечания:

* *Noto Emoji* монохромный; веса (Light/Regular/Medium/SemiBold/Bold) могут быть статическими инстансами variable-шрифта — лицензия OFL 1.1 сохраняется.
* *Noto Color Emoji* цветной; для ImGui нужен FreeType с `LoadColor`.
* Все файлы включены **без изменений**; сохраняйте лицензии в дистрибутивах.

## Licenses

See the [`../licenses/`](../licenses/) directory:
- [`OFL.txt`](../licenses/OFL.txt) — full, unmodified SIL Open Font License 1.1
- [`LICENSE-APACHE-2.0.txt`](../licenses/LICENSE-APACHE-2.0.txt)

All fonts are included **unmodified**. If you subset or otherwise modify fonts, ensure compliance with OFL 1.1 (rename if a Reserved Font Name applies) and keep the license text with distributions.
