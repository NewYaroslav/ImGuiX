#pragma once
#ifndef _IMGUIX_WIDGETS_VIRTUAL_KEYBOARD_HPP_INCLUDED
#define _IMGUIX_WIDGETS_VIRTUAL_KEYBOARD_HPP_INCLUDED

#include <iostream>
/// \file virtual_keyboard.hpp
/// \brief On-screen keyboard with locales (en/ru), auto-height, and simple layout options.
/// ImGui 1.92.* compatible (public API only).

#if defined(_MSC_VER)
#pragma execution_character_set(u8"utf-8")
#endif

#include <imgui.h>
#include <string>
#include <vector>
#include <cctype>
#include <cstring>
#include <algorithm>
#include <regex>

#include <imguix/widgets/controls/icon_button.hpp>
#include <imguix/config/icons.hpp>

namespace ImGuiX::Widgets {

    // ---------------- Config ----------------

    /// \class VirtualKeyboardConfig
    /// \brief
    struct VirtualKeyboardConfig {
        // Заданные размеры. Если <=0 — вычисляется автоматически
        ImVec2  size         = ImVec2(0, 0);   ///< (w,h).
        
        // Геометрия клавиш/отступов
        ImVec2  key_size     = ImVec2(24, 24); ///< key button size
        float   spacing      = 4.0f;           ///< spacing between keys
        float   pane_gap     = 6.0f;           ///< зазор между левой/правой панелью
        float   height_bias_px = 0.0f;         ///<
        bool    border       = true;           ///< child border

        // Состав панелей
        bool    show_digits  = true;           ///< top row 1..0
        bool    show_special = true;           ///< right pane with symbols
        bool    show_bottom  = true;           ///< bottom row (Shift, Space, Bksp, Enter)
        bool    allow_enter  = true;           ///< Enter appends '\n'
        bool    show_accents = true;           ///< allow accents pane
        bool    remember_pane = true;          ///< store pane mode in ImGuiStorage if no external state
        bool    show_clipboard_buttons = false;///< show Copy/Paste buttons
        bool    allow_copy_from_text  = true;  ///< Copy enabled
        bool    allow_paste_into_text = true;  ///< Paste enabled
        bool    submit_on_enter     = false;   ///< true: Enter submits (like InputTextEnterReturnsTrue)
        bool    capture_hw_enter    = false;   ///< react to physical Enter when VK is focused
        bool    shift_enter_newline = false;   ///< Shift+Enter inserts '\n' instead of submit
        std::function<void(const std::string&)> on_submit; ///< callback on submit(text)
    
        bool  paste_replace_all           = false;  ///< true: replace text, false: append
        bool  sanitize_paste_newlines     = true;   ///< convert CRLF/CR -> '\n'
        int   max_text_len_on_paste       = 0;      ///< 0 = unlimited (truncate if >0)
        float clipboard_buttons_spacing   = 0.0f;   ///< 0 => style.ItemSpacing.x
        
        // Иконки/шрифт
        ImFont* icon_font = nullptr;  ///< шрифт иконок (например, MaterialIcons); null => использовать текущий
        ImGuiX::Widgets::IconButtonConfig icon_btn{}; ///< стиль для IconButtonCentered
        std::unordered_map<std::string, const char*> labels {
            {u8"Shift",   u8"Shift"},
            {u8"Space",   u8"Space"},
            {u8"Bksp",    u8"Bksp"},
            {u8"Enter",   u8"Enter"},
            {u8"Symbols", u8"Symbols"},
            {u8"Accents", u8"Accents"},
            {u8"Copy",    IMGUIX_ICON_COPY},
            {u8"Paste",   IMGUIX_ICON_PASTE}
        };
        
        std::unordered_map<std::string, const char*> tooltips{
            {u8"Shift",   u8"Shift key"},
            {u8"Space",   u8"Space key"},
            {u8"Bksp",    u8"Backspace key"},
            {u8"Enter",   u8"Enter key"},
            {u8"Symbols", u8"Switch to symbols"},
            {u8"Accents", u8"Switch to accents"},
            {u8"Copy",    u8"Copy to clipboard"},
            {u8"Paste",   u8"Paste from clipboard"}
        };
        
