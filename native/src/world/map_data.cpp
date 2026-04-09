#include "pokered/world/map_data.hpp"

#include <array>
#include <cstddef>

#include "blues_house_blk.hpp"
#include "blues_house_metadata.hpp"
#include "dojo_blockset.hpp"
#include "oaks_lab_blk.hpp"
#include "oaks_lab_metadata.hpp"
#include "overworld_blockset.hpp"
#include "pallet_town_blk.hpp"
#include "pallet_town_metadata.hpp"
#include "reds_house_1f_blk.hpp"
#include "reds_house_1f_metadata.hpp"
#include "reds_house_2f_blk.hpp"
#include "reds_house_2f_metadata.hpp"
#include "pewter_speech_house_blk.hpp"
#include "pewter_speech_house_metadata.hpp"
#include "house_blockset.hpp"
#include "reds_house_blockset.hpp"

namespace pokered {
namespace {

constexpr int kTilesPerBlock = 16;
constexpr int kBlockTileWidth = 4;
constexpr int kCellsPerBlockEdge = 2;
constexpr int kCollisionStride = 2;
constexpr int kCollisionRowOffset = 1;
constexpr int kSmallIndoorBlockWidth = 4;
constexpr int kSmallIndoorBlockHeight = 4;
constexpr int kSmallIndoorWidth = kSmallIndoorBlockWidth * kCellsPerBlockEdge;
constexpr int kSmallIndoorHeight = kSmallIndoorBlockHeight * kCellsPerBlockEdge;
constexpr int kOaksLabBlockWidth = 5;
constexpr int kOaksLabBlockHeight = 6;
constexpr int kOaksLabWidth = kOaksLabBlockWidth * kCellsPerBlockEdge;
constexpr int kOaksLabHeight = kOaksLabBlockHeight * kCellsPerBlockEdge;
constexpr int kPalletTownBlockWidth = 10;
constexpr int kPalletTownBlockHeight = 9;
constexpr int kPalletTownWidth = kPalletTownBlockWidth * kCellsPerBlockEdge;
constexpr int kPalletTownHeight = kPalletTownBlockHeight * kCellsPerBlockEdge;
constexpr std::uint8_t kOverworldTilesetId = 0;

constexpr std::array<std::uint8_t, 4> kRedsHouse1TableTiles = {
    0x36, 0x38, 0x3A, 0x3C,
};

static_assert(generated::kRedsHouse1FBlocks.size() ==
              static_cast<std::size_t>(kSmallIndoorBlockWidth * kSmallIndoorBlockHeight));
static_assert(generated::kRedsHouse2FBlocks.size() ==
              static_cast<std::size_t>(kSmallIndoorBlockWidth * kSmallIndoorBlockHeight));
static_assert(generated::kPewterSpeechHouseBlocks.size() ==
              static_cast<std::size_t>(kSmallIndoorBlockWidth * kSmallIndoorBlockHeight));
static_assert(generated::kBluesHouseBlocks.size() ==
              static_cast<std::size_t>(kSmallIndoorBlockWidth * kSmallIndoorBlockHeight));
static_assert(generated::kOaksLabBlocks.size() ==
              static_cast<std::size_t>(kOaksLabBlockWidth * kOaksLabBlockHeight));
static_assert(generated::kPalletTownBlocks.size() ==
              static_cast<std::size_t>(kPalletTownBlockWidth * kPalletTownBlockHeight));
static_assert(generated::kRedsHouseBlockset.size() % kTilesPerBlock == 0);
static_assert(generated::kHouseBlockset.size() % kTilesPerBlock == 0);
static_assert(generated::kDojoBlockset.size() % kTilesPerBlock == 0);
static_assert(generated::kOverworldBlockset.size() % kTilesPerBlock == 0);

template <std::size_t N>
constexpr bool Contains(const std::array<std::uint8_t, N>& values, std::uint8_t target) {
  for (const std::uint8_t value : values) {
    if (value == target) {
      return true;
    }
  }
  return false;
}

template <int BlockWidth, int BlockHeight, std::size_t BlockCount, std::size_t BlocksetSize>
constexpr std::uint8_t BehaviorTileAt(const std::array<std::uint8_t, BlockCount>& blocks,
                                      const std::array<std::uint8_t, BlocksetSize>& blockset,
                                      int x,
                                      int y) {
  static_assert(BlockCount == static_cast<std::size_t>(BlockWidth * BlockHeight));

  const int block_x = x / kCellsPerBlockEdge;
  const int block_y = y / kCellsPerBlockEdge;
  const int within_block_x = x % kCellsPerBlockEdge;
  const int within_block_y = y % kCellsPerBlockEdge;
  const std::uint8_t block_id = blocks[static_cast<std::size_t>(block_y * BlockWidth + block_x)];
  const std::size_t block_offset = static_cast<std::size_t>(block_id) * kTilesPerBlock;
  // Overworld interaction logic samples the lower-left 8x8 tile for each 16x16
  // movement cell, so the native importer uses the same representative tile.
  const int tile_row = within_block_y * kCollisionStride + kCollisionRowOffset;
  const int tile_col = within_block_x * kCollisionStride;
  return blockset[block_offset + static_cast<std::size_t>(tile_row * kBlockTileWidth + tile_col)];
}

template <int BlockWidth,
          int BlockHeight,
          std::size_t BlockCount,
          std::size_t BlocksetSize,
          std::size_t CollisionCount>
constexpr auto BuildCells(
    const std::array<std::uint8_t, BlockCount>& blocks,
    const std::array<std::uint8_t, BlocksetSize>& blockset,
    const std::array<std::uint8_t, CollisionCount>& collision_tiles)
    -> std::array<MapCell, static_cast<std::size_t>(BlockWidth * kCellsPerBlockEdge * BlockHeight * kCellsPerBlockEdge)> {
  constexpr int kWidth = BlockWidth * kCellsPerBlockEdge;
  constexpr int kHeight = BlockHeight * kCellsPerBlockEdge;

  std::array<MapCell, static_cast<std::size_t>(kWidth * kHeight)> cells {};
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      const std::uint8_t behavior_tile = BehaviorTileAt<BlockWidth, BlockHeight>(blocks, blockset, x, y);
      cells[static_cast<std::size_t>(y * kWidth + x)] = {
          behavior_tile,
          Contains(collision_tiles, behavior_tile),
      };
    }
  }
  return cells;
}

