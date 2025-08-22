#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>

#ifdef IMGUIX_FONTS_ENABLE_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef IMGUI_ENABLE_FREETYPE
#include <imgui_freetype.h>
#endif

namespace ImGuiX::Fonts {
namespace fs = std::filesystem;

    /// ----------------------------
    /// Inline trivial setters/getters
    /// ----------------------------

    inline void FontManager::setConfigPath(std::string path) {
      m_config_path = std::move(path);
    }

    inline void FontManager::setBaseDir(std::string base_dir) {
      m_params.base_dir = std::move(base_dir);
      markDirty();
    }

    inline void FontManager::setLocale(std::string locale) {
        if (locale == m_active_locale) return;
        m_active_locale = std::move(locale);
        markDirty();
    }
    
    inline void FontManager::setRanges(std::string preset) {
        m_manual.ranges_preset = std::move(preset);
        m_manual.ranges.clear();
        markDirty();
    }

    inline void FontManager::setRanges(const std::vector<ImWchar>& pairs) {
        m_manual.ranges_preset.clear();
        m_manual.ranges = pairs;
        markDirty();
    }

    inline void FontManager::clearRanges() {
        m_manual.ranges_preset.clear();
        m_manual.ranges.clear();
        markDirty();
    }

    inline void FontManager::setMarkdownSizes(
            float body_px, 
            float h1_px,
            float h2_px, 
            float h3_px
        ) {
        m_px_body = body_px;
        m_px_h1 = h1_px;
        m_px_h2 = h2_px;
        m_px_h3 = h3_px;
        markDirty();
    }

    inline void FontManager::setLocalePack(const LocalePack &pack) {
      m_packs[pack.locale] = pack;
      // Not marking dirty automatically; active locale may be different.
    }

    inline void FontManager::clearPacks() {
      m_packs.clear();
      markDirty();
    }

    inline void FontManager::beginManual() {
        m_manual = {};
        m_manual.active = true;
        markDirty();
    }

    inline void FontManager::addFontBody(const FontFile &ff) {
      m_manual.has_body = true;
      m_manual.body = ff;
    }

    inline void FontManager::addFontHeadline(FontRole role_h, const FontFile &ff) {
      m_manual.headlines[role_h] = ff;
    }

    inline void FontManager::addFontMerge(FontRole role, const FontFile &ff) {
      if (!m_manual.active) {
        m_manual.active = true;
      } // allow call without beginManual()
      if (role == FontRole::Icons) {
        m_manual.merges_icons.push_back(ff);
      } else if (role == FontRole::Emoji) {
        m_manual.merges_emoji.push_back(ff);
      } else {
        // ignore invalid roles to keep invariants
        return;
      }
      markDirty();
    }

    inline void FontManager::addFontMerge(const FontFile &ff) {
      if (!m_manual.active) {
        m_manual.active = true;
      }
      m_manual.merges_unknown.push_back(ff);
      markDirty();
    }

    inline void FontManager::markDirty() { m_dirty = true; }

    inline ImFont *FontManager::getFont(FontRole role) const {
      auto it = m_fonts.find(role);
      return (it != m_fonts.end()) ? it->second : nullptr;
    }

    inline const std::string &FontManager::activeLocale() const {
      return m_active_locale;
    }
    inline const BuildParams &FontManager::params() const { return m_params; }

    /// ----------------------------
    /// Heavy helpers (implementation)
    /// ----------------------------

    /// \brief Compute effective pixel size (px @ runtime) given base size at 96
    /// DPI.
    inline float FontManager::scalePx(float px_96, const BuildParams &p) {
      return px_96 * (p.dpi / 96.0f) * p.ui_scale;
    }

