#pragma once
#ifndef _IMGUIX_CORE_I18N_I18N_HPP_INCLUDED
#define _IMGUIX_CORE_I18N_I18N_HPP_INCLUDED

/// \file i18n.hpp
/// \brief Central include for ImGuiX i18n (localization) components.
///
/// Public API:
/// - ImGuiX::Utils::I18N::LangStore   — key/value strings, docs, plural forms, formatting
/// - ImGuiX::Utils::I18N::PluralRules — basic pluralization helpers
///
/// Notes:
/// - By default, LangStore looks for resources relative to the executable.
/// - You can override default lookup roots via the macros below.

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <sstream>
#include <utility>
#include <stdexcept>
#include <system_error>
#include <memory>

#include <fmt/core.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <filesystem>

#include <imguix/utils/strip_json_comments.hpp>
#include <imguix/utils/path_utils.hpp>

#ifndef IMGUIX_I18N_DIR
#   define IMGUIX_I18N_DIR "data/resources/i18n"   // default root for i18n assets
#endif

#ifndef IMGUIX_I18N_JSON_BASENAME
#   define IMGUIX_I18N_JSON_BASENAME "strings.json"
#endif

#ifndef IMGUIX_I18N_PLURALS_FILENAME
#   define IMGUIX_I18N_PLURALS_FILENAME "plurals.json"
#endif

// 1 = resolve IMGUIX_I18N_DIR relatively to the executable location.
// 0 = use IMGUIX_I18N_DIR "as is".
#ifndef IMGUIX_RESOLVE_PATHS_REL_TO_EXE
#   define IMGUIX_RESOLVE_PATHS_REL_TO_EXE 1
#endif

#include "i18n/PluralRules.hpp"
#include "i18n/LangStore.hpp"

#endif // _IMGUIX_CORE_I18N_I18N_HPP_INCLUDED

