#include "pokered/app/application.hpp"

#include <algorithm>
#include <SDL.h>

#include <filesystem>
#include <iostream>
#include <string_view>

#include "pokered/core/game_state.hpp"
#include "pokered/save/save_system.hpp"
#include "pokered/ui/bitmap_font.hpp"
#include "pokered/world/map_data.hpp"

namespace pokered {
namespace {

constexpr int kLogicalWidth = 160;
constexpr int kLogicalHeight = 144;
constexpr int kWindowScale = 4;
constexpr int kTilePixels = 10;
constexpr int kMapViewportWidth = 12;
constexpr int kMapViewportHeight = 8;
constexpr int kMapOffsetX = 20;
constexpr int kMapOffsetY = 14;
constexpr int kTextBoxY = 98;

struct FrameInput {
  bool up = false;
  bool down = false;
  bool left = false;
  bool right = false;
  bool confirm = false;
  bool back = false;
  bool save = false;
  bool load = false;
  bool toggle_starter = false;
  bool cycle_map = false;
};

struct CameraView {
  int origin_x = 0;
  int origin_y = 0;
  int width = 0;
  int height = 0;
  int render_x = kMapOffsetX;
  int render_y = kMapOffsetY;
};

const std::filesystem::path& SavePath() {
  static const std::filesystem::path path = "native-save.sav";
  return path;
}

std::filesystem::path TempRuntimePath(std::string_view filename) {
  std::error_code error;
  std::filesystem::path directory = std::filesystem::temp_directory_path(error);
  if (error || directory.empty() || !std::filesystem::is_directory(directory, error)) {
    directory = "/tmp";
  }
  return directory / std::string(filename);
}

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

CameraView BuildCameraView(const MapData& map, const WorldState& world) {
  CameraView view {};
  view.width = std::min(map.width, kMapViewportWidth);
  view.height = std::min(map.height, kMapViewportHeight);
  view.origin_x = std::clamp(world.player.x - view.width / 2, 0, std::max(0, map.width - view.width));
  view.origin_y = std::clamp(world.player.y - view.height / 2, 0, std::max(0, map.height - view.height));
  view.render_x = kMapOffsetX + (kMapViewportWidth - view.width) * kTilePixels / 2;
  view.render_y = kMapOffsetY + (kMapViewportHeight - view.height) * kTilePixels / 2;
  return view;
}

int FindPalletTownNorthExitX(const MapData& map) {
  for (int x = 0; x < map.width; ++x) {
    if (CanMoveTo(map, x, 2) && CanMoveTo(map, x, 1)) {
      return x;
    }
  }
  return -1;
}

void RefreshSaveAvailability(GameState& state) {
  state.has_save = SaveSystem::Exists(SavePath());
}

void ShowMessage(GameState& state, MessageId message) {
  state.active_message = message;
  state.active_message_page = 0;
}

void AdvanceOrDismissMessage(GameState& state) {
  if (state.active_message == MessageId::None) {
    state.active_message_page = 0;
    return;
  }

  if (state.active_message_page + 1 < MessagePageCount(state.active_message)) {
    ++state.active_message_page;
    return;
  }

  state.active_message = MessageId::None;
  state.active_message_page = 0;
}

void StartNewGame(GameState& state) {
  StartNewGameShortcut(state);
  RefreshSaveAvailability(state);
}

void HandleWorldMove(GameState& state, Facing facing) {
  const MoveResult result = TryMoveWithResult(state.world, facing);
  if (result.message != MessageId::None) {
    ShowMessage(state, result.message);
  }
}

void TryLoadGame(GameState& state) {
  const LoadResult result = SaveSystem::Load(SavePath());
  switch (result.status) {
    case LoadStatus::Ok:
      state = result.state;
      ShowMessage(state, MessageId::LoadOk);
      RefreshSaveAvailability(state);
      break;
    case LoadStatus::Missing:
      ShowMessage(state, MessageId::SaveMissing);
      break;
    case LoadStatus::Corrupt:
      ShowMessage(state, MessageId::SaveCorrupt);
      break;
    case LoadStatus::IoError:
      ShowMessage(state, MessageId::SaveCorrupt);
      break;
  }
}

void TrySaveGame(GameState& state) {
  if (SaveSystem::Save(SavePath(), state)) {
    ShowMessage(state, MessageId::SaveOk);
    RefreshSaveAvailability(state);
  } else {
    ShowMessage(state, MessageId::SaveCorrupt);
  }
}

void SetDebugMap(GameState& state, WorldId map_id) {
  state.world.map_id = map_id;
  state.active_message = MessageId::None;
  state.active_message_page = 0;

  switch (map_id) {
    case WorldId::RedsHouse1F:
      state.world.player = PlayerState {3, 6, Facing::Up};
      state.world.last_map = static_cast<std::uint16_t>(WorldId::PalletTown);
      state.world.last_warp = 1;
      break;
    case WorldId::RedsHouse2F:
      state.world.player = PlayerState {7, 2, Facing::Up};
      state.world.last_map = static_cast<std::uint16_t>(WorldId::RedsHouse1F);
      state.world.last_warp = 3;
      break;
    case WorldId::PewterSpeechHouse:
      state.world.player = PlayerState {3, 6, Facing::Up};
      state.world.last_map = kNoLastMap;
      state.world.last_warp = 1;
      break;
    case WorldId::BluesHouse:
      state.world.player = PlayerState {3, 6, Facing::Up};
      state.world.last_map = static_cast<std::uint16_t>(WorldId::PalletTown);
      state.world.last_warp = 2;
      break;
    case WorldId::PalletTown:
      state.world.player = PlayerState {5, 6, Facing::Up};
      state.world.last_map = static_cast<std::uint16_t>(WorldId::RedsHouse1F);
      state.world.last_warp = 2;
      break;
    case WorldId::OaksLab:
      state.world.player = PlayerState {2, 2, Facing::Up};
      state.world.last_map = static_cast<std::uint16_t>(WorldId::PalletTown);
      state.world.last_warp = 3;
      break;
  }
}

void CycleDebugMap(GameState& state) {
  switch (state.world.map_id) {
    case WorldId::RedsHouse1F:
      SetDebugMap(state, WorldId::PalletTown);
      break;
    case WorldId::PalletTown:
      SetDebugMap(state, WorldId::BluesHouse);
      break;
    case WorldId::BluesHouse:
      SetDebugMap(state, WorldId::OaksLab);
      break;
    case WorldId::OaksLab:
      SetDebugMap(state, WorldId::RedsHouse2F);
      break;
    case WorldId::RedsHouse2F:
      SetDebugMap(state, WorldId::PewterSpeechHouse);
      break;
    case WorldId::PewterSpeechHouse:
      SetDebugMap(state, WorldId::RedsHouse1F);
      break;
  }
}

FrameInput PollInput(GameState& state) {
  FrameInput input {};
  SDL_Event event {};
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      state.running = false;
      continue;
    }