        // Top preview bar
        bool   show_top_preview      = false;              ///< draw preview line above keyboard
        float  preview_height        = 0.0f;               ///< 0 => ImGui::GetFrameHeight()
        bool   preview_border        = true;               ///< draw border around preview bar
        ImVec4 preview_bg_col        = ImVec4(0,0,0,0);    ///< 0 alpha => no background
        ImVec4 preview_text_col      = ImVec4(1,1,1,1);    ///< normal preview text color
        ImVec4 preview_text_invalid  = ImVec4(0.95f,0.35f,0.35f,1.0f); ///< invalid color

        // Validation (regex)
        bool        validate_with_regex = false;           ///< enable regex validation
        const char* validate_regex      = nullptr;         ///< ECMA regex (std::regex)
        
        // Locale/Layouts
        std::vector<const char*> locales = {
            u8"en",
            u8"es",u8"pt-BR",u8"pt",u8"fr",u8"de",u8"it",
            u8"ru",u8"uk",u8"be",
            u8"tr",u8"pl",
            u8"id",u8"vi",u8"th",
            u8"ar",u8"hi",u8"ur"
        };
        int     start_locale_index = 0;        ///< index inside locales

        // Optional header height (for locale combo). If 0 => measured from frame height.
        float   header_extra_h = 0.0f;         ///<
        
        // Размещение селектора локали
        bool    locale_bottom_right  = true;   ///< true: внизу справа; false: сверху (как раньше)
        bool    show_locale_combo    = true;   ///< show Combo to choose locale
        float   locale_width         = 64.0f;  ///< ширина превью "en"/"ru" (компактно)
        
        float   left_ratio           = 0.60f;  /// < Доли ширины панелей при заданной ширине (size.x>0)
    
        // --- bottom row auto widths ---
        bool  auto_bottom_width      = true;   ///< auto width for Shift/Bksp/Enter
        float min_wide_key_factor    = 1.6f;   ///< fallback: min wide W = key_size.x * factor
        float space_width_multiplier = 2.5f;   ///< Space width = wide * multiplier (2..3)
    };
    
    enum class VKSpecialPane : int { Symbols = 0, Accents = 1 };
    
    /// \class VirtualKeyboardState
    /// \brief
    struct VirtualKeyboardState {
        int  locale_index = 0;  ///< текущая локаль
        bool shift = false;     ///< состояние Shift
        bool submitted = false;   ///< one-frame flag: true when Enter triggers submit
        VKSpecialPane pane = VKSpecialPane::Symbols; ///< right pane mode
    };
    
    // --- Accents data (lower/upper where meaningful). Compact, pragmatic sets ---

    struct AccentsSet { 
        const char* const* items; 
        int count; 
    };

