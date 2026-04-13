#pragma once

#include <filesystem>

#include "pokered/core/game_state.hpp"

namespace pokered {

enum class LoadStatus {
  Ok = 0,
  Missing,
  Corrupt,
  IoError,
};

struct LoadResult {
  LoadStatus status = LoadStatus::Missing;
  GameState state {};
};

class SaveSystem {
public:
  static bool Exists(const std::filesystem::path& path);
  static bool Save(const std::filesystem::path& path, const GameState& state);
  static LoadResult Load(const std::filesystem::path& path);
};

}  // namespace pokered
