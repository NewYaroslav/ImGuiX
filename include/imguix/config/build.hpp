#pragma once
#ifndef _IMGUIX_CONFIG_BUILD_HPP_INCLUDED
#define _IMGUIX_CONFIG_BUILD_HPP_INCLUDED

/// \file build.hpp
/// \brief Internal build-mode helpers shared by ImGuiX implementation headers.

#ifndef IMGUIX_IMPL_INLINE
#   if defined(IMGUIX_HEADER_ONLY) && !defined(IMGUIX_COMPILED_LIBRARY)
#       define IMGUIX_IMPL_INLINE inline
#   else
#       define IMGUIX_IMPL_INLINE
#   endif
#endif

#endif // _IMGUIX_CONFIG_BUILD_HPP_INCLUDED