constexpr std::array<MapCell, kSmallIndoorWidth * kSmallIndoorHeight> kRedsHouse1FCells =
    BuildCells<kSmallIndoorBlockWidth, kSmallIndoorBlockHeight>(
        generated::kRedsHouse1FBlocks, generated::kRedsHouseBlockset, generated::kRedsHouseCollisionTiles);
constexpr std::array<MapCell, kSmallIndoorWidth * kSmallIndoorHeight> kRedsHouse2FCells =
    BuildCells<kSmallIndoorBlockWidth, kSmallIndoorBlockHeight>(
        generated::kRedsHouse2FBlocks, generated::kRedsHouseBlockset, generated::kRedsHouseCollisionTiles);
constexpr std::array<MapCell, kSmallIndoorWidth * kSmallIndoorHeight> kPewterSpeechHouseCells =
    BuildCells<kSmallIndoorBlockWidth, kSmallIndoorBlockHeight>(
        generated::kPewterSpeechHouseBlocks, generated::kHouseBlockset, generated::kHouseCollisionTiles);
constexpr std::array<MapCell, kSmallIndoorWidth * kSmallIndoorHeight> kBluesHouseCells =
    BuildCells<kSmallIndoorBlockWidth, kSmallIndoorBlockHeight>(
        generated::kBluesHouseBlocks, generated::kHouseBlockset, generated::kBluesHouseCollisionTiles);
