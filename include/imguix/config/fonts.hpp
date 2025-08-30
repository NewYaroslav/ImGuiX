#pragma once
#ifndef _IMGUIX_CONFIG_FONTS_HPP_INCLUDED
#define _IMGUIX_CONFIG_FONTS_HPP_INCLUDED

/// \file fonts.hpp
/// \brief Default font configuration paths.

#ifndef IMGUIX_FONTS_DIR
/// \brief Default fonts directory.
#   define IMGUIX_FONTS_DIR u8"data/resources/fonts"
#endif

#ifndef IMGUIX_FONTS_CONFIG
/// \brief Default fonts configuration file.
#   define IMGUIX_FONTS_CONFIG u8"data/resources/fonts/fonts.json"
#endif

#ifndef IMGUIX_FONTS_CONFIG_BASENAME
/// \brief Basename of fonts configuration file.
#   define IMGUIX_FONTS_CONFIG_BASENAME u8"fonts.json"
#endif

#ifndef IMGUIX_FONTS_FALLBACK_BODY_BASENAME
/// \brief Fallback body font filename.
#   define IMGUIX_FONTS_FALLBACK_BODY_BASENAME u8"Roboto-Medium.ttf"
#endif

#ifndef IMGUIX_FONTS_FALLBACK_ICONS_BASENAME
/// \brief Fallback icon font filename.
#   define IMGUIX_FONTS_FALLBACK_ICONS_BASENAME u8"forkawesome-webfont.ttf"
#endif

#endif // _IMGUIX_CONFIG_FONTS_HPP_INCLUDED
