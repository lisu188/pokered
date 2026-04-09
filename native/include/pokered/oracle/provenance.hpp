#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include "pokered/core/game_state.hpp"
#include "pokered/oracle/map_file.hpp"
#include "pokered/oracle/symbol_file.hpp"

namespace pokered::oracle {

struct ProvenanceSymbol {
  std::string label;
  SymbolAddress address {};
  std::optional<MapSection> section;
};

struct MapProvenance {
  WorldId world_id = WorldId::RedsHouse1F;
  ProvenanceSymbol header;
  ProvenanceSymbol object;
};

std::optional<MapProvenance> LookupMapProvenance(const SymbolTable& symbols,
                                                 const MapSections& sections,
                                                 WorldId world_id);
std::optional<MapProvenance> LookupMapProvenance(const std::filesystem::path& sym_path,
                                                 const std::filesystem::path& map_path,
                                                 WorldId world_id);

}  // namespace pokered::oracle
