#pragma once

#include <cstdint>

namespace pokered {

enum class SceneId : std::uint8_t {
  Title = 0,
  World = 1,
};

enum class Facing : std::uint8_t {
  Up = 0,
  Down = 1,
  Left = 2,
  Right = 3,
};

enum class WorldId : std::uint8_t {
  RedsHouse1F = 0,
  RedsHouse2F = 1,
  PewterSpeechHouse = 2,
  PalletTown = 3,
  BluesHouse = 4,
  OaksLab = 5,
};

enum class MessageId : std::uint8_t {
  None = 0,
  MomWakeUp,
  MomRest,
  TvMovie,
  TvWrongSide,
  PewterSpeechHouseGambler,
  PewterSpeechHouseYoungster,
  BluesHouseDaisyRivalAtLab,
  BluesHouseDaisyWalking,
  BluesHouseTownMap,
  OaksLabRival,
  OaksLabPokeBall,
  OaksLabOak1,
  OaksLabPokedex,
  OaksLabOak2,
  OaksLabGirl,
  OaksLabScientist,
  OaksLabRivalGrampsIsntAround,
  OaksLabRivalMyPokemonLooksStronger,
  OaksLabThoseArePokeBalls,
  OaksLabLastMon,
  OaksLabOak1WhichPokemonDoYouWant,
  OaksLabOak1YourPokemonCanFight,
  PalletTownOakHeyWaitDontGoOut,
  PalletTownOak,
  PalletTownGirl,
  PalletTownFisher,
  PalletTownOaksLabSign,
  PalletTownSign,
  PalletTownPlayersHouseSign,
  PalletTownRivalsHouseSign,
  SaveOk,
  LoadOk,
  SaveMissing,
  SaveCorrupt,
};

enum class MoveBlocker : std::uint8_t {
  None = 0,
  Bounds,
  Collision,
  Npc,
  Script,
};

inline constexpr std::uint16_t kNoLastMap = 0xFFFFu;

struct PlayerState {
  int x = 3;
  int y = 6;
  Facing facing = Facing::Up;
};

struct WorldState {
  WorldId map_id = WorldId::RedsHouse1F;
  PlayerState player {};
  bool got_starter = false;
  std::uint32_t rng_state = 0x4F414B31u;
  std::uint64_t step_counter = 0;
  std::uint16_t last_map = kNoLastMap;
  std::uint8_t last_warp = 1;
};

struct GameState {
  SceneId scene = SceneId::Title;
  int menu_index = 0;
  bool has_save = false;
  WorldState world {};
  MessageId active_message = MessageId::None;
  int active_message_page = 0;
  bool running = true;
};

struct MoveResult {
  bool moved = false;
  MessageId message = MessageId::None;
  bool warped = false;
  WorldId source_map = WorldId::RedsHouse1F;
  std::uint8_t source_warp = 0;
  WorldId target_map = WorldId::RedsHouse1F;
  std::uint8_t target_warp = 0;
  Facing facing = Facing::Up;
  int from_x = 0;
  int from_y = 0;
  int to_x = 0;
  int to_y = 0;
  MoveBlocker blocker = MoveBlocker::None;
};

void StartNewGameShortcut(GameState& state);
MoveResult TryMoveWithResult(WorldState& world, Facing facing);
bool TryMove(WorldState& world, Facing facing);

}  // namespace pokered
