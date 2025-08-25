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
#include "icon_button.hpp"

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

        bool submit_on_enter     = false;      ///< true: Enter submits (like InputTextEnterReturnsTrue)
        bool capture_hw_enter    = false;      ///< react to physical Enter when VK is focused
        bool shift_enter_newline = false;      ///< Shift+Enter inserts '\n' instead of submit
        std::function<void(const std::string&)> on_submit; ///< callback on submit(text)
    
        // Иконки/шрифт
        ImFont* icon_font = nullptr;  ///< шрифт иконок (например, MaterialIcons); null => использовать текущий
        ImGuiX::Widgets::IconButtonConfig icon_btn{}; ///< стиль для IconButtonCentered
        std::unordered_map<std::string, const char*> icons {
            /*
            {"Shift",   u8"\uE8E7"}, // примеры-заглушки: заменишь на свои PUA
            {"Space",   u8"\uE5CA"},
            {"Bksp",    u8"\uE14A"},
            {"Enter",   u8"\uE5B0"},
            {"Symbols", u8"\uE8B8"},
            {"Accents", u8"\uE23E"},
            */
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
        float   locale_width         = 56.0f;  ///< ширина превью "en"/"ru" (компактно)
        
        float   left_ratio           = 0.60f;  /// < Доли ширины панелей при заданной ширине (size.x>0)
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

    inline bool VirtualKeyboard(
            const char* id, 
            std::string& text, 
            const VirtualKeyboardConfig& cfg,
            VirtualKeyboardState* state = nullptr) {
        bool modified = false;

        ImGui::PushID(id);
        ImVec2 spacing = (cfg.spacing > 0.0f) ? ImVec2(cfg.spacing, cfg.spacing)
                                      : ImGui::GetStyle().ItemSpacing; // keep style
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, spacing);

        const ImGuiID key_vk_locale = ImGui::GetID(u8"vk_locale");
        const ImGuiID key_vk_shift_state = ImGui::GetID(u8"vk_shift_state");
        const ImGuiID key_vk_pane_mode   = ImGui::GetID(u8"vk_pane_mode");
        ImGuiStorage* st = ImGui::GetStateStorage();

        if (state) state->submitted = false;
        bool did_submit_this_frame = false;
        
        // helper to trigger submit once
        auto do_submit = [&](){
            if (did_submit_this_frame) return;         // prevent duplicates in the same frame
            did_submit_this_frame = true;
            if (state) state->submitted = true;
            if (cfg.on_submit) cfg.on_submit(text);    // user callback
        };
        
        auto get_locale = [&](){
            if (state) return std::clamp(state->locale_index, 0, (int)cfg.locales.size() - 1);
            int locale_index = st->GetInt(key_vk_locale, std::clamp(cfg.start_locale_index, 0, (int)cfg.locales.size() - 1));
            return std::clamp(locale_index, 0, (int)cfg.locales.size() - 1);
        };
        
        auto set_locale = [&](int i){
            if (state) state->locale_index = i;
            else st->SetInt(key_vk_locale, i);
        };


        auto get_shift = [&](){
            if (state) return state->shift;
            return st->GetInt(key_vk_shift_state, 0) != 0;
        };
        
        auto set_shift = [&](bool v){
            if (state) state->shift = v;
            else st->SetInt(key_vk_shift_state, v ? 1 : 0);
        };
        

        auto get_pane = [&](){
            if (state) return state->pane;
            int v = cfg.remember_pane ? st->GetInt(key_vk_pane_mode, 0) : 0;
            return v == 1 ? VKSpecialPane::Accents : VKSpecialPane::Symbols;
        };
        
        auto set_pane = [&](VKSpecialPane p){
            if (state) state->pane = p;
            else if (cfg.remember_pane) st->SetInt(key_vk_pane_mode, p==VKSpecialPane::Accents ? 1:0);
        };

        // --- раскладка / shift состояние ---
        int locale_index = get_locale();
        const char* locale_name = cfg.locales.empty() ? u8"en" : cfg.locales[locale_index];
        const KBLayout& L = get_layout(locale_name);
        bool shift = get_shift();

        const ImGuiStyle& style = ImGui::GetStyle();
        const float pad_x = style.WindowPadding.x;
        const float pad_y = style.WindowPadding.y;
        const float child_b = style.ChildBorderSize;
        const float row_gap  = style.ItemSpacing.y;

        // --- авто ширина/высота (с учётом паддингов Child) ---
        // внутренние размеры контента:
        const int left_cols_max = std::max(10, std::max(L.row2_n, std::max(L.row3_n, L.row4_n)));
        const int right_cols    = 6;
        const int left_rows     = (cfg.show_digits ? 4 : 3);
        const int right_rows    = 5;
        const float preview_h = cfg.show_top_preview
            ? ((cfg.preview_height > 0.0f) ? cfg.preview_height : ImGui::GetFrameHeight() + 2.0f * style.FramePadding.y)
            : 0.0f;

        float inner_w = (cfg.size.x > 0.0f)
            ? (cfg.size.x - style.WindowPadding.x * 2.0f)
            : calc_min_inner_width(cfg, left_cols_max, right_cols);

        float inner_h = (cfg.size.y > 0.0f)
            ? (cfg.size.y - style.WindowPadding.y * 2.0f)
            : calc_min_inner_height(cfg, left_rows, right_rows);
            
        if (cfg.size.y <= 0.0f && cfg.show_top_preview)
            inner_h += row_gap + preview_h;

        // внешние размеры Child
        float child_w = inner_w + 2.0f * pad_x + (cfg.border ? 2.0f * child_b : 0.0f);
        float child_h = inner_h + 2.0f * pad_y + (cfg.border ? 2.0f * child_b : 0.0f);
        child_h += cfg.height_bias_px;
        child_h += 2.0f * pad_y;

        ImGui::BeginChild(u8"##VirtualKeyboard", ImVec2(child_w, child_h), cfg.border, ImGuiWindowFlags_NoScrollbar);
        const float base_x = ImGui::GetCursorPosX();

        // 
        bool is_valid = true; // default ok
        if (cfg.validate_with_regex && cfg.validate_regex) {
            try {
                std::regex re(cfg.validate_regex);
                is_valid = std::regex_match(text, re);
            } catch (...) {
                is_valid = true; // invalid regex => do not block
            }
        }

        if (cfg.show_top_preview) {
            // background (Child) + colored text inside
            ImGui::PushStyleColor(ImGuiCol_ChildBg, cfg.preview_bg_col);
            ImGui::BeginChild(u8"##vk_preview",
                ImVec2(inner_w, preview_h),
                cfg.preview_border,
                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::PushStyleColor(ImGuiCol_Text, is_valid ? cfg.preview_text_col : cfg.preview_text_invalid);
            // Single-line display; clip if long
            ImGui::TextUnformatted(text.c_str());
            ImGui::PopStyleColor();

            ImGui::EndChild();
            ImGui::PopStyleColor();

            // spacing between preview and panels
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetStyle().ItemSpacing.y));
        }

        auto press = [&](const char* label, bool ascii_single = false) {
            if (std::strcmp(label, u8"Bksp") == 0) {
                if (!text.empty()) { 
                    text.pop_back(); 
                    modified = true; 
                }
                return;
            }
            if (std::strcmp(label, u8"Shift") == 0) {
                shift = !shift; 
                set_shift(shift);
                return;
            }
            if (std::strcmp(label, u8"Space") == 0) { 
                text.push_back(' '); 
                modified = true; 
                return; 
            }
            if (std::strcmp(label, u8"Enter") == 0) { 
                if (cfg.submit_on_enter) {
                    // если нужно — поддержать Shift+Enter = перенос строки
                    if (cfg.shift_enter_newline && 
                        shift && 
                        cfg.allow_enter) {
                        text.push_back('\n'); 
                        modified = true;
                    } else {
                        do_submit();
                    }
                } else {
                    if (cfg.allow_enter) { 
                        text.push_back('\n'); 
                        modified = true; 
                    }
                }
                return; 
            }
            if (ascii_single) append_char_ascii(text, label, shift, modified);
            else append_utf8(text, label, modified);
        };

        // --- верхняя область: левая/правая панели ---
        ImVec2 key_size = cfg.key_size;
        if (key_size.x <= 0.0f || key_size.y <= 0.0f) {
            const float h = ImGui::GetFrameHeight(); // respects font & global scale
            key_size = ImVec2(h, h);                 // square keys by default
        }

        const float frame_h  = ImGui::GetFrameHeight();
        const float btn_h    = cfg.show_bottom ? key_size.y : 0.0f;
        const float combo_h  = (cfg.show_locale_combo && cfg.locale_bottom_right) ? frame_h : 0.0f;
        const float row_h    = (btn_h > combo_h ? btn_h : combo_h);
        const float bottom_h_used = (cfg.show_bottom || (cfg.show_locale_combo && cfg.locale_bottom_right))
                                    ? (row_gap + row_h) : 0.0f;

        const float top_h = inner_h - bottom_h_used - (cfg.show_top_preview ? (row_gap + preview_h) : 0.0f);

        // ширины панелей по КОНТЕНТУ:
        float right_content_w = 0.0f;
        if (cfg.show_special)
            right_content_w = right_cols * key_size.x + (right_cols - 1) * cfg.spacing;

        // учтём паддинги каждого саб-чилда:
        float right_w_outer = cfg.show_special ? (right_content_w + 2.0f * pad_x + 2.0f * child_b) : 0.0f;
        float left_w_outer  = inner_w - right_w_outer - (cfg.show_special ? cfg.pane_gap : 0.0f);
        const float right_anchor_offset = left_w_outer + (cfg.show_special ? cfg.pane_gap : 0.0f);

        // Панель слева
        ImGui::BeginChild(u8"##vk_left", ImVec2(left_w_outer, top_h), true, ImGuiWindowFlags_NoScrollbar);
        {
            if (cfg.show_digits) {
                const char* row1[] = {u8"1",u8"2",u8"3",u8"4",u8"5",u8"6",u8"7",u8"8",u8"9",u8"0"};
                for (int i = 0; i < 10; ++i) {
                    if (ImGui::Button(row1[i], key_size)) press(row1[i], /*ascii=*/true);
                    if (i < 9) ImGui::SameLine();
                }
            }

            auto draw_row = [&](const char* const* row, int n, bool ascii) {
                for (int i = 0; i < n; ++i) {
                    const char* lbl = row[i];
                    if (ascii && shift && std::isalpha((unsigned char)lbl[0])) {
                        char up[2] = { (char)std::toupper((unsigned char)lbl[0]), 0 };
                        if (ImGui::Button(up, key_size)) press(up, /*ascii=*/true);
                    } else {
                        if (ImGui::Button(lbl, key_size)) {
                            if (ascii) press(lbl, /*ascii=*/true);
                            else       press(lbl, /*ascii=*/false);
                        }
                    }
                    if (i < (n - 1)) ImGui::SameLine();
                }
            };

            bool ascii_letters = (std::strcmp(L.name, u8"en") == 0);

            draw_row(shift && !ascii_letters ? L.row2U : L.row2, L.row2_n, ascii_letters);
            draw_row(shift && !ascii_letters ? L.row3U : L.row3, L.row3_n, ascii_letters);
            draw_row(shift && !ascii_letters ? L.row4U : L.row4, L.row4_n, ascii_letters);
        }
        ImGui::EndChild();

        // Панель справа
        if (cfg.show_special) {
            ImGui::SameLine(0.0f, cfg.pane_gap);
            ImGui::BeginChild(u8"##vk_right", ImVec2(right_w_outer, top_h), true, ImGuiWindowFlags_NoScrollbar);
            {
                // --- small mode switch (top)
                VKSpecialPane pane = get_pane();
                bool has_accents = cfg.show_accents && (get_accents_for_locale(locale_name).count > 0);

                if (pane == VKSpecialPane::Accents && has_accents) {
                    // --- ACCENTS GRID ---
                    AccentsSet acc = get_accents_for_locale(locale_name);
                    constexpr int cols = 6; // compact grid
                    int idx = 0;
                    for (int i = 0; i < acc.count; ++i) {
                        const char* s = acc.items[i];
                        if (ImGui::Button(s, key_size)) press(s, /*ascii=*/false);
                        ++idx;
                        if ((idx % cols) != 0 && i < acc.count - 1)
                            ImGui::SameLine();
                    }
                } else {
                    // --- SYMBOLS GRID (as before) ---
                    const char* spec[] = {
                        u8"!",u8"@",u8"#",u8"$",u8"%",u8"^",
                        u8"&",u8"*",u8"(",u8")",u8"-",u8"_",
                        u8"+",u8"=",u8"{",u8"}",u8"[",u8"]",
                        u8"|",u8"\\",u8":",u8";",u8"'",u8"\"",
                        u8"<",u8">",u8",",u8".",u8"?",u8"/"
                    };
                    constexpr int cols = 6, rows = 5;
                    int idx = 0;
                    for (int r = 0; r < rows; ++r) {
                        for (int c = 0; c < cols; ++c, ++idx) {
                            if (ImGui::Button(spec[idx], key_size)) press(spec[idx], /*ascii=*/true);
                            if (c < cols - 1) ImGui::SameLine();
                        }
                    }
                }
            }
            ImGui::EndChild();
        }

        // реакция на ФИЗИЧЕСКИЙ Enter
        if (cfg.submit_on_enter && cfg.capture_hw_enter) {
            const bool wnd_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);
            if (wnd_focused) {
                const bool key_enter  = ImGui::IsKeyPressed(ImGuiKey_Enter, false);
                const bool key_kp_ent = ImGui::IsKeyPressed(ImGuiKey_KeypadEnter, false);
                if (key_enter || key_kp_ent) {
                    if (cfg.shift_enter_newline && (ImGui::GetIO().KeyShift || get_shift())) {
                        if (cfg.allow_enter) { 
                            text.push_back('\n'); 
                            modified = true; 
                        }
                    } else {
                        do_submit();
                    }
                }
            }
        }

        // Универсальный рендер кнопки (иконка или текст)
        auto ButtonOrIcon = [&](const char* label, const ImVec2& size, bool ascii_single = false) -> bool {
            // если иконка настроена — рисуем иконку
            auto it = cfg.icons.find(label);
            if (it != cfg.icons.end() && it->second && it->second[0] != '\0') {
                // используем скопию конфигура: подменим шрифт при необходимости
                ImGuiX::Widgets::IconButtonConfig ib = cfg.icon_btn;
                if (cfg.icon_font) ib.font = cfg.icon_font;
                // если шрифт не задан, предполагаем, что иконки смержены в текущий
                return ImGuiX::Widgets::IconButtonCentered(label, it->second, ib);
            }
            // обычная кнопка
            return ImGui::Button(label, size);
        };

        // --- нижняя строка + селектор локали (справа) ---
        if (cfg.show_bottom || 
            (cfg.show_locale_combo && cfg.locale_bottom_right) || 
            (cfg.show_accents && cfg.show_special)) {
            ImGui::Dummy(ImVec2(0.0f, style.ItemSpacing.y));
            
            const float line_start_x = ImGui::GetCursorPosX();

            if (cfg.show_bottom) {
                const ImVec2 wide(key_size.x * 1.5f, key_size.y);
                const ImVec2 space_size(key_size.x * 4,   key_size.y);
                if (ButtonOrIcon(u8"Shift", wide)) press(u8"Shift");
                ImGui::SameLine();
                if (ButtonOrIcon(u8"Space", space_size)) press(u8"Space");
                ImGui::SameLine();
                if (ButtonOrIcon(u8"Bksp", wide)) press(u8"Bksp");
                if (cfg.allow_enter) { 
                    ImGui::SameLine(); 
                    if (ButtonOrIcon(u8"Enter", wide)) { 
                        press(u8"Enter"); 
                    }
                }
            }
            
            // по началу правой части — переключатель режимов
            if (cfg.show_accents && cfg.show_special) {
                bool has_accents = get_accents_for_locale(locale_name).count > 0;
                
                if (has_accents) {
                    float offset = right_anchor_offset;                    // смещение от начала строки
                    if (offset < 0.0f) offset = 0.0f;                      // на всякий случай
                    ImGui::SameLine(offset, style.ItemSpacing.x);          // позиционируем "по X от начала"
                    const char* label = (get_pane() == VKSpecialPane::Symbols) ? u8"Accents" : u8"Symbols";
                    const float btn_w = 64.0f;
                    if (ButtonOrIcon(label, ImVec2(btn_w, 0))) {
                        set_pane(get_pane() == VKSpecialPane::Symbols ? 
                            VKSpecialPane::Accents : 
                            VKSpecialPane::Symbols);
                    }
                }
                
                float offset = right_anchor_offset;                    // смещение от начала строки
                if (offset < 0.0f) offset = 0.0f;                      // на всякий случай
                ImGui::SameLine(offset, style.ItemSpacing.x);          // позиционируем "по X от начала"
            }

            // комбо локали справа
            if (cfg.show_locale_combo && cfg.locale_bottom_right && !cfg.locales.empty()) {
                // прижать вправо
                float w = std::max(44.0f, cfg.locale_width);
                float x = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - w;
                ImGui::SameLine();
                ImGui::SetCursorPosX(x);
                ImGui::SetNextItemWidth(w);
                const char* cur = cfg.locales[locale_index];
                if (ImGui::BeginCombo(u8"##vk_locale_br", cur, ImGuiComboFlags_HeightSmall)) {
                    for (int i = 0; i < (int)cfg.locales.size(); ++i) {
                        bool sel = (i == locale_index);
                        if (ImGui::Selectable(cfg.locales[i], sel)) {
                            set_locale(i);
                            locale_index = i;
                        }
                        if (sel) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
        } else 
        if (cfg.show_locale_combo && !cfg.locale_bottom_right && !cfg.locales.empty()) {
            // старое размещение сверху (компактно)
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() - (cfg.spacing + key_size.y)); // поднять к верху
            ImGui::SetNextItemWidth(std::max(44.0f, cfg.locale_width));
            const char* cur = cfg.locales[locale_index];
            if (ImGui::BeginCombo(u8"##vk_locale_top", cur, ImGuiComboFlags_HeightSmall)) {
                for (int i = 0; i < (int)cfg.locales.size(); ++i) {
                    bool sel = (i == locale_index);
                    if (ImGui::Selectable(cfg.locales[i], sel)) {
                        set_locale(i);
                        locale_index = i;
                    }
                    if (sel) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }

        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopID();

        return modified;
    }

    // Back-compat overload
    inline bool VirtualKeyboard(const char* id, std::string& text, ImVec2 size) {
        VirtualKeyboardConfig cfg;
        cfg.size = size;
        return VirtualKeyboard(id, text, cfg);
    }

} // namespace ImGuiX::Widgets

#endif // _IMGUIX_WIDGETS_VIRTUAL_KEYBOARD_HPP_INCLUDED
