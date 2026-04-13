#include "pokered/world/world_logic.hpp"

#include <cstddef>
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

const Warp* FindWarpAt(const MapData& map, int x, int y, std::uint8_t* warp_index = nullptr) {
  for (std::size_t i = 0; i < map.warps.size(); ++i) {
    const Warp& warp = map.warps[i];
    if (warp.x == x && warp.y == y) {
      if (warp_index != nullptr) {
        *warp_index = static_cast<std::uint8_t>(i + 1);
      }
      return &warp;
    }
  }
  return nullptr;
}

}  // namespace

bool TryEnterWarp(WorldState& world) {
  const MapData& source_map = GetMapData(world.map_id);
  std::uint8_t source_warp_index = 0;
  const Warp* warp = FindWarpAt(source_map, world.player.x, world.player.y, &source_warp_index);
  if (warp == nullptr || warp->uses_last_map || !HasMapData(warp->target_map)) {
    return false;
  }

  const MapData& target_map = GetMapData(warp->target_map);
  if (warp->target_warp == 0 || warp->target_warp > target_map.warps.size()) {
    return false;
  }

  const Warp& destination = target_map.warps[static_cast<std::size_t>(warp->target_warp - 1)];
  world.last_map = static_cast<std::uint16_t>(world.map_id);
  world.last_warp = source_warp_index;
  world.map_id = warp->target_map;
  world.player.x = destination.x;
  world.player.y = destination.y;
  world.player.facing = world.map_id == WorldId::RedsHouse2F ? Facing::Up : Facing::Down;
  return true;
}

bool TryMove(WorldState& world, Facing facing) {
  world.player.facing = facing;
  const auto [dx, dy] = FacingOffset(facing);
  const int next_x = world.player.x + dx;
  const int next_y = world.player.y + dy;
  const MapData& map = GetMapData(world.map_id);
  if (!CanMoveTo(map, next_x, next_y)) {
    return false;
  }

  world.player.x = next_x;
  world.player.y = next_y;
  ++world.step_counter;
  TryEnterWarp(world);
  return true;
}

}  // namespace pokered