    if (event.type != SDL_KEYDOWN || event.key.repeat != 0) {
      continue;
    }

    switch (event.key.keysym.sym) {
      case SDLK_UP:
        input.up = true;
        break;
      case SDLK_DOWN:
        input.down = true;
        break;
      case SDLK_LEFT:
        input.left = true;
        break;
      case SDLK_RIGHT:
        input.right = true;
        break;
      case SDLK_RETURN:
      case SDLK_SPACE:
      case SDLK_z:
        input.confirm = true;
        break;
      case SDLK_x:
      case SDLK_ESCAPE:
        input.back = true;
        break;
      case SDLK_F5:
        input.save = true;
        break;
      case SDLK_F9:
        input.load = true;
        break;
      case SDLK_g:
        input.toggle_starter = true;
        break;
      case SDLK_F6:
        input.cycle_map = true;
        break;
      default:
        break;
    }
  }
  return input;
}

void UpdateTitle(GameState& state, const FrameInput& input) {
  if (input.up || input.down) {
    state.menu_index = state.menu_index == 0 ? 1 : 0;
  }

  if (!input.confirm) {
    return;
  }

  if (state.menu_index == 0) {
    StartNewGame(state);
    return;
  }

  if (state.has_save) {
    TryLoadGame(state);
  } else {
    ShowMessage(state, MessageId::SaveMissing);
  }
}