    inline AccentsSet get_accents_for_locale(const char* lc) {
        // ES
        static const char* ACC_ES[] = {
            u8"ñ", u8"Ñ", u8"á", u8"é", u8"í", u8"ó", u8"ú", u8"ü", u8"¡", u8"¿"
        };
        // PT (+BR)
        static const char* ACC_PT[] = { u8"ç",u8"Ç",u8"ã",u8"Ã",u8"õ",u8"Õ",u8"á",u8"é",u8"í",u8"ó",u8"ú" };
        // DE
        static const char* ACC_DE[] = { u8"ä",u8"Ä",u8"ö",u8"Ö",u8"ü",u8"Ü",u8"ß",u8"ẞ" };
        // FR
        static const char* ACC_FR[] = { u8"ç",u8"Ç",u8"œ",u8"Œ",u8"æ",u8"Æ",u8"é",u8"è",u8"ê",u8"ë",u8"â",u8"î",u8"ô",u8"û",u8"ï",u8"ü",u8"à",u8"ù",u8"ç" };
        // IT
        static const char* ACC_IT[] = { u8"à",u8"è",u8"é",u8"ì",u8"ò",u8"ù",u8"À",u8"È",u8"É",u8"Ì",u8"Ò",u8"Ù" };
        // PL
        static const char* ACC_PL[] = { u8"ą",u8"Ą",u8"ć",u8"Ć",u8"ę",u8"Ę",u8"ł",u8"Ł",u8"ń",u8"Ń",u8"ó",u8"Ó",u8"ś",u8"Ś",u8"ź",u8"Ź",u8"ż",u8"Ż" };
        // TR
        static const char* ACC_TR[] = { u8"ç",u8"Ç",u8"ğ",u8"Ğ",u8"ı",u8"İ",u8"ö",u8"Ö",u8"ş",u8"Ş",u8"ü",u8"Ü" };

        // Fallback (id/en/ru/uk/be/…)
        static const AccentsSet ACC_EMPTY{ nullptr, 0 };
        
        if (!lc) return ACC_EMPTY;
        auto eq = [](const char* a, const char* b){ return a && std::strcmp(a,b)==0; };
        if (eq(lc,u8"es"))   return AccentsSet{ ACC_ES, (int)(sizeof(ACC_ES)/sizeof(*ACC_ES)) };
        if (eq(lc,u8"pt") || eq(lc,u8"pt-BR"))
                           return AccentsSet{ ACC_PT, (int)(sizeof(ACC_PT)/sizeof(*ACC_PT)) };
        if (eq(lc,u8"de"))   return AccentsSet{ ACC_DE, (int)(sizeof(ACC_DE)/sizeof(*ACC_DE)) };
        if (eq(lc,u8"fr"))   return AccentsSet{ ACC_FR, (int)(sizeof(ACC_FR)/sizeof(*ACC_FR)) };
        if (eq(lc,u8"it"))   return AccentsSet{ ACC_IT, (int)(sizeof(ACC_IT)/sizeof(*ACC_IT)) };
        if (eq(lc,u8"pl"))   return AccentsSet{ ACC_PL, (int)(sizeof(ACC_PL)/sizeof(*ACC_PL)) };
        if (eq(lc,u8"tr"))   return AccentsSet{ ACC_TR, (int)(sizeof(ACC_TR)/sizeof(*ACC_TR)) };
        return ACC_EMPTY;
    }

    // -------------- Internal: layouts -----------

    struct KBLayout {
        const char* name;             // "en", "ru", ...
        // rows of letters (qwerty style). Counts used to render.
        const char* row2[16]; int row2_n;
        const char* row3[16];  int row3_n;
        const char* row4[16];  int row4_n;

        // Uppercase variants (for Shift ON). For ASCII we can compute upper on-the-fly,
        // but for Cyrillic we need explicit strings.
        const char* row2U[16]; int row2U_n;
        const char* row3U[16];  int row3U_n;
        const char* row4U[16];  int row4U_n;
    };