constexpr std::array<MapCell, kOaksLabWidth * kOaksLabHeight> kOaksLabCells =
    BuildCells<kOaksLabBlockWidth, kOaksLabBlockHeight>(
        generated::kOaksLabBlocks, generated::kDojoBlockset, generated::kDojoCollisionTiles);
constexpr std::array<MapCell, kPalletTownWidth * kPalletTownHeight> kPalletTownCells =
    BuildCells<kPalletTownBlockWidth, kPalletTownBlockHeight>(
        generated::kPalletTownBlocks, generated::kOverworldBlockset, generated::kOverworldCollisionTiles);

constexpr MapData kRedsHouse1F = {
    WorldId::RedsHouse1F,
    "REDS HOUSE 1F",
    kSmallIndoorWidth,
    kSmallIndoorHeight,
    1,
    generated::kRedsHouse1FBorderBlock,
    generated::kRedsHouse1FBlocks,
    kRedsHouse1FCells,
    generated::kRedsHouse1FWarps,
    generated::kRedsHouse1FBgEvents,
    generated::kRedsHouse1FNpcs,
};

constexpr MapData kRedsHouse2F = {
    WorldId::RedsHouse2F,
    "REDS HOUSE 2F",
    kSmallIndoorWidth,
    kSmallIndoorHeight,
    4,
    generated::kRedsHouse2FBorderBlock,
    generated::kRedsHouse2FBlocks,
    kRedsHouse2FCells,
    generated::kRedsHouse2FWarps,
    generated::kRedsHouse2FBgEvents,
    generated::kRedsHouse2FNpcs,
};

constexpr MapData kPewterSpeechHouse = {
    WorldId::PewterSpeechHouse,
    "PEWTER SPEECH HOUSE",
    kSmallIndoorWidth,
    kSmallIndoorHeight,
    8,
    generated::kPewterSpeechHouseBorderBlock,
    generated::kPewterSpeechHouseBlocks,
    kPewterSpeechHouseCells,
    generated::kPewterSpeechHouseWarps,
    generated::kPewterSpeechHouseBgEvents,
    generated::kPewterSpeechHouseNpcs,
};

constexpr MapData kBluesHouse = {
    WorldId::BluesHouse,
    "BLUE'S HOUSE",
    kSmallIndoorWidth,
    kSmallIndoorHeight,
    8,
    generated::kBluesHouseBorderBlock,
    generated::kBluesHouseBlocks,
    kBluesHouseCells,
    generated::kBluesHouseWarps,
    generated::kBluesHouseBgEvents,
    generated::kBluesHouseNpcs,
};

constexpr MapData kOaksLab = {
    WorldId::OaksLab,
    "OAK'S LAB",
    kOaksLabWidth,
    kOaksLabHeight,
    5,
    generated::kOaksLabBorderBlock,
    generated::kOaksLabBlocks,
    kOaksLabCells,
    generated::kOaksLabWarps,
    generated::kOaksLabBgEvents,
    generated::kOaksLabNpcs,
};

constexpr MapData kPalletTown = {
    WorldId::PalletTown,
    "PALLET TOWN",
    kPalletTownWidth,
    kPalletTownHeight,
    kOverworldTilesetId,
    generated::kPalletTownBorderBlock,
    generated::kPalletTownBlocks,
    kPalletTownCells,
    generated::kPalletTownWarps,
    generated::kPalletTownBgEvents,
    generated::kPalletTownNpcs,
};

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

const MapData* FindMapData(WorldId id) {
  switch (id) {
    case WorldId::RedsHouse1F:
      return &kRedsHouse1F;
    case WorldId::RedsHouse2F:
      return &kRedsHouse2F;
    case WorldId::PewterSpeechHouse:
      return &kPewterSpeechHouse;
    case WorldId::BluesHouse:
      return &kBluesHouse;
    case WorldId::OaksLab:
      return &kOaksLab;
    case WorldId::PalletTown:
      return &kPalletTown;
  }
  return nullptr;
}

}  // namespace

