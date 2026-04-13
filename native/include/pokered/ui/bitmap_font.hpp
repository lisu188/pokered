#pragma once

#include <string_view>

#include <SDL.h>

namespace pokered {

void DrawText(SDL_Renderer* renderer,
              int x,
              int y,
              std::string_view text,
              SDL_Color color,
              int scale);

}  // namespace pokered