    inline const KBLayout& get_layout(const char* name) {
        // English (ASCII): uppercase computed at press-time for ASCII
        static const KBLayout EN {
            u8"en",
            {u8"q",u8"w",u8"e",u8"r",u8"t",u8"y",u8"u",u8"i",u8"o",u8"p"}, 10,
            {u8"a",u8"s",u8"d",u8"f",u8"g",u8"h",u8"j",u8"k",u8"l"},     9,
            {u8"z",u8"x",u8"c",u8"v",u8"b",u8"n",u8"m"},             7,
            // uppercase placeholders (not used; will be toupper for ASCII)
            {u8"Q",u8"W",u8"E",u8"R",u8"T",u8"Y",u8"U",u8"I",u8"O",u8"P"}, 10,
            {u8"A",u8"S",u8"D",u8"F",u8"G",u8"H",u8"J",u8"K",u8"L"},      9,
            {u8"Z",u8"X",u8"C",u8"V",u8"B",u8"N",u8"M"},              7
        };
        
        // ES (Spanish): add ñ / Ñ
        static const KBLayout ES {
            u8"es",
            {u8"q",u8"w",u8"e",u8"r",u8"t",u8"y",u8"u",u8"i",u8"o",u8"p"}, 10,
            {u8"a",u8"s",u8"d",u8"f",u8"g",u8"h",u8"j",u8"k",u8"l",u8"ñ"}, 10,
            {u8"z",u8"x",u8"c",u8"v",u8"b",u8"n",u8"m"},                    7,
            {u8"Q",u8"W",u8"E",u8"R",u8"T",u8"Y",u8"U",u8"I",u8"O",u8"P"}, 10,
            {u8"A",u8"S",u8"D",u8"F",u8"G",u8"H",u8"J",u8"K",u8"L",u8"Ñ"}, 11,
            {u8"Z",u8"X",u8"C",u8"V",u8"B",u8"N",u8"M"},                    7
        };
        
        // PT (Portuguese, incl. pt-BR): add ç / Ç and ã / Ã, õ / Õ
        static const KBLayout PT {
            u8"pt",
            {u8"q",u8"w",u8"e",u8"r",u8"t",u8"y",u8"u",u8"i",u8"o",u8"p"},      10,
            {u8"a",u8"s",u8"d",u8"f",u8"g",u8"h",u8"j",u8"k",u8"l",u8"ç"},      10,
            {u8"z",u8"x",u8"c",u8"v",u8"b",u8"n",u8"m",u8"ã",u8"õ"},          9,
            {u8"Q",u8"W",u8"E",u8"R",u8"T",u8"Y",u8"U",u8"I",u8"O",u8"P"},      10,
            {u8"A",u8"S",u8"D",u8"F",u8"G",u8"H",u8"J",u8"K",u8"L",u8"Ç"},      10,
            {u8"Z",u8"X",u8"C",u8"V",u8"B",u8"N",u8"M",u8"Ã",u8"Õ"},          9
        };
        
        // DE (German): add ä ö ü ß (ẞ uppercase)
        static const KBLayout DE {
            u8"de",
            {u8"q",u8"w",u8"e",u8"r",u8"t",u8"z",u8"u",u8"i",u8"o",u8"p"},         10, // 'z' position per DE habit
            {u8"a",u8"s",u8"d",u8"f",u8"g",u8"h",u8"j",u8"k",u8"l",u8"ä",u8"ö"},     11,
            {u8"y",u8"x",u8"c",u8"v",u8"b",u8"n",u8"m",u8"ü",u8"ß"},             9,
            {u8"Q",u8"W",u8"E",u8"R",u8"T",u8"Z",u8"U",u8"I",u8"O",u8"P"},         10,
            {u8"A",u8"S",u8"D",u8"F",u8"G",u8"H",u8"J",u8"K",u8"L",u8"Ä",u8"Ö"},     11,
            {u8"Y",u8"X",u8"C",u8"V",u8"B",u8"N",u8"M",u8"Ü",u8"ẞ"},             9
        };
        
        // FR (French): add ç / Ç, œ / Œ, æ / Æ (acute etc. via accent panel later)
        static const KBLayout FR {
            u8"fr",
            {u8"q",u8"w",u8"e",u8"r",u8"t",u8"y",u8"u",u8"i",u8"o",u8"p"},         10,
            {u8"a",u8"s",u8"d",u8"f",u8"g",u8"h",u8"j",u8"k",u8"l",u8"ç",u8"œ"},     11,
            {u8"z",u8"x",u8"c",u8"v",u8"b",u8"n",u8"m",u8"æ"},                  8,
            {u8"Q",u8"W",u8"E",u8"R",u8"T",u8"Y",u8"U",u8"I",u8"O",u8"P"},         10,
            {u8"A",u8"S",u8"D",u8"F",u8"G",u8"H",u8"J",u8"K",u8"L",u8"Ç",u8"Œ"},     11,
            {u8"Z",u8"X",u8"C",u8"V",u8"B",u8"N",u8"M",u8"Æ"},                  8
        };
        
        // IT (Italian): common accented vowels (minimal set)
        static const KBLayout IT {
            u8"it",
            {u8"q",u8"w",u8"e",u8"r",u8"t",u8"y",u8"u",u8"i",u8"o",u8"p"},               10,
            {u8"a",u8"s",u8"d",u8"f",u8"g",u8"h",u8"j",u8"k",u8"l",u8"à",u8"è",u8"é"},       12,
            {u8"z",u8"x",u8"c",u8"v",u8"b",u8"n",u8"m",u8"ì",u8"ò",u8"ù"},               10,
            {u8"Q",u8"W",u8"E",u8"R",u8"T",u8"Y",u8"U",u8"I",u8"O",u8"P"},               10,
            {u8"A",u8"S",u8"D",u8"F",u8"G",u8"H",u8"J",u8"K",u8"L",u8"À",u8"È",u8"É"},       12,
            {u8"Z",u8"X",u8"C",u8"V",u8"B",u8"N",u8"M",u8"Ì",u8"Ò",u8"Ù"},               10
        };
        
        // PL (Polish): ą ć ę ł ń ó ś ź ż
        static const KBLayout PL {
            u8"pl",
            {u8"q",u8"w",u8"e",u8"r",u8"t",u8"y",u8"u",u8"i",u8"o",u8"p"},                10,
            {u8"a",u8"s",u8"d",u8"f",u8"g",u8"h",u8"j",u8"k",u8"l",u8"ł",u8"ó"},            11,
            {u8"z",u8"x",u8"c",u8"v",u8"b",u8"n",u8"m",u8"ą",u8"ć",u8"ę",u8"ń",u8"ś",u8"ź",u8"ż"},14,
            {u8"Q",u8"W",u8"E",u8"R",u8"T",u8"Y",u8"U",u8"I",u8"O",u8"P"},                10,
            {u8"A",u8"S",u8"D",u8"F",u8"G",u8"H",u8"J",u8"K",u8"L",u8"Ł",u8"Ó"},            11,
            {u8"Z",u8"X",u8"C",u8"V",u8"B",u8"N",u8"M",u8"Ą",u8"Ć",u8"Ę",u8"Ń",u8"Ś",u8"Ź",u8"Ż"},14
        };
        
        // TR (Turkish): ç ğ ı İ ö ş ü
        static const KBLayout TR {
            u8"tr",
            {u8"q",u8"w",u8"e",u8"r",u8"t",u8"y",u8"u",u8"ı",u8"o",u8"p"},            10, // dotless ı
            {u8"a",u8"s",u8"d",u8"f",u8"g",u8"h",u8"j",u8"k",u8"l",u8"ş",u8"ç"},        11,
            {u8"z",u8"x",u8"c",u8"v",u8"b",u8"n",u8"m",u8"ö",u8"ü",u8"ğ"},            10,
            {u8"Q",u8"W",u8"E",u8"R",u8"T",u8"Y",u8"U",u8"İ",u8"O",u8"P"},            10, // dotted İ
            {u8"A",u8"S",u8"D",u8"F",u8"G",u8"H",u8"J",u8"K",u8"L",u8"Ş",u8"Ç"},        11,
            {u8"Z",u8"X",u8"C",u8"V",u8"B",u8"N",u8"M",u8"Ö",u8"Ü",u8"Ğ"},            10
        };

        // Russian (полный набор с Ё)
        static const KBLayout CYR_RU {
            u8"cyr-ru",
            {u8"й",u8"ц",u8"у",u8"к",u8"е",u8"н",u8"г",u8"ш",u8"щ",u8"з",u8"х",u8"ъ"}, 12,
            {u8"ф",u8"ы",u8"в",u8"а",u8"п",u8"р",u8"о",u8"л",u8"д",u8"ж",u8"э"},     11,
            {u8"я",u8"ч",u8"с",u8"м",u8"и",u8"т",u8"ь",u8"б",u8"ю",u8"ё"},         10,
            {u8"Й",u8"Ц",u8"У",u8"К",u8"Е",u8"Н",u8"Г",u8"Ш",u8"Щ",u8"З",u8"Х",u8"Ъ"}, 12,
            {u8"Ф",u8"Ы",u8"В",u8"А",u8"П",u8"Р",u8"О",u8"Л",u8"Д",u8"Ж",u8"Э"},     11,
            {u8"Я",u8"Ч",u8"С",u8"М",u8"И",u8"Т",u8"Ь",u8"Б",u8"Ю",u8"Ё"},         10
        };
        
        // Ukrainian (добавлены І, Ї, Є, Ґ)
        static const KBLayout CYR_UK {
            u8"cyr-uk",
            {u8"й",u8"ц",u8"у",u8"к",u8"е",u8"н",u8"г",u8"ш",u8"щ",u8"з",u8"х",u8"ї"}, 12,
            {u8"ф",u8"і",u8"в",u8"а",u8"п",u8"р",u8"о",u8"л",u8"д",u8"ж",u8"є"},     11,
            {u8"я",u8"ч",u8"с",u8"м",u8"и",u8"т",u8"ь",u8"б",u8"ю",u8"ґ"},         10,
            {u8"Й",u8"Ц",u8"У",u8"К",u8"Е",u8"Н",u8"Г",u8"Ш",u8"Щ",u8"З",u8"Х",u8"Ї"}, 12,
            {u8"Ф",u8"І",u8"В",u8"А",u8"П",u8"Р",u8"О",u8"Л",u8"Д",u8"Ж",u8"Є"},     11,
            {u8"Я",u8"Ч",u8"С",u8"М",u8"И",u8"Т",u8"Ь",u8"Б",u8"Ю",u8"Ґ"},         10
        };

        // Belarusian (добавлены Ў, І, Ё)
        static const KBLayout CYR_BE {
            u8"cyr-be",
            {u8"й",u8"ц",u8"у",u8"к",u8"е",u8"н",u8"г",u8"ш",u8"ў",u8"з",u8"х",u8"'",}, 12, // заменяем щ → ў
            {u8"ф",u8"і",u8"в",u8"а",u8"п",u8"р",u8"о",u8"л",u8"д",u8"ж",u8"э"},      11,
            {u8"я",u8"ч",u8"с",u8"м",u8"і",u8"т",u8"ь",u8"б",u8"ю",u8"ё"},          10,
            {u8"Й",u8"Ц",u8"У",u8"К",u8"Е",u8"Н",u8"Г",u8"Ш",u8"Ў",u8"З",u8"Х",u8"\"",},12,
            {u8"Ф",u8"І",u8"В",u8"А",u8"П",u8"Р",u8"О",u8"Л",u8"Д",u8"Ж",u8"Э"},      11,
            {u8"Я",u8"Ч",u8"С",u8"М",u8"І",u8"Т",u8"Ь",u8"Б",u8"Ю",u8"Ё"},          10
        };

        if (!name) return EN;
        auto eq = [](const char* a, const char* b){ return a && std::strcmp(a,b)==0; };

        if (eq(name,u8"ru")) return CYR_RU;
        if (eq(name,u8"uk")) return CYR_UK;
        if (eq(name,u8"be")) return CYR_BE;
        
         // Latin with dedicated layouts
        if (eq(name,u8"es"))     return ES;
        if (eq(name,u8"pt") || eq(name,u8"pt-BR")) return PT;
        if (eq(name,u8"de"))     return DE;
        if (eq(name,u8"fr"))     return FR;
        if (eq(name,u8"it"))     return IT;
        if (eq(name,u8"pl"))     return PL;
        if (eq(name,u8"tr"))     return TR;
        // es, pt, pt-BR, de, fr, it, pl, tr, id, vi, th, ar, hi, ur и т.д. → EN (временный fallback)
        
        // Fallbacks
        // Indonesian (id) and anything else latin -> EN
        return EN;
    }