const MapData& GetMapData(WorldId id) {
  if (const MapData* map = FindMapData(id)) {
    return *map;
  }
  return kRedsHouse1F;
}

bool HasMapData(WorldId id) {
  return FindMapData(id) != nullptr;
}

const MapCell& GetCell(const MapData& map, int x, int y) {
  return map.cells[static_cast<std::size_t>(y * map.width + x)];
}

TileKind RenderTileKind(const MapData& map, int x, int y) {
  for (const BgEvent& event : map.bg_events) {
    if (event.x == x && event.y == y && event.message == MessageId::TvMovie) {
      return TileKind::Tv;
    }
  }

  for (const Warp& warp : map.warps) {
    if (warp.x == x && warp.y == y) {
      return (warp.uses_last_map || map.tileset_id == kOverworldTilesetId) ? TileKind::Door : TileKind::Stairs;
    }
  }

  const MapCell& cell = GetCell(map, x, y);
  if (cell.passable) {
    return TileKind::Floor;
  }
  if (Contains(kRedsHouse1TableTiles, cell.behavior_tile)) {
    return TileKind::Table;
  }
  return TileKind::Wall;
}

bool CanMoveTo(const MapData& map, int x, int y) {
  if (x < 0 || y < 0 || x >= map.width || y >= map.height) {
    return false;
  }

  if (!GetCell(map, x, y).passable) {
    return false;
  }

  for (const Npc& npc : map.npcs) {
    if (npc.x == x && npc.y == y) {
      return false;
    }
  }

  return true;
}

MessageId InteractionForFacingTile(const MapData& map, const WorldState& world) {
  const auto [dx, dy] = FacingOffset(world.player.facing);
  const int target_x = world.player.x + dx;
  const int target_y = world.player.y + dy;

  for (const Npc& npc : map.npcs) {
    if (npc.x == target_x && npc.y == target_y) {
      if (npc.message == MessageId::MomWakeUp && world.got_starter) {
        return MessageId::MomRest;
      }
      if (npc.message == MessageId::OaksLabRival) {
        return world.got_starter ? MessageId::OaksLabRivalMyPokemonLooksStronger
                                 : MessageId::OaksLabRivalGrampsIsntAround;
      }
      if (npc.message == MessageId::OaksLabPokeBall) {
        return world.got_starter ? MessageId::OaksLabLastMon : MessageId::OaksLabThoseArePokeBalls;
      }
      if (npc.message == MessageId::OaksLabOak1) {
        return world.got_starter ? MessageId::OaksLabOak1YourPokemonCanFight
                                 : MessageId::OaksLabOak1WhichPokemonDoYouWant;
      }
      return npc.message;
    }
  }

  for (const BgEvent& event : map.bg_events) {
    if (event.x == target_x && event.y == target_y) {
      if (event.message == MessageId::TvMovie && world.player.facing != Facing::Up) {
        return MessageId::TvWrongSide;
      }
      return event.message;
    }
  }

  return MessageId::None;
}