    /// \brief Add standard ranges based on locale id.
    /// \note Keep conservative defaults to avoid huge atlases.
    inline void FontManager::addLocaleRanges(
            ImFontGlyphRangesBuilder &b,
            ImGuiIO &io,
            const std::string &locale
        ) {
        // Always include default UI symbols
        b.AddRanges(io.Fonts->GetGlyphRangesDefault());

        // Common add-ons: punctuation seen in many UIs
        b.AddText(u8"–—…•“”‘’"); // en/em dashes, ellipsis, bullets, quotes

        // Latin-based locales already covered by Default()
        // Add Cyrillic
        if (locale == "ru" || 
            locale == "uk" ||
            locale == "bg" || 
            locale == "kk" ||
            locale == "sr") {
            b.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
        }

        // Vietnamese
        if (locale == "vi") b.AddRanges(io.Fonts->GetGlyphRangesVietnamese());

        // CJK (careful: large)
        if (locale == "ja")
            b.AddRanges(io.Fonts->GetGlyphRangesJapanese());
        else if (locale == "zh" || locale == "zh-CN" || locale == "zh-TW")
            b.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
        else if (locale == "ko")
            b.AddRanges(io.Fonts->GetGlyphRangesKorean());

        // RTL (Arabic/Hebrew) – ImGui does not provide built-ins for all subsets;
        // user usually supplies proper ranges via JSON LocalePack::ranges or custom
        // font files.
        if (locale == "ar" || locale == "fa" || locale == "he") {
            // Leave to JSON-provided ranges; if absent, fallback stays Latin-only.
            // You may add custom ranges here if you ship appropriate fonts.
        }
    }

    inline void FontManager::addNamedRanges(ImFontGlyphRangesBuilder &b,
                                            ImGuiIO &io, const std::string &spec) {
      auto split = [](const std::string &s) {
        std::vector<std::string> out;
        std::string cur;
        for (char c : s) {
          if (c == '+') {
            if (!cur.empty())
              out.push_back(cur), cur.clear();
          } else
            cur.push_back(c);
        }
        if (!cur.empty())
          out.push_back(cur);
        return out;
      };
      for (auto tok : split(spec)) {
        if (tok == "Default")
          b.AddRanges(io.Fonts->GetGlyphRangesDefault());
        else if (tok == "Cyrillic")
          b.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
        else if (tok == "Vietnamese")
          b.AddRanges(io.Fonts->GetGlyphRangesVietnamese());
        else if (tok == "Japanese" || tok == "JapaneseFull")
          b.AddRanges(io.Fonts->GetGlyphRangesJapanese());
        else if (tok == "Chinese" || tok == "ChineseFull")
          b.AddRanges(io.Fonts->GetGlyphRangesChineseFull());
        else if (tok == "Korean")
          b.AddRanges(io.Fonts->GetGlyphRangesKorean());
        else if (tok == "Punct")
          b.AddText(u8"–—…•“”‘’");
        // Latin, Greek, Thai, etc. as needed
      }
    }

    /// \brief Build glyph ranges from LocalePack + extra glyphs from all FontFiles.
    /// \details Combines named presets (ranges_preset), explicit pairs (ranges),
    ///          locale-based defaults, and per-file extra_glyphs (UTF-8).
    inline void FontManager::buildRangesFromPack(std::vector<ImWchar> &out,
                                                 const LocalePack *pack,
                                                 const std::string &active_locale) {
      ImGuiIO &io = ImGui::GetIO();
      ImFontGlyphRangesBuilder b;

      if (pack) {
        // 1) Named presets (e.g., "Default+Cyrillic+Vietnamese+Punct")
        if (!pack->ranges_preset.empty())
          addNamedRanges(b, io, pack->ranges_preset);

        // 2) Explicit [start,end] pairs (0-terminated). Ensure terminator.
        if (!pack->ranges.empty()) {
          if (pack->ranges.back() == 0) {
            b.AddRanges(pack->ranges.data());
          } else {
            std::vector<ImWchar> tmp = pack->ranges;
            tmp.push_back(0);
            b.AddRanges(tmp.data());
          }
        }

        // 3) If neither presets nor explicit ranges given, fall back to locale
        // defaults.
        if (pack->ranges_preset.empty() && pack->ranges.empty())
          addLocaleRanges(b, io, active_locale);

        // 4) Common punctuation (harmless to add twice; builder deduplicates).
        b.AddText(u8"–—…•“”‘’");

        // 5) Per-file extra glyphs (UTF-8) — union across all roles/files.
        for (const auto &kv : pack->roles)
          for (const auto &ff : kv.second)
            if (!ff.extra_glyphs.empty())
              addExtraGlyphs(b, ff.extra_glyphs);
      } else {
        // No pack: locale-based defaults + punctuation.
        addLocaleRanges(b, io, active_locale);
        b.AddText(u8"–—…•“”‘’");
      }

      ImVector<ImWchar> built;
      b.BuildRanges(&built);
      out.assign(built.begin(), built.end());
    }