void UpdateWorld(GameState& state, const FrameInput& input) {
  if (input.toggle_starter) {
    state.world.got_starter = !state.world.got_starter;
  }
  if (input.cycle_map) {
    CycleDebugMap(state);
    return;
  }

  if (state.active_message != MessageId::None) {
    if (input.confirm || input.back) {
      AdvanceOrDismissMessage(state);
    }
    return;
  }

  if (input.save) {
    TrySaveGame(state);
    return;
  }
  if (input.load) {
    TryLoadGame(state);
    return;
  }

  if (input.up) {
    HandleWorldMove(state, Facing::Up);
    return;
  }
  if (input.down) {
    HandleWorldMove(state, Facing::Down);
    return;
  }
  if (input.left) {
    HandleWorldMove(state, Facing::Left);
    return;
  }
  if (input.right) {
    HandleWorldMove(state, Facing::Right);
    return;
  }
  if (input.confirm) {
    ShowMessage(state, InteractionForFacingTile(GetMapData(state.world.map_id), state.world));
  }
}

SDL_Color TileColor(TileKind tile) {
  switch (tile) {
    case TileKind::Floor:
      return {201, 186, 150, 255};
    case TileKind::Wall:
      return {96, 72, 54, 255};
    case TileKind::Door:
      return {123, 76, 38, 255};
    case TileKind::Stairs:
      return {175, 173, 169, 255};
    case TileKind::Tv:
      return {44, 112, 145, 255};
    case TileKind::Table:
      return {138, 102, 62, 255};
  }
  return {0, 0, 0, 255};
}

void DrawMessageBox(SDL_Renderer* renderer, const GameState& state) {
  if (state.active_message == MessageId::None) {
    SDL_SetRenderDrawColor(renderer, 24, 28, 36, 255);
    const SDL_Rect info {4, kTextBoxY, 152, 42};
    SDL_RenderFillRect(renderer, &info);
    SDL_SetRenderDrawColor(renderer, 205, 214, 221, 255);
    SDL_RenderDrawRect(renderer, &info);
    DrawText(renderer,
             8,
             kTextBoxY + 6,
             "ARROWS MOVE  Z TALK\nF5 SAVE  F9 LOAD\nF6 NEXT MAP  G FLAG",
             SDL_Color {240, 240, 240, 255},
             1);
    return;
  }

  SDL_SetRenderDrawColor(renderer, 248, 248, 240, 255);
  const SDL_Rect box {4, kTextBoxY, 152, 42};
  SDL_RenderFillRect(renderer, &box);
  SDL_SetRenderDrawColor(renderer, 24, 28, 36, 255);
  SDL_RenderDrawRect(renderer, &box);
  DrawText(renderer,
           8,
           kTextBoxY + 6,
           MessageText(state.active_message, state.active_message_page),
           SDL_Color {16, 16, 16, 255},
           1);
}

void RenderTitle(SDL_Renderer* renderer, const GameState& state) {
  SDL_SetRenderDrawColor(renderer, 18, 24, 32, 255);
  SDL_RenderClear(renderer);

  DrawText(renderer, 20, 24, "POKERED NATIVE", SDL_Color {255, 255, 255, 255}, 2);
  DrawText(renderer, 28, 52, "FIRST PLAYABLE", SDL_Color {195, 219, 255, 255}, 1);
  DrawText(renderer, 21, 62, "PALLET TOWN SLICE", SDL_Color {195, 219, 255, 255}, 1);

  const SDL_Color active {255, 220, 120, 255};
  const SDL_Color inactive {190, 190, 190, 255};
  const SDL_Color disabled {100, 100, 100, 255};

  DrawText(renderer, 26, 88, state.menu_index == 0 ? "> START NEW GAME" : "  START NEW GAME",
           state.menu_index == 0 ? active : inactive, 1);
  DrawText(renderer, 26, 100, state.menu_index == 1 ? "> CONTINUE" : "  CONTINUE",
           state.has_save ? (state.menu_index == 1 ? active : inactive) : disabled, 1);

  if (state.active_message != MessageId::None) {
    DrawMessageBox(renderer, state);
  } else {
    DrawText(renderer, 26, 126, "ENTER OR Z", SDL_Color {180, 180, 180, 255}, 1);
  }
}

