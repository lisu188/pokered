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

struct MapConnection {
  Facing direction;
  WorldId target_map;
  int offset;
};

enum class InteractionKind : std::uint8_t {
  None = 0,
  Npc,
  BgEvent,
};

struct InteractionResult {
  InteractionKind kind = InteractionKind::None;
  MessageId origin_message = MessageId::None;
  MessageId message = MessageId::None;
  int target_x = 0;
  int target_y = 0;
  StateGate state_gate = StateGate::None;
  bool state_gate_value = false;
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
  std::span<const MapConnection> connections {};
};

const MapData& GetMapData(WorldId id);
bool HasMapData(WorldId id);
const MapCell& GetCell(const MapData& map, int x, int y);
TileKind RenderTileKind(const MapData& map, int x, int y);
MoveBlocker BlockerAt(const MapData& map, int x, int y);
bool CanMoveTo(const MapData& map, int x, int y);
bool ShouldAutoStepDoorExit(const MapData& map, int x, int y);
InteractionResult InspectFacingTile(const MapData& map, const WorldState& world);
MessageId InteractionForFacingTile(const MapData& map, const WorldState& world);
int MessagePageCount(MessageId message);
std::string_view MessageText(MessageId message, int page = 0);

}  // namespace pokered