    // -------------- Helpers ---------------------

    inline void append_char_ascii(std::string& text, const char* label, bool shift, bool& modified) {
        // ASCII single-char label
        char ch = label[0];
        if (std::isalpha(static_cast<unsigned char>(ch)) && shift)
            ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
        text.push_back(ch);
        modified = true;
    }

    inline void append_utf8(std::string& text, const char* s, bool& modified) {
        text += s;
        modified = true;
    }

    // --- расчёт минимально достаточной ширины контента (без внешних паддингов) ---
    inline float calc_min_inner_width(
            const VirtualKeyboardConfig& cfg,
            int left_cols_max,
            int right_cols) {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float pad_x   = style.WindowPadding.x;
        const float child_b = style.ChildBorderSize;

        // Если ключи/спейсинг не заданы, подхватываем из текущего стиля
        const float k   = (cfg.key_size.x > 0.0f) ? cfg.key_size.x : ImGui::GetFrameHeight();
        const float sp  = (cfg.spacing   > 0.0f) ? cfg.spacing     : style.ItemSpacing.x;
        const float gap = cfg.pane_gap;

        const float left_content  = left_cols_max * k + (left_cols_max - 1) * sp;
        const float right_content = cfg.show_special ? (right_cols * k + (right_cols - 1) * sp) : 0.0f;

        // + паддинги И бордеры внутренних панелей
        float inner = left_content + 2.0f * pad_x + 2.0f * child_b; // левая
        if (cfg.show_special)
            inner += gap + right_content + 2.0f * pad_x + 2.0f * child_b; // правая
        return inner;
    }


