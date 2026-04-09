#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace pokered::oracle {

struct SymbolAddress {
  std::uint16_t bank = 0;
  std::uint16_t address = 0;
};

using SymbolTable = std::unordered_map<std::string, SymbolAddress>;

class SymbolFile {
public:
  static SymbolTable Load(const std::filesystem::path& path);
  static std::optional<SymbolAddress> Lookup(const std::filesystem::path& path, std::string_view name);
};

}  // namespace pokered::oracle