void RenderWorld(SDL_Renderer* renderer, const GameState& state) {
  SDL_SetRenderDrawColor(renderer, 140, 188, 216, 255);
  SDL_RenderClear(renderer);

  const MapData& map = GetMapData(state.world.map_id);
  const CameraView view = BuildCameraView(map, state.world);

  for (int screen_y = 0; screen_y < view.height; ++screen_y) {
    for (int screen_x = 0; screen_x < view.width; ++screen_x) {
      const int map_x = view.origin_x + screen_x;
      const int map_y = view.origin_y + screen_y;
      const SDL_Color color = TileColor(RenderTileKind(map, map_x, map_y));
      SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
      const SDL_Rect tile {
          view.render_x + screen_x * kTilePixels,
          view.render_y + screen_y * kTilePixels,
          kTilePixels,
          kTilePixels,
      };
      SDL_RenderFillRect(renderer, &tile);
      SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
      SDL_RenderDrawRect(renderer, &tile);
    }
  }

  for (const Npc& npc : map.npcs) {
    if (npc.x < view.origin_x || npc.x >= view.origin_x + view.width || npc.y < view.origin_y ||
        npc.y >= view.origin_y + view.height) {
      continue;
    }
    SDL_SetRenderDrawColor(renderer, 224, 136, 168, 255);
    const SDL_Rect sprite {
        view.render_x + (npc.x - view.origin_x) * kTilePixels + 2,
        view.render_y + (npc.y - view.origin_y) * kTilePixels + 1,
        kTilePixels - 4,
        kTilePixels - 2,
    };
    SDL_RenderFillRect(renderer, &sprite);
  }

  SDL_SetRenderDrawColor(renderer, 42, 76, 214, 255);
  const SDL_Rect player {
      view.render_x + (state.world.player.x - view.origin_x) * kTilePixels + 2,
      view.render_y + (state.world.player.y - view.origin_y) * kTilePixels + 1,
      kTilePixels - 4,
      kTilePixels - 2,
  };
  SDL_RenderFillRect(renderer, &player);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  const auto [dx, dy] = FacingOffset(state.world.player.facing);
  const SDL_Rect facing {
      player.x + (dx > 0 ? player.w - 3 : dx < 0 ? 0 : player.w / 2 - 1),
      player.y + (dy > 0 ? player.h - 3 : dy < 0 ? 0 : player.h / 2 - 1),
      3,
      3,
  };
  SDL_RenderFillRect(renderer, &facing);

  DrawText(renderer, 8, 4, map.name, SDL_Color {20, 20, 20, 255}, 1);
  if (state.world.got_starter) {
    DrawText(renderer, 92, 4, "STARTER FLAG", SDL_Color {20, 20, 20, 255}, 1);
  }

  DrawMessageBox(renderer, state);
}

