#include "pokered/oracle/provenance.hpp"

#include <optional>
#include <string_view>
#include <utility>

namespace pokered::oracle {
namespace {

std::optional<std::pair<std::string_view, std::string_view>> LabelsForWorld(WorldId world_id) {
  switch (world_id) {
    case WorldId::RedsHouse1F:
      return std::pair {"RedsHouse1F_h", "RedsHouse1F_Object"};
    case WorldId::RedsHouse2F:
      return std::pair {"RedsHouse2F_h", "RedsHouse2F_Object"};
    case WorldId::PewterSpeechHouse:
      return std::pair {"PewterSpeechHouse_h", "PewterSpeechHouse_Object"};
    case WorldId::PalletTown:
      return std::pair {"PalletTown_h", "PalletTown_Object"};
    case WorldId::BluesHouse:
      return std::pair {"BluesHouse_h", "BluesHouse_Object"};
    case WorldId::OaksLab:
      return std::pair {"OaksLab_h", "OaksLab_Object"};
  }
  return std::nullopt;
}

std::optional<ProvenanceSymbol> LookupSymbolProvenance(const SymbolTable& symbols,
                                                       const MapSections& sections,
                                                       std::string_view label) {
  const auto found = symbols.find(std::string(label));
  if (found == symbols.end()) {
    return std::nullopt;
  }

  return ProvenanceSymbol {
      .label = std::string(label),
      .address = found->second,
      .section = MapFile::Lookup(sections, found->second.bank, found->second.address),
  };
}

}  // namespace

std::optional<MapProvenance> LookupMapProvenance(const SymbolTable& symbols,
                                                 const MapSections& sections,
                                                 WorldId world_id) {
  const auto labels = LabelsForWorld(world_id);
  if (!labels) {
    return std::nullopt;
  }

  const auto header = LookupSymbolProvenance(symbols, sections, labels->first);
  const auto object = LookupSymbolProvenance(symbols, sections, labels->second);
  if (!header || !object) {
    return std::nullopt;
  }

  return MapProvenance {
      .world_id = world_id,
      .header = *header,
      .object = *object,
  };
}

std::optional<MapProvenance> LookupMapProvenance(const std::filesystem::path& sym_path,
                                                 const std::filesystem::path& map_path,
                                                 WorldId world_id) {
  return LookupMapProvenance(SymbolFile::Load(sym_path), MapFile::Load(map_path), world_id);
}

std::optional<WarpProvenance> LookupWarpProvenance(const SymbolTable& symbols,
                                                   const MapSections& sections,
                                                   WorldId source_world_id,
                                                   std::uint8_t source_warp,
                                                   WorldId target_world_id,
                                                   std::uint8_t target_warp) {
  if (source_warp == 0 || target_warp == 0) {
    return std::nullopt;
  }

  const auto source = LookupMapProvenance(symbols, sections, source_world_id);
  const auto target = LookupMapProvenance(symbols, sections, target_world_id);
  if (!source || !target) {
    return std::nullopt;
  }

  return WarpProvenance {
      .source_world_id = source_world_id,
      .source_warp = source_warp,
      .source_object = source->object,
      .target_world_id = target_world_id,
      .target_warp = target_warp,
      .target_object = target->object,
  };
}

}  // namespace pokered::oracle