    /// \brief Add a font file to ImGui atlas (respecting merge flag, size scaling
    /// and ranges).
    inline ImFont *FontManager::addFontFile(
            const FontFile &ff,
            const BuildParams &params,
            const std::vector<ImWchar> &ranges,
            const std::string &base_dir_abs,
            const ImFontConfig &base_cfg
        ) {
        ImFontConfig cfg = base_cfg;
        cfg.MergeMode = cfg.MergeMode || ff.merge;

        const float px = (ff.size_px > 0.0f ? ff.size_px : 16.0f);
        const float eff_px = scalePx(px, params);

        fs::path p = fs::u8path(ff.path);
        fs::path resolved_p = p.is_absolute() ? p : (fs::u8path(base_dir_abs) / p);
        const std::string resolved = resolved_p.lexically_normal().u8string();

        return ImGui::GetIO().Fonts->AddFontFromFileTTF(
            resolved.c_str(), eff_px, &cfg, ranges.empty() ? 
                nullptr : 
                ranges.data()
        );
    }

    /// \brief Switch FreeType builder for ImGui if requested and available.
    inline void FontManager::setupFreetypeIfNeeded(const BuildParams &params) {
#   ifdef IMGUI_ENABLE_FREETYPE
        if (!params.use_freetype) return;
#       if IMGUI_VERSION_NUM >= 19200
        ImGui::GetIO().Fonts->FontLoader = ImGuiFreeType::GetFontLoader();
        ImGui::GetIO().Fonts->FontLoaderFlags = 0;
#       else
        ImGui::GetIO().Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
        ImGui::GetIO().Fonts->FontBuilderFlags = 0;
#       endif
#   else
        (void)params;
#   endif
    }

    /// \brief After Fonts->Build(), update backend texture for SFML.
    inline bool FontManager::updateBackendTexture() {
#       ifdef IMGUIX_USE_SFML_BACKEND
        // ImGui-SFML provides UpdateFontTexture(); some versions return void.
        return ImGui::SFML::UpdateFontTexture();
#       else
        // If not using SFML backend, user should replace this with their backend
        // call. We still return true to not fail builds on other backends.
        return true;
#       endif
    }

    /// ----------------------------
    /// Public methods implementation
    /// ----------------------------

    inline bool FontManager::bootstrap(const BuildParams &params) {
      m_params = params;
      m_dirty = true;
      return true;
    }

