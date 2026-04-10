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

struct WarpProvenance {
  WorldId source_world_id = WorldId::RedsHouse1F;
  std::uint8_t source_warp = 0;
  ProvenanceSymbol source_object;
  WorldId target_world_id = WorldId::RedsHouse1F;
  std::uint8_t target_warp = 0;
  ProvenanceSymbol target_object;
};

struct LastMapProvenance {
  WorldId world_id = WorldId::RedsHouse1F;
  std::uint8_t warp_id = 0;
  ProvenanceSymbol object;
};

struct MessageProvenance {
  MessageId message_id = MessageId::None;
  ProvenanceSymbol text;
};

struct MessageSourceProvenance {
  MessageId message_id = MessageId::None;
  ProvenanceSymbol source;
};

struct MoveScriptProvenance {
  WorldId world_id = WorldId::RedsHouse1F;
  MoveBlocker blocker = MoveBlocker::None;
  MessageId message_id = MessageId::None;
  ProvenanceSymbol script;
};

struct InteractionProvenance {
  WorldId world_id = WorldId::RedsHouse1F;
  MessageId origin_message = MessageId::None;
  MessageId message_id = MessageId::None;
  ProvenanceSymbol object;
  ProvenanceSymbol source;
};

struct InteractionBranchProvenance {
  WorldId world_id = WorldId::RedsHouse1F;
  MessageId origin_message = MessageId::None;
  MessageId message_id = MessageId::None;
  ProvenanceSymbol branch;
};

std::optional<MapProvenance> LookupMapProvenance(const SymbolTable& symbols,
                                                 const MapSections& sections,
                                                 WorldId world_id);
std::optional<MapProvenance> LookupMapProvenance(const std::filesystem::path& sym_path,
                                                 const std::filesystem::path& map_path,
                                                 WorldId world_id);
std::optional<WarpProvenance> LookupWarpProvenance(const SymbolTable& symbols,
                                                   const MapSections& sections,
                                                   WorldId source_world_id,
                                                   std::uint8_t source_warp,
                                                   WorldId target_world_id,
                                                   std::uint8_t target_warp);
std::optional<LastMapProvenance> LookupLastMapProvenance(const SymbolTable& symbols,
                                                         const MapSections& sections,
                                                         WorldId world_id,
                                                         std::uint8_t warp_id);
std::optional<MessageProvenance> LookupMessageProvenance(const SymbolTable& symbols,
                                                         const MapSections& sections,
                                                         MessageId message_id);
std::optional<MessageSourceProvenance> LookupMessageSourceProvenance(const SymbolTable& symbols,
                                                                     const MapSections& sections,
                                                                     MessageId message_id);
std::optional<MoveScriptProvenance> LookupMoveScriptProvenance(const SymbolTable& symbols,
                                                               const MapSections& sections,
                                                               WorldId world_id,
                                                               MoveBlocker blocker,
                                                               MessageId message_id);
std::optional<InteractionProvenance> LookupInteractionProvenance(const SymbolTable& symbols,
                                                                 const MapSections& sections,
                                                                 WorldId world_id,
                                                                 MessageId origin_message,
                                                                 MessageId message_id);
std::optional<InteractionBranchProvenance> LookupInteractionBranchProvenance(const SymbolTable& symbols,
                                                                             const MapSections& sections,
                                                                             WorldId world_id,
                                                                             MessageId origin_message,
                                                                             MessageId message_id);

}  // namespace pokered::oracle
