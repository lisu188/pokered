#include "pokered/core/game_state.hpp"

#include <cstddef>
#include <optional>
#include <utility>

#include "pokered/world/map_data.hpp"

namespace pokered {
namespace {

std::pair<int, int> FacingOffset(Facing facing) {
  switch (facing) {
    case Facing::Up:
      return {0, -1};
    case Facing::Down:
      return {0, 1};
    case Facing::Left:
      return {-1, 0};
    case Facing::Right:
      return {1, 0};
  }
  return {0, 0};
}

}  // namespace

void StartNewGameShortcut(GameState& state) {
  state.scene = SceneId::World;
  state.menu_index = 0;
  state.active_message = MessageId::None;
  state.world.map_id = WorldId::RedsHouse1F;
  state.world.player = PlayerState {};
  state.world.got_starter = false;
  state.world.rng_state = 0x4F414B31u;
  state.world.step_counter = 0;
  state.world.last_map = static_cast<std::uint16_t>(WorldId::PalletTown);
  state.world.last_warp = 1;
}

MoveResult TryMoveWithResult(WorldState& world, Facing facing) {
  world.player.facing = facing;

  const auto [dx, dy] = FacingOffset(facing);
  const int from_x = world.player.x;
  const int from_y = world.player.y;
  const int next_x = from_x + dx;
  const int next_y = from_y + dy;
  const MapData& map = GetMapData(world.map_id);
  MoveResult result {
      .moved = false,
      .message = MessageId::None,
      .warped = false,
      .source_map = map.id,
      .source_warp = 0,
      .target_map = map.id,
      .target_warp = 0,
      .facing = facing,
      .from_x = from_x,
      .from_y = from_y,
      .to_x = next_x,
      .to_y = next_y,
      .blocker = MoveBlocker::None,
      .state_gate = StateGate::None,
      .state_gate_value = false,
  };
  if (map.id == WorldId::PalletTown && !world.got_starter && facing == Facing::Up && next_y == 1) {
    result.message = MessageId::PalletTownOakHeyWaitDontGoOut;
    result.blocker = MoveBlocker::Script;
    result.state_gate = StateGate::GotStarter;
    result.state_gate_value = false;
    return result;
  }
  result.blocker = BlockerAt(map, next_x, next_y);
  if (result.blocker != MoveBlocker::None) {
    return result;
  }

  world.player.x = next_x;
  world.player.y = next_y;
  ++world.step_counter;
  result.moved = true;

  for (std::size_t index = 0; index < map.warps.size(); ++index) {
    const Warp& warp = map.warps[index];
    if (warp.x != world.player.x || warp.y != world.player.y) {
      continue;
    }

    std::optional<WorldId> target_map_id;
    if (warp.uses_last_map) {
      if (world.last_map == kNoLastMap) {
        continue;
      }
      const auto candidate = static_cast<WorldId>(world.last_map);
      if (!HasMapData(candidate)) {
        continue;
      }
      target_map_id = candidate;
      world.last_map = static_cast<std::uint16_t>(map.id);
      world.last_warp = static_cast<std::uint8_t>(index + 1);
    } else {
      if (!HasMapData(warp.target_map)) {
        continue;
      }
      world.last_map = static_cast<std::uint16_t>(map.id);
      world.last_warp = static_cast<std::uint8_t>(index + 1);
      target_map_id = warp.target_map;
    }

    world.map_id = *target_map_id;
    result.warped = true;
    result.source_warp = static_cast<std::uint8_t>(index + 1);
    result.target_map = *target_map_id;
    result.target_warp = warp.target_warp;

    const MapData& target_map = GetMapData(world.map_id);
    if (warp.target_warp == 0 || warp.target_warp > target_map.warps.size()) {
      return result;
    }

    const Warp& target_warp = target_map.warps[warp.target_warp - 1];
    world.player.x = target_warp.x;
    world.player.y = target_warp.y;
    world.player.facing = Facing::Down;
    return result;
  }

  return result;
}

bool TryMove(WorldState& world, Facing facing) {
  return TryMoveWithResult(world, facing).moved;
}

}  // namespace pokered
