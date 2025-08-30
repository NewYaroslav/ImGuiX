#pragma once
#ifndef _IMGUIX_CONFIG_I18N_HPP_INCLUDED
#define _IMGUIX_CONFIG_I18N_HPP_INCLUDED

/// \file i18n.hpp
/// \brief Internationalization paths and settings.

#ifndef IMGUIX_I18N_DIR
/// \brief Default i18n resources directory.
#   define IMGUIX_I18N_DIR u8"data/resources/i18n"
#endif

#ifndef IMGUIX_I18N_JSON_BASENAME
/// \brief Basename of the localization file.
#   define IMGUIX_I18N_JSON_BASENAME u8"strings.json"
#endif

#ifndef IMGUIX_I18N_PLURALS_FILENAME
/// \brief Filename of the plural rules file.
#   define IMGUIX_I18N_PLURALS_FILENAME u8"plurals.json"
#endif

#ifndef IMGUIX_RESOLVE_PATHS_REL_TO_EXE
/// \brief Resolve resource paths relative to the executable when nonzero.
#   define IMGUIX_RESOLVE_PATHS_REL_TO_EXE 1
#endif

#endif // _IMGUIX_CONFIG_I18N_HPP_INCLUDED