    inline BuildResult FontManager::buildNow() {
        BuildResult br{};

        std::string base_dir_abs;
#       ifdef __EMSCRIPTEN__
        base_dir_abs = m_params.base_dir; // as is
#       else
        base_dir_abs = ImGuiX::Utils::resolveExecPath(m_params.base_dir);
#       endif

        // We support either manual buffer (if active) or the active locale pack.
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->Clear();
        io.FontDefault = nullptr;

        setupFreetypeIfNeeded(m_params);
        
        // 1) Select pack_ptr for active locale (if not manual)
        const LocalePack* pack_ptr = nullptr;
        if (!m_manual.active) {
            auto it = m_packs.find(m_active_locale);
            if (it == m_packs.end()) {
                auto it2 = m_packs.find("default");
                if (it2 != m_packs.end()) pack_ptr = &it2->second;
            } else {
                pack_ptr = &it->second;
            }
        }
        
        // 2) Collect FreeType flags from all FontFile
        unsigned int ft_flags = 0;
        if (!m_manual.active) {
            const LocalePack* p = pack_ptr;
            if (p) {
                for (const auto& role_vec : p->roles) {
                    for (const auto& ff : role_vec.second) {
                        ft_flags |= ff.freetype_flags;
                    }
                }
            }
        } else {
            if (m_manual.has_body) ft_flags |= m_manual.body.freetype_flags;
            for (const auto& kv : m_manual.headlines) ft_flags |= kv.second.freetype_flags;
            for (const auto& ff : m_manual.merges_icons)   ft_flags |= ff.freetype_flags;
            for (const auto& ff : m_manual.merges_emoji)   ft_flags |= ff.freetype_flags;
            for (const auto& ff : m_manual.merges_unknown) ft_flags |= ff.freetype_flags;
        }

        // 3) Apply flags to atlas BEFORE AddFontFromFileTTF
#   ifdef IMGUI_ENABLE_FREETYPE
#       if IMGUI_VERSION_NUM >= 19200
        ImGui::GetIO().Fonts->FontLoaderFlags = ft_flags;
#       else
        ImGui::GetIO().Fonts->FontBuilderFlags = ft_flags;
#       endif
#   endif

        // Build ranges
        // 4) Gather ranges
        std::vector<ImWchar> ranges;
        if (!m_manual.active) {
            buildRangesFromPack(ranges, pack_ptr, m_active_locale);
        } else {
            // Manual ranges: use default builder with extra glyphs from manual entries
            ImFontGlyphRangesBuilder b;

            // 1) Priority: preset → explicit pairs → locale fallback
            if (!m_manual.ranges_preset.empty()) {
                addNamedRanges(b, io, m_manual.ranges_preset);
            } else 
            if (!m_manual.ranges.empty()) {
                if (m_manual.ranges.back() == 0) {
                    b.AddRanges(m_manual.ranges.data());
                } else {
                    auto tmp = m_manual.ranges;
                    tmp.push_back(0);
                    b.AddRanges(tmp.data());
                }
            } else {
                addLocaleRanges(b, io, m_active_locale);
            }

            // 2) Useful punctuation (deduplicated inside builder)
            b.AddText(u8"–—…•“”‘’");

            // 3) Extra glyphs from all manual config sources
            if (m_manual.has_body) addExtraGlyphs(b, m_manual.body.extra_glyphs);
            for (const auto& kv : m_manual.headlines)      addExtraGlyphs(b, kv.second.extra_glyphs);
            for (const auto& ff : m_manual.merges_icons)   addExtraGlyphs(b, ff.extra_glyphs);
            for (const auto& ff : m_manual.merges_emoji)   addExtraGlyphs(b, ff.extra_glyphs);
            for (const auto& ff : m_manual.merges_unknown) addExtraGlyphs(b, ff.extra_glyphs);

            ImVector<ImWchar> r;
            b.BuildRanges(&r);
            ranges.assign(r.begin(), r.end());
        }

        // Base config for all fonts
        ImFontConfig cfg{};
        cfg.OversampleH = 3;
        cfg.OversampleV = 1;
        cfg.PixelSnapH = false;

        m_fonts.clear();

        // Add Body first (either from pack or from manual)
        ImFont *body = nullptr;

        auto add_role_vec = [&](FontRole role,
                              const std::vector<FontFile> &vec) -> ImFont * {
            ImFont *last = nullptr;
            for (size_t i = 0; i < vec.size(); ++i) {
                ImFontConfig local = cfg;
                // Merge mode depends on FontFile::merge; Body chain root usually
                // merge=false.
                local.MergeMode = (i > 0) ? 
                    (local.MergeMode || vec[i].merge) : 
                    vec[i].merge;
                ImFont *f = addFontFile(vec[i], m_params, ranges, base_dir_abs, local);
                if (!f) {
                    br.message = "Failed to load font: " + vec[i].path;
                }
                last = f;
            }
            if (last) m_fonts[role] = last;
            return last;
        };

        auto add_single = [&](FontRole role, const FontFile &ff) -> ImFont * {
            ImFont *f = addFontFile(ff, m_params, ranges, base_dir_abs, cfg);
            if (f) m_fonts[role] = f;
            else br.message = "Failed to load font: " + ff.path;
            return f;
        };

        // Strategy per mode
        if (!m_manual.active) {
        // --- PACK MODE ---
        // Body
        if (pack_ptr) {
          if (auto it = pack_ptr->roles.find(FontRole::Body);
              it != pack_ptr->roles.end() && !it->second.empty()) {
            body = add_role_vec(FontRole::Body, it->second);
          }
          // Merge Icons / Emoji into the last added (Body chain)
          if (auto it = pack_ptr->roles.find(FontRole::Icons);
              it != pack_ptr->roles.end()) {
            for (const auto &ff : it->second) {
              FontFile mff = ff;
              mff.merge = true;
              ImFont *f = addFontFile(mff, m_params, ranges, base_dir_abs, cfg);
              (void)f; // merged; no separate role pointer necessary
            }
            // record role as present (point to Body for retrieval semantics)
            if (body)
              m_fonts[FontRole::Icons] = body;
          }
          if (auto it = pack_ptr->roles.find(FontRole::Emoji);
              it != pack_ptr->roles.end()) {
            for (const auto &ff : it->second) {
              FontFile mff = ff;
              mff.merge = true;
              ImFont *f = addFontFile(mff, m_params, ranges, base_dir_abs, cfg);
              (void)f;
            }
            if (body)
              m_fonts[FontRole::Emoji] = body;
          }

          // Headings H1/H2/H3 (separate instances, may reuse Body TTF path with
          // different size)
          auto add_headline = [&](FontRole role, float px_default) -> ImFont * {
            const FontFile *chosen = nullptr;
            if (auto it = pack_ptr->roles.find(role);
                it != pack_ptr->roles.end() && !it->second.empty())
              chosen = &it->second.front();
            FontFile ff{};
            if (chosen) {
              ff = *chosen;
              if (ff.size_px <= 0.0f)
                ff.size_px = px_default;
            } else if (body) {
              // reuse first Body file path
              const auto &bodyv = pack_ptr->roles.at(FontRole::Body);
              ff = bodyv.front();
              ff.size_px = px_default;
            } else {
              return nullptr;
            }
            return add_single(role, ff);
          };

          add_headline(FontRole::H1, m_px_h1);
          add_headline(FontRole::H2, m_px_h2);
          add_headline(FontRole::H3, m_px_h3);

          // Bold/Italic/BoldItalic/Monospace
          auto add_optional_role = [&](FontRole role) -> ImFont * {
            if (auto it = pack_ptr->roles.find(role);
                it != pack_ptr->roles.end() && !it->second.empty())
              return add_single(role, it->second.front());
            return nullptr;
          };
          add_optional_role(FontRole::Bold);
          add_optional_role(FontRole::Italic);
          add_optional_role(FontRole::BoldItalic);
          add_optional_role(FontRole::Monospace);
        }

        // If still no Body, try a minimal fallback (Roboto + Icons) using base_dir
        if (!body) {
          FontFile fb{};
          fb.path = "Roboto-Medium.ttf";
          fb.size_px = m_px_body;
          body = add_single(FontRole::Body, fb);

          FontFile ic{};
          ic.path = "forkawesome-webfont.ttf";
          ic.size_px = m_px_body;
          ic.merge = true;
          addFontFile(ic, m_params, ranges, base_dir_abs, cfg);
          if (body)
            m_fonts[FontRole::Icons] = body;

          // Headings reuse Body path with different sizes
          FontFile h{};
          h.path = fb.path;
          h.size_px = m_px_h1;
          add_single(FontRole::H1, h);
          h.size_px = m_px_h2;
          add_single(FontRole::H2, h);
          h.size_px = m_px_h3;
          add_single(FontRole::H3, h);
        }
      } else {
        // --- MANUAL MODE ---
        if (m_manual.has_body) {
          // Body root
          body = add_single(FontRole::Body, m_manual.body);

          auto do_merge_vec = [&](const std::vector<FontFile> &vec) {
            for (auto ff : vec) {
              ff.merge = true;
              (void)addFontFile(ff, m_params, ranges, base_dir_abs, cfg);
            }
          };

          // Explicit roles
          bool merged_icons = !m_manual.merges_icons.empty();
          bool merged_emoji = !m_manual.merges_emoji.empty();
          do_merge_vec(m_manual.merges_icons);
          do_merge_vec(m_manual.merges_emoji);

          // Legacy: if old method used, include both roles
          if (!m_manual.merges_unknown.empty()) {
            do_merge_vec(m_manual.merges_unknown);
            merged_icons = true;
            merged_emoji = true;
          }

          if (body) {
            if (merged_icons)
              m_fonts[FontRole::Icons] = body;
            if (merged_emoji)
              m_fonts[FontRole::Emoji] = body;
          }

          // Headlines
          auto ensure_headline = [&](FontRole role, float px_default) {
            auto it = m_manual.headlines.find(role);
            if (it != m_manual.headlines.end()) {
              add_single(role, it->second);
            } else if (body) {
              // reuse body path as a convenience
              FontFile ff = m_manual.body;
              ff.size_px = px_default;
              add_single(role, ff);
            }
          };
          ensure_headline(FontRole::H1, m_px_h1);
          ensure_headline(FontRole::H2, m_px_h2);
          ensure_headline(FontRole::H3, m_px_h3);
        } else {
          br.message = "Manual mode: Body font not provided";
        }
      }

        // Update backend texture (SFML or stub true)
        if (!updateBackendTexture()) {
            br.success = false;
            br.message = "Backend font texture update failed";
            return br;
        }

        // Default font
        if (ImFont *f = getFont(FontRole::Body)) {
            io.FontDefault = f;
        }

        // Done
        br.success = true;
        br.fonts = m_fonts;
        m_dirty = false;
        return br;
    }