    // --- авто-высота для внутренней области (без внешних паддингов) ---
    inline float calc_min_inner_height(const VirtualKeyboardConfig& cfg,
                                       int left_rows, int right_rows) {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float key_h   = cfg.key_size.y;
        const float row_gap = style.ItemSpacing.y;     // вертикальные зазоры между рядами (как в реальном UI)
        const float pad_y   = style.WindowPadding.y;
        const float child_b = style.ChildBorderSize;   // бордер у внутренних панелей

        // Контент панелей (без паддингов/бордеров)
        const float left_content_h  = left_rows  * key_h + (left_rows  - 1) * row_gap;
        const float right_content_h = cfg.show_special ? (right_rows * key_h + (right_rows - 1) * row_gap) : 0.0f;

        // Верхняя зона: максимум из панелей + их внутренние паддинги + их бордеры
        const float top_h = std::max(left_content_h, right_content_h)
                          + 2.0f * pad_y
                          + 2.0f * child_b;

        // Нижняя зона: такой же отступ + высота ряда = max(кнопки, combo локали)
        float bottom_h = 0.0f;
        if (cfg.show_bottom || (cfg.show_locale_combo && cfg.locale_bottom_right)) {
            const float frame_h = ImGui::GetFrameHeight(); // реальная высота Combo
            const float btn_h   = cfg.show_bottom ? key_h : 0.0f;
            const float combo_h = (cfg.show_locale_combo && cfg.locale_bottom_right) ? frame_h : 0.0f;
            const float row_h   = (btn_h > combo_h ? btn_h : combo_h);
            bottom_h = row_gap + row_h;
        }

        return top_h + bottom_h;
    }


    // -------------- Widget ----------------------

    bool VirtualKeyboard(
            const char* id,
            std::string& text,
            VirtualKeyboardConfig cfg
        );

    // Back-compat overload
inline bool VirtualKeyboard(const char* id, std::string& text, ImVec2 size) {
        VirtualKeyboardConfig cfg;
        cfg.size = size;
        return VirtualKeyboard(id, text, cfg);
    }

} // namespace ImGuiX::Widgets

#ifdef IMGUIX_HEADER_ONLY
#   include "virtual_keyboard.ipp"
#endif

#endif // _IMGUIX_WIDGETS_VIRTUAL_KEYBOARD_HPP_INCLUDED
