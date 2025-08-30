#pragma once

/// \file imconfig-imguix.hpp
/// \brief ImGui configuration overrides for ImGuiX.
/// \details Forces 32-bit indices and includes backend config when needed.

#ifdef ImDrawIdx
#  undef ImDrawIdx
#endif
/// \brief Use 32-bit indices for ImDrawIdx to support ImPlot.
#define ImDrawIdx unsigned int

#ifdef IMGUIX_USE_SFML_BACKEND
/// \brief Include SFML backend config for hotkeys and colors.
#   include "imconfig-SFML.h"
#endif
