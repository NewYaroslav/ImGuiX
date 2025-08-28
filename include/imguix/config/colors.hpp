#pragma once
#ifndef _IMGUIX_CONFIG_COLORS_HPP_INCLUDED
#define _IMGUIX_CONFIG_COLORS_HPP_INCLUDED

/// \file colors.hpp
/// \brief Default color constants for ImGuiX widgets.

#include <imgui.h>

/// \brief RGBA color applied to invalid inputs.
#define IMGUIX_COLOR_ERROR ImVec4(0.9f, 0.5f, 0.5f, 1.0f)

/// \brief RGBA color for warning markers.
#define IMGUIX_COLOR_WARNING ImVec4(1.0f, 1.0f, 0.0f, 1.0f)

/// \brief RGBA color for informational markers.
#define IMGUIX_COLOR_INFO ImVec4(0.10f, 0.45f, 0.95f, 1.0f)

/// \brief RGBA color for success markers.
#define IMGUIX_COLOR_SUCCESS ImVec4(0.0f, 0.60f, 0.0f, 1.0f)

/// \brief Default bull candle color for PlotOHLCChart.
#define IMGUIX_COLOR_OHLC_BULL ImVec4(0.0f, 1.0f, 0.441f, 1.0f)

/// \brief Default bear candle color for PlotOHLCChart.
#define IMGUIX_COLOR_OHLC_BEAR ImVec4(0.853f, 0.05f, 0.31f, 1.0f)

#endif // _IMGUIX_CONFIG_COLORS_HPP_INCLUDED
