#pragma once
#ifndef _IMGUIX_CORE_I18N_I18N_HPP_INCLUDED
#define _IMGUIX_CORE_I18N_I18N_HPP_INCLUDED

/// \file i18n.hpp
/// \brief Central include for ImGuiX i18n components.
/// \note Public API: ImGuiX::Utils::I18N::LangStore and ImGuiX::Utils::I18N::PluralRules.
/// \note LangStore searches resources relative to the executable by default.
/// \note Override lookup roots via the macros below.

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <deque>
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

#include <imguix/config/i18n.hpp>
#include <imguix/utils/strip_json_comments.hpp>
#include <imguix/utils/path_utils.hpp>

#include "i18n/PluralRules.hpp"
#include "i18n/LangStore.hpp"

#endif // _IMGUIX_CORE_I18N_I18N_HPP_INCLUDED

