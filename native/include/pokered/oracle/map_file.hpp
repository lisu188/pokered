#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace pokered::oracle {

struct MapSection {
  std::string memory_region;
  std::uint16_t bank = 0;
  std::uint16_t start = 0;
  std::uint16_t end = 0;
  std::string name;
};

using MapSections = std::vector<MapSection>;

class MapFile {
public:
  static MapSections Load(const std::filesystem::path& path);
  static std::optional<MapSection> Lookup(const MapSections& sections, std::uint16_t bank, std::uint16_t address);
  static std::optional<MapSection> Lookup(const std::filesystem::path& path, std::uint16_t bank, std::uint16_t address);
};

}  // namespace pokered::oracle
