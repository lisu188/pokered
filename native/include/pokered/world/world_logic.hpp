#pragma once

#include "pokered/core/game_state.hpp"

namespace pokered {

bool TryMove(WorldState& world, Facing facing);
bool TryEnterWarp(WorldState& world);

}  // namespace pokered
