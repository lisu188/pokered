#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string_view>

#include "pokered/core/game_state.hpp"

namespace pokered {

enum class TileKind : std::uint8_t {
  Floor = 0,
  Wall,
  Door,
  Stairs,
  Tv,
  Table,
};

struct MapCell {
  std::uint8_t behavior_tile = 0;
  bool passable = false;
};

struct Warp {
  int x;
  int y;
  bool uses_last_map;
  WorldId target_map;
  std::uint8_t target_warp;
};

struct BgEvent {
  int x;
  int y;
  MessageId message;
};

struct Npc {
  int x;
  int y;
  Facing facing;
  MessageId message;
};

struct MapData {
  WorldId id;
  std::string_view name;
  int width;
  int height;
  std::uint8_t tileset_id;
  std::uint8_t border_block;
  std::span<const std::uint8_t> block_ids;
  std::span<const MapCell> cells;
  std::span<const Warp> warps;
  std::span<const BgEvent> bg_events;
  std::span<const Npc> npcs;
};

const MapData& GetMapData(WorldId id);
bool HasMapData(WorldId id);
const MapCell& GetCell(const MapData& map, int x, int y);
TileKind RenderTileKind(const MapData& map, int x, int y);
MoveBlocker BlockerAt(const MapData& map, int x, int y);
bool CanMoveTo(const MapData& map, int x, int y);
MessageId InteractionForFacingTile(const MapData& map, const WorldState& world);
int MessagePageCount(MessageId message);
std::string_view MessageText(MessageId message, int page = 0);

}  // namespace pokered
