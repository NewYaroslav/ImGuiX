#pragma once
#ifndef _IMGUIX_CONFIG_PATHS_HPP_INCLUDED
#define _IMGUIX_CONFIG_PATHS_HPP_INCLUDED

/// \file paths.hpp
/// \brief Default paths for configuration files.

#ifndef IMGUIX_CONFIG_DIR
/// \brief Directory for configuration files.
#   define IMGUIX_CONFIG_DIR u8"data/config"
#endif

#ifndef IMGUIX_OPTIONS_FILENAME
/// \brief Options storage filename.
#   define IMGUIX_OPTIONS_FILENAME u8"options.json"
#endif

#endif // _IMGUIX_CONFIG_PATHS_HPP_INCLUDED