int RunSmokeTest() {
  GameState state {};
  StartNewGame(state);
  if (state.scene != SceneId::World) {
    std::cerr << "smoke: failed to enter world\n";
    return 1;
  }

  if (!TryMove(state.world, Facing::Down)) {
    std::cerr << "smoke: failed to exit RedsHouse1F\n";
    return 1;
  }
  if (state.world.map_id != WorldId::PalletTown || state.world.player.x != 5 || state.world.player.y != 5 ||
      state.world.player.facing != Facing::Down ||
      state.world.last_map != static_cast<std::uint16_t>(WorldId::RedsHouse1F) || state.world.last_warp != 2) {
    std::cerr << "smoke: expected door warp into PalletTown\n";
    return 1;
  }

  if (!TryMove(state.world, Facing::Down) || !TryMove(state.world, Facing::Down) || !TryMove(state.world, Facing::Down) ||
      !TryMove(state.world, Facing::Left)) {
    std::cerr << "smoke: failed to move through PalletTown\n";
    return 1;
  }
  if (state.world.map_id != WorldId::PalletTown || state.world.player.x != 4 || state.world.player.y != 8 ||
      state.world.player.facing != Facing::Left || state.world.step_counter != 5) {
    std::cerr << "smoke: expected outdoor movement state in PalletTown\n";
    return 1;
  }
  if (InteractionForFacingTile(GetMapData(state.world.map_id), state.world) != MessageId::PalletTownGirl) {
    std::cerr << "smoke: expected PalletTown girl interaction\n";
    return 1;
  }

  const int north_exit_x = FindPalletTownNorthExitX(GetMapData(state.world.map_id));
  if (north_exit_x < 0) {
    std::cerr << "smoke: failed to find PalletTown north exit seam\n";
    return 1;
  }
  state.world.player = PlayerState {north_exit_x, 2, Facing::Up};
  const MoveResult oak_warning = TryMoveWithResult(state.world, Facing::Up);
  if (oak_warning.moved || oak_warning.message != MessageId::PalletTownOakHeyWaitDontGoOut ||
      state.world.player.x != north_exit_x || state.world.player.y != 2 || state.world.step_counter != 5) {
    std::cerr << "smoke: expected PalletTown Oak north-exit warning\n";
    return 1;
  }

  state.world.player = PlayerState {12, 12, Facing::Up};
  if (!TryMove(state.world, Facing::Up) || state.world.map_id != WorldId::OaksLab ||
      state.world.player.x != 5 || state.world.player.y != 11 || state.world.player.facing != Facing::Down ||
      state.world.last_map != static_cast<std::uint16_t>(WorldId::PalletTown) || state.world.last_warp != 3) {
    std::cerr << "smoke: expected PalletTown door warp into OaksLab\n";
    return 1;
  }

  state.world.player = PlayerState {2, 2, Facing::Up};
  if (InteractionForFacingTile(GetMapData(state.world.map_id), state.world) != MessageId::OaksLabPokedex) {
    std::cerr << "smoke: expected OaksLab pokedex interaction\n";
    return 1;
  }

  const std::filesystem::path smoke_path = TempRuntimePath("pokered_native_smoke.bin");
  if (!SaveSystem::Save(smoke_path, state)) {
    std::cerr << "smoke: failed to save\n";
    return 1;
  }

  const LoadResult loaded = SaveSystem::Load(smoke_path);
  if (loaded.status != LoadStatus::Ok) {
    std::cerr << "smoke: failed to load\n";
    return 1;
  }
  if (loaded.state.world.map_id != state.world.map_id || loaded.state.world.player.x != state.world.player.x ||
      loaded.state.world.player.y != state.world.player.y ||
      loaded.state.world.player.facing != state.world.player.facing ||
      loaded.state.world.last_map != state.world.last_map || loaded.state.world.last_warp != state.world.last_warp ||
      loaded.state.world.step_counter != state.world.step_counter) {
    std::cerr << "smoke: save/load mismatch\n";
    return 1;
  }

  std::cout << "smoke-ok: world=" << static_cast<int>(loaded.state.world.map_id)
            << " pos=" << loaded.state.world.player.x << "," << loaded.state.world.player.y
            << " steps=" << loaded.state.world.step_counter << "\n";
  return 0;
}

}  // namespace

int Application::Run(int argc, char** argv) const {
  for (int i = 1; i < argc; ++i) {
    if (std::string_view(argv[i]) == "--smoke-test") {
      return RunSmokeTest();
    }
  }

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
    return 1;
  }

  SDL_Window* window = SDL_CreateWindow("pokered native",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        kLogicalWidth * kWindowScale,
                                        kLogicalHeight * kWindowScale,
                                        SDL_WINDOW_SHOWN);
  if (window == nullptr) {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << "\n";
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == nullptr) {
    std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << "\n";
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_RenderSetLogicalSize(renderer, kLogicalWidth, kLogicalHeight);

  GameState state {};
  RefreshSaveAvailability(state);

  while (state.running) {
    const FrameInput input = PollInput(state);
    if (state.active_message != MessageId::None && state.scene == SceneId::Title && (input.confirm || input.back)) {
      AdvanceOrDismissMessage(state);
    } else if (state.scene == SceneId::Title) {
      UpdateTitle(state, input);
    } else {
      UpdateWorld(state, input);
    }

    if (state.scene == SceneId::Title) {
      RenderTitle(renderer, state);
    } else {
      RenderWorld(renderer, state);
    }

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

}  // namespace pokered