    inline BuildResult FontManager::rebuildIfNeeded() {
        if (!m_dirty) {
            BuildResult ok{};
            ok.success = true;
            ok.fonts = m_fonts;
            return ok;
        }
        return buildNow();
    }

    inline BuildResult FontManager::initFromJsonOrDefaults() {
        using nlohmann::json;
#   ifdef IMGUIX_FONTS_ENABLE_JSON
        BuildResult br{};

        // Load JSON text (if exists)
        std::string cfg_path;
#       ifdef __EMSCRIPTEN__
        cfg_path = m_config_path; // as is
#       else
        cfg_path = ImGuiX::Utils::resolveExecPath(m_config_path);
        // Fallback: if empty or not found, try base_dir/fonts.json
        if (readTextFile(cfg_path).empty()) {
            const auto base_abs = ImGuiX::Utils::resolveExecPath(m_params.base_dir);
            cfg_path = ImGuiX::Utils::joinPaths(base_abs, "fonts.json");
        }
#       endif

      const std::string json_text = readTextFile(cfg_path);

      if (!json_text.empty()) {
        json j;
        try {
          j = json::parse(json_text);

          // base_dir (optional)
          if (j.contains("base_dir") && j["base_dir"].is_string())
            m_params.base_dir = j["base_dir"].get<std::string>();

          // markdown_sizes (optional)
          if (j.contains("markdown_sizes") && j["markdown_sizes"].is_object()) {
            const auto &ms = j["markdown_sizes"];
            if (ms.contains("body"))
              m_px_body = ms["body"].get<float>();
            if (ms.contains("h1"))
              m_px_h1 = ms["h1"].get<float>();
            if (ms.contains("h2"))
              m_px_h2 = ms["h2"].get<float>();
            if (ms.contains("h3"))
              m_px_h3 = ms["h3"].get<float>();
          }

          // locales (packs)
          if (j.contains("locales") && j["locales"].is_object()) {
            for (auto it = j["locales"].begin(); it != j["locales"].end(); ++it) {
              const std::string loc = it.key();
              const json &L = it.value();

              LocalePack pack{};
              pack.locale = loc;

              if (L.contains("inherits") && L["inherits"].is_string())
                pack.inherits = L["inherits"].get<std::string>();

              // ranges: array of pairs OR string preset (e.g.,
              // "Default+Cyrillic+Punct")
              if (L.contains("ranges")) {
                if (L["ranges"].is_array()) {
                  for (const auto &v : L["ranges"])
                    pack.ranges.push_back(static_cast<ImWchar>(v.get<int>()));
                  // Append 0 terminator later inside buildRangesFromPack()
                } else if (L["ranges"].is_string()) {
                  pack.ranges_preset =
                      L["ranges"].get<std::string>(); // <-- IMPORTANT: keep
                }
              }

              // roles...
              if (L.contains("roles") && L["roles"].is_object()) {
                const auto &R = L["roles"];
                auto parse_files = [&](const char *key, FontRole role) {
                  if (R.contains(key) && R[key].is_array()) {
                    for (const auto &item : R[key]) {
                      FontFile ff{};
                      if (item.contains("path"))
                        ff.path = item["path"].get<std::string>();
                      if (item.contains("size_px"))
                        ff.size_px = item["size_px"].get<float>();
                      if (item.contains("merge"))
                        ff.merge = item["merge"].get<bool>();
                      if (item.contains("freetype_flags"))
                        ff.freetype_flags = item["freetype_flags"].get<unsigned>();
                      if (item.contains("extra_glyphs"))
                        ff.extra_glyphs = item["extra_glyphs"].get<std::string>();
                      pack.roles[role].push_back(std::move(ff));
                    }
                  }
                };
                parse_files("Body", FontRole::Body);
                parse_files("H1", FontRole::H1);
                parse_files("H2", FontRole::H2);
                parse_files("H3", FontRole::H3);
                parse_files("Monospace", FontRole::Monospace);
                parse_files("Bold", FontRole::Bold);
                parse_files("Italic", FontRole::Italic);
                parse_files("BoldItalic", FontRole::BoldItalic);
                parse_files("Icons", FontRole::Icons);
                parse_files("Emoji", FontRole::Emoji);
              }

              m_packs[pack.locale] = std::move(pack);
            }

            // Second pass: resolve inheritance after all packs are read
            {
              bool changed = true;
              int guard = 0;
              while (changed && guard++ < 16) {
                changed = false;
                for (auto &kv : m_packs) {
                  auto &child = kv.second;
                  if (child.inherits.empty())
                    continue;
                  auto pit = m_packs.find(child.inherits);
                  if (pit == m_packs.end())
                    continue;
                  const auto &parent = pit->second;

                  // Roles: copy missing ones (do not overwrite existing)
                  for (const auto &rp : parent.roles)
                    if (!child.roles.count(rp.first)) {
                      child.roles[rp.first] = rp.second;
                      changed = true;
                    }

                  // Ranges: inherit if not set explicitly
                  if (child.ranges.empty() && !parent.ranges.empty()) {
                    child.ranges = parent.ranges;
                    changed = true;
                  }
                  // Preset: inherit if not set explicitly
                  if (child.ranges_preset.empty() &&
                      !parent.ranges_preset.empty()) {
                    child.ranges_preset = parent.ranges_preset;
                    changed = true;
                  }
                }
              }
            }
          }

        } catch (const std::exception &e) {
          br.success = false;
          br.message = std::string("JSON parse error: ") + e.what();
          // Fallback to defaults below
        }
      }

        // Choose active locale, or fallback to "default"
        if (!m_packs.empty()) {
            if (!m_packs.count(m_active_locale)) {
                if (m_packs.count("default"))
                m_active_locale = "default";
            }
        }

        // Build now (either from packs or fallback)
        return buildNow();
#   else
        // JSON disabled: just build fallback (Roboto + Icons) using current
        // BuildParams/base_dir.
        return buildNow();
#   endif
    }

    inline std::string FontManager::readTextFile(const std::string &path) {
        std::ifstream ifs(path, std::ios::in | std::ios::binary);
        if (!ifs) return {};
        std::ostringstream oss;
        oss << ifs.rdbuf();
        return oss.str();
    }

    inline void FontManager::addExtraGlyphs(
            ImFontGlyphRangesBuilder &b,
            const std::string &utf8
        ) {
        if (!utf8.empty()) b.AddText(utf8.c_str());
    }

} // namespace ImGuiX::Fonts
