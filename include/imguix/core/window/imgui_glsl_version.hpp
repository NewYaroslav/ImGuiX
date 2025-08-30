/// \file imgui_glsl_version.hpp
/// \brief Define GLSL version string based on platform.
#pragma once
#ifndef IMGUIX_GLSL_VERSION
#  if defined(__EMSCRIPTEN__)
#    define IMGUIX_GLSL_VERSION u8"#version 300 es"   // WebGL2
#  elif defined(IMGUI_IMPL_OPENGL_ES3)
#    define IMGUIX_GLSL_VERSION u8"#version 300 es"
#  elif defined(IMGUI_IMPL_OPENGL_ES2)
#    define IMGUIX_GLSL_VERSION u8"#version 100"
#  elif defined(__APPLE__)
#    define IMGUIX_GLSL_VERSION u8"#version 150"      // macOS Core 3.2
#  else
#    define IMGUIX_GLSL_VERSION u8"#version 130"      // Win/Linux Desktop GL
#  endif
#endif
