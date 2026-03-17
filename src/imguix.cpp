// Static-library mode still reuses the existing .ipp inclusion path.
// IMGUIX_COMPILED_LIBRARY only flips heavy Category B definitions from inline
// header-only mode to ordinary one-TU definitions while preserving dual-mode use.
#define IMGUIX_COMPILED_LIBRARY
#define IMGUIX_HEADER_ONLY
#include "imguix/core.hpp"
#include "imguix/controllers.hpp"
#include "imguix/extensions.hpp"
#include "imguix/utils.hpp"
#include "imguix/windows.hpp"
#include "imguix/widgets.hpp"
