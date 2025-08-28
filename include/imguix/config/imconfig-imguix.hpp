#pragma once

// ---- ImDrawIdx selection --------------------------------------------------
// Для ImPlot важно поддерживать 32-bits индексы
// Смотреть: https://github.com/ocornut/imgui/issues/2591

#ifdef ImDrawIdx
#  undef ImDrawIdx
#endif
#define ImDrawIdx unsigned int

// Если используем SFML-бэкенд — подтянем их дефолтный конфиг,
// чтобы сохранить совместимость (горячие клавиши, цвета и т.п.).
#ifdef IMGUIX_USE_SFML_BACKEND
#   include "imconfig-SFML.h"
#endif