std::string_view RawMessageText(MessageId message) {
  switch (message) {
    case MessageId::None:
      return "";
    case MessageId::MomWakeUp:
      return generated::kRedsHouse1FMomWakeUpText;
    case MessageId::MomRest:
      return generated::kRedsHouse1FMomYouShouldRestText;
    case MessageId::TvMovie:
      return generated::kRedsHouse1FTVStandByMeMovieText;
    case MessageId::TvWrongSide:
      return generated::kRedsHouse1FTVWrongSideText;
    case MessageId::PewterSpeechHouseGambler:
      return generated::kPewterSpeechHouseGamblerText;
    case MessageId::PewterSpeechHouseYoungster:
      return generated::kPewterSpeechHouseYoungsterText;
    case MessageId::BluesHouseDaisyRivalAtLab:
      return generated::kBluesHouseDaisyRivalAtLabText;
    case MessageId::BluesHouseDaisyWalking:
      return generated::kBluesHouseDaisyWalkingText;
    case MessageId::BluesHouseTownMap:
      return generated::kBluesHouseTownMapText;
    case MessageId::OaksLabRival:
    case MessageId::OaksLabPokeBall:
    case MessageId::OaksLabOak1:
      return "";
    case MessageId::OaksLabPokedex:
      return generated::kOaksLabPokedexText;
    case MessageId::OaksLabOak2:
      return generated::kOaksLabOak2Text;
    case MessageId::OaksLabGirl:
      return generated::kOaksLabGirlText;
    case MessageId::OaksLabScientist:
      return generated::kOaksLabScientistText;
    case MessageId::OaksLabRivalGrampsIsntAround:
      return generated::kOaksLabRivalGrampsIsntAroundText;
    case MessageId::OaksLabRivalMyPokemonLooksStronger:
      return generated::kOaksLabRivalMyPokemonLooksStrongerText;
    case MessageId::OaksLabThoseArePokeBalls:
      return generated::kOaksLabThoseArePokeBallsText;
    case MessageId::OaksLabLastMon:
      return generated::kOaksLabLastMonText;
    case MessageId::OaksLabOak1WhichPokemonDoYouWant:
      return generated::kOaksLabOak1WhichPokemonDoYouWantText;
    case MessageId::OaksLabOak1YourPokemonCanFight:
      return generated::kOaksLabOak1YourPokemonCanFightText;
    case MessageId::PalletTownOakHeyWaitDontGoOut:
      return generated::kPalletTownOakHeyWaitDontGoOutText;
    case MessageId::PalletTownOak:
      return generated::kPalletTownOakItsUnsafeText;
    case MessageId::PalletTownGirl:
      return generated::kPalletTownGirlText;
    case MessageId::PalletTownFisher:
      return generated::kPalletTownFisherText;
    case MessageId::PalletTownOaksLabSign:
      return generated::kPalletTownOaksLabSignText;
    case MessageId::PalletTownSign:
      return generated::kPalletTownSignText;
    case MessageId::PalletTownPlayersHouseSign:
      return generated::kPalletTownPlayersHouseSignText;
    case MessageId::PalletTownRivalsHouseSign:
      return generated::kPalletTownRivalsHouseSignText;
    case MessageId::SaveOk:
      return "SAVE WRITTEN TO\nNATIVE-SAVE.SAV";
    case MessageId::LoadOk:
      return "SAVE LOADED.";
    case MessageId::SaveMissing:
      return "NO SAVE FILE FOUND.";
    case MessageId::SaveCorrupt:
      return "SAVE FILE FAILED\nCHECKSUM VALIDATION.";
  }
  return "";
}

int CountPages(std::string_view text) {
  if (text.empty()) {
    return 1;
  }

  int count = 1;
  for (const char ch : text) {
    if (ch == '\f') {
      ++count;
    }
  }
  return count;
}

std::string_view StripTextTerminator(std::string_view text) {
  while (!text.empty() && text.back() == '@') {
    text.remove_suffix(1);
  }
  return text;
}

std::string_view TextPage(std::string_view text, int page) {
  if (page < 0) {
    page = 0;
  }

  std::size_t start = 0;
  int current_page = 0;
  while (true) {
    const std::size_t end = text.find('\f', start);
    if (current_page == page || end == std::string_view::npos) {
      const std::size_t count = end == std::string_view::npos ? text.size() - start : end - start;
      return text.substr(start, count);
    }
    start = end + 1;
    ++current_page;
  }
}

int MessagePageCount(MessageId message) {
  return CountPages(StripTextTerminator(RawMessageText(message)));
}

std::string_view MessageText(MessageId message, int page) {
  const std::string_view text = StripTextTerminator(RawMessageText(message));
  const int page_count = CountPages(text);
  if (page < 0 || page >= page_count) {
    return TextPage(text, 0);
  }
  return TextPage(text, page);
}

}  // namespace pokered
