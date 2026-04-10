#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string_view>
#include <vector>

#include "pokered/core/game_state.hpp"
#include "pokered/oracle/map_file.hpp"
#include "pokered/oracle/provenance.hpp"
#include "pokered/oracle/symbol_file.hpp"
#include "pokered/save/save_system.hpp"
#include "pokered/world/map_data.hpp"

namespace {

std::vector<unsigned char> ReadBytes(const std::filesystem::path& path) {
  std::ifstream input(path, std::ios::binary);
  return std::vector<unsigned char>((std::istreambuf_iterator<char>(input)),
                                    std::istreambuf_iterator<char>());
}

void WriteBytes(const std::filesystem::path& path, const std::vector<unsigned char>& bytes) {
  std::ofstream output(path, std::ios::binary | std::ios::trunc);
  output.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
}

bool ExpectSectionForSymbol(const pokered::oracle::SymbolTable& symbols,
                            const pokered::oracle::MapSections& sections,
                            const char* symbol_name,
                            std::uint16_t expected_bank,
                            std::uint16_t expected_address,
                            std::string_view expected_region,
                            std::string_view expected_section) {
  const auto symbol = symbols.find(symbol_name);
  if (symbol == symbols.end() || symbol->second.bank != expected_bank || symbol->second.address != expected_address) {
    return false;
  }

  const auto section = pokered::oracle::MapFile::Lookup(sections, symbol->second.bank, symbol->second.address);
  return section && section->memory_region == expected_region && section->name == expected_section;
}

std::filesystem::path OraclePath(std::string_view filename) {
#ifdef POKERED_NATIVE_SOURCE_DIR
  return std::filesystem::path(POKERED_NATIVE_SOURCE_DIR) / std::filesystem::path(filename);
#else
  return std::filesystem::path(filename);
#endif
}

}  // namespace

int main() {
  const pokered::oracle::SymbolTable symbols = pokered::oracle::SymbolFile::Load(OraclePath("pokered.sym"));
  const pokered::oracle::MapSections sections = pokered::oracle::MapFile::Load(OraclePath("pokered.map"));
  const auto enter_map = symbols.find("EnterMap");
  const auto overworld_loop = symbols.find("OverworldLoop");
  const auto load_save = symbols.find("TryLoadSaveFile");
  if (enter_map == symbols.end() || enter_map->second.bank != 0x00 || enter_map->second.address != 0x03A6 ||
      overworld_loop == symbols.end() || overworld_loop->second.bank != 0x00 ||
      overworld_loop->second.address != 0x03FF || load_save == symbols.end() ||
      load_save->second.bank != 0x1C || load_save->second.address != 0x75E8) {
    std::cerr << "expected key oracle symbols in pokered.sym\n";
    return 1;
  }
  if (sections.empty()) {
    std::cerr << "expected parsed pokered.map sections\n";
    return 1;
  }
  if (!ExpectSectionForSymbol(symbols, sections, "EnterMap", 0x00, 0x03A6, "ROM0", "Home") ||
      !ExpectSectionForSymbol(symbols, sections, "OverworldLoop", 0x00, 0x03FF, "ROM0", "Home") ||
      !ExpectSectionForSymbol(symbols, sections, "PalletTown_h", 0x06, 0x42A1, "ROMX", "Maps 1") ||
      !ExpectSectionForSymbol(symbols, sections, "PalletTown_Object", 0x06, 0x42C3, "ROMX", "Maps 1") ||
      !ExpectSectionForSymbol(symbols, sections, "RedsHouse1F_h", 0x12, 0x415C, "ROMX", "Maps 8") ||
      !ExpectSectionForSymbol(symbols, sections, "RedsHouse1F_Object", 0x12, 0x41E4, "ROMX", "Maps 8") ||
      !ExpectSectionForSymbol(symbols, sections, "RedsHouse2F_h", 0x17, 0x40A4, "ROMX", "Maps 15") ||
      !ExpectSectionForSymbol(symbols, sections, "RedsHouse2F_Object", 0x17, 0x40D0, "ROMX", "Maps 15") ||
      !ExpectSectionForSymbol(symbols, sections, "OaksLab_h", 0x07, 0x4B02, "ROMX", "Maps 4") ||
      !ExpectSectionForSymbol(symbols, sections, "OaksLab_Object", 0x07, 0x540A, "ROMX", "Maps 4") ||
      !ExpectSectionForSymbol(symbols, sections, "PewterSpeechHouse_h", 0x07, 0x563C, "ROMX", "Maps 4") ||
      !ExpectSectionForSymbol(symbols, sections, "PewterSpeechHouse_Object", 0x07, 0x5659, "ROMX", "Maps 4") ||
      !ExpectSectionForSymbol(symbols, sections, "TryLoadSaveFile", 0x1C, 0x75E8, "ROMX", "bank1C")) {
    std::cerr << "expected existing key oracle sections in pokered.map\n";
    return 1;
  }
  if (!ExpectSectionForSymbol(symbols, sections, "BluesHouse_h", 0x06, 0x5B2F, "ROMX", "Maps 2")) {
    std::cerr << "expected BluesHouse_h section ownership in pokered.map\n";
    return 1;
  }
  if (!ExpectSectionForSymbol(symbols, sections, "BluesHouse_Object", 0x06, 0x5BCE, "ROMX", "Maps 2")) {
    std::cerr << "expected BluesHouse_Object section ownership in pokered.map\n";
    return 1;
  }
  const auto pallet_provenance = pokered::oracle::LookupMapProvenance(symbols, sections, pokered::WorldId::PalletTown);
  if (!pallet_provenance || pallet_provenance->header.label != "PalletTown_h" ||
      pallet_provenance->header.address.bank != 0x06 || pallet_provenance->header.address.address != 0x42A1 ||
      !pallet_provenance->header.section || pallet_provenance->header.section->name != "Maps 1" ||
      pallet_provenance->object.label != "PalletTown_Object" ||
      pallet_provenance->object.address.bank != 0x06 || pallet_provenance->object.address.address != 0x42C3 ||
      !pallet_provenance->object.section || pallet_provenance->object.section->name != "Maps 1") {
    std::cerr << "expected PalletTown provenance lookup\n";
    return 1;
  }
  const auto oaks_provenance = pokered::oracle::LookupMapProvenance(symbols, sections, pokered::WorldId::OaksLab);
  if (!oaks_provenance || oaks_provenance->header.label != "OaksLab_h" ||
      oaks_provenance->header.address.bank != 0x07 || oaks_provenance->header.address.address != 0x4B02 ||
      !oaks_provenance->header.section || oaks_provenance->header.section->name != "Maps 4" ||
      oaks_provenance->object.label != "OaksLab_Object" ||
      oaks_provenance->object.address.bank != 0x07 || oaks_provenance->object.address.address != 0x540A ||
      !oaks_provenance->object.section || oaks_provenance->object.section->name != "Maps 4") {
    std::cerr << "expected OaksLab provenance lookup\n";
    return 1;
  }
  const auto oaks_warp_provenance = pokered::oracle::LookupWarpProvenance(
      symbols, sections, pokered::WorldId::PalletTown, 3, pokered::WorldId::OaksLab, 2);
  if (!oaks_warp_provenance || oaks_warp_provenance->source_world_id != pokered::WorldId::PalletTown ||
      oaks_warp_provenance->source_warp != 3 ||
      oaks_warp_provenance->source_object.label != "PalletTown_Object" ||
      oaks_warp_provenance->source_object.address.bank != 0x06 ||
      oaks_warp_provenance->source_object.address.address != 0x42C3 ||
      !oaks_warp_provenance->source_object.section || oaks_warp_provenance->source_object.section->name != "Maps 1" ||
      oaks_warp_provenance->target_world_id != pokered::WorldId::OaksLab || oaks_warp_provenance->target_warp != 2 ||
      oaks_warp_provenance->target_object.label != "OaksLab_Object" ||
      oaks_warp_provenance->target_object.address.bank != 0x07 ||
      oaks_warp_provenance->target_object.address.address != 0x540A ||
      !oaks_warp_provenance->target_object.section || oaks_warp_provenance->target_object.section->name != "Maps 4") {
    std::cerr << "expected PalletTown -> OaksLab warp provenance lookup\n";
    return 1;
  }
  const auto house_warp_provenance = pokered::oracle::LookupWarpProvenance(
      symbols, sections, pokered::WorldId::RedsHouse1F, 2, pokered::WorldId::PalletTown, 1);
  if (!house_warp_provenance || house_warp_provenance->source_world_id != pokered::WorldId::RedsHouse1F ||
      house_warp_provenance->source_warp != 2 ||
      house_warp_provenance->source_object.label != "RedsHouse1F_Object" ||
      house_warp_provenance->source_object.address.bank != 0x12 ||
      house_warp_provenance->source_object.address.address != 0x41E4 ||
      !house_warp_provenance->source_object.section || house_warp_provenance->source_object.section->name != "Maps 8" ||
      house_warp_provenance->target_world_id != pokered::WorldId::PalletTown ||
      house_warp_provenance->target_warp != 1 ||
      house_warp_provenance->target_object.label != "PalletTown_Object" ||
      house_warp_provenance->target_object.address.bank != 0x06 ||
      house_warp_provenance->target_object.address.address != 0x42C3 ||
      !house_warp_provenance->target_object.section || house_warp_provenance->target_object.section->name != "Maps 1") {
    std::cerr << "expected RedsHouse1F -> PalletTown warp provenance lookup\n";
    return 1;
  }
  const auto mom_provenance = pokered::oracle::LookupMessageProvenance(
      symbols, sections, pokered::MessageId::MomWakeUp);
  const bool mom_ok = mom_provenance && mom_provenance->message_id == pokered::MessageId::MomWakeUp &&
                      mom_provenance->text.label == "_RedsHouse1FMomWakeUpText" &&
                      mom_provenance->text.address.bank == 0x25 &&
                      mom_provenance->text.address.address == 0x4B07 && mom_provenance->text.section &&
                      mom_provenance->text.section->name == "Text 6";
  if (!mom_ok) {
    std::cerr << "expected Mom wake-up message provenance lookup";
    if (mom_provenance) {
      std::cerr << " got id=" << static_cast<int>(mom_provenance->message_id)
                << " label=" << mom_provenance->text.label
                << " bank=" << mom_provenance->text.address.bank
                << " address=" << std::hex << mom_provenance->text.address.address << std::dec
                << " section="
                << (mom_provenance->text.section ? mom_provenance->text.section->name : std::string("<none>"));
    } else {
      std::cerr << " got <none>";
    }
    std::cerr << "\n";
    return 1;
  }
  const auto oak_warning_provenance = pokered::oracle::LookupMessageProvenance(
      symbols, sections, pokered::MessageId::PalletTownOakHeyWaitDontGoOut);
  const bool oak_warning_ok =
      oak_warning_provenance && oak_warning_provenance->text.label == "_PalletTownOakHeyWaitDontGoOutText" &&
      oak_warning_provenance->text.address.bank == 0x29 &&
      oak_warning_provenance->text.address.address == 0x4245 && oak_warning_provenance->text.section &&
      oak_warning_provenance->text.section->name == "Text 10";
  if (!oak_warning_ok) {
    std::cerr << "expected PalletTown Oak warning message provenance lookup";
    if (oak_warning_provenance) {
      std::cerr << " got label=" << oak_warning_provenance->text.label
                << " bank=" << oak_warning_provenance->text.address.bank
                << " address=" << std::hex << oak_warning_provenance->text.address.address << std::dec
                << " section="
                << (oak_warning_provenance->text.section ? oak_warning_provenance->text.section->name
                                                         : std::string("<none>"));
    } else {
      std::cerr << " got <none>";
    }
    std::cerr << "\n";
    return 1;
  }
  const auto pokedex_provenance = pokered::oracle::LookupMessageProvenance(
      symbols, sections, pokered::MessageId::OaksLabPokedex);
  if (!pokedex_provenance || pokedex_provenance->text.label != "_OaksLabPokedexText" ||
      pokedex_provenance->text.address.bank != 0x25 || pokedex_provenance->text.address.address != 0x5236 ||
      !pokedex_provenance->text.section || pokedex_provenance->text.section->name != "Text 6") {
    std::cerr << "expected OaksLab pokedex message provenance lookup\n";
    return 1;
  }
  const auto mom_source_provenance = pokered::oracle::LookupMessageSourceProvenance(
      symbols, sections, pokered::MessageId::MomWakeUp);
  if (!mom_source_provenance || mom_source_provenance->source.label != "RedsHouse1FMomText.WakeUpText" ||
      mom_source_provenance->source.address.bank != 0x12 ||
      mom_source_provenance->source.address.address != 0x4185 || !mom_source_provenance->source.section ||
      mom_source_provenance->source.section->name != "Maps 8") {
    std::cerr << "expected Mom wake-up source provenance lookup\n";
    return 1;
  }
  const auto oak_warning_source_provenance = pokered::oracle::LookupMessageSourceProvenance(
      symbols, sections, pokered::MessageId::PalletTownOakHeyWaitDontGoOut);
  if (!oak_warning_source_provenance ||
      oak_warning_source_provenance->source.label != "PalletTownOakText.HeyWaitDontGoOutText" ||
      oak_warning_source_provenance->source.address.bank != 0x06 ||
      oak_warning_source_provenance->source.address.address != 0x4FB0 ||
      !oak_warning_source_provenance->source.section ||
      oak_warning_source_provenance->source.section->name != "Maps 2") {
    std::cerr << "expected PalletTown Oak warning source provenance lookup\n";
    return 1;
  }
  const auto rival_source_provenance = pokered::oracle::LookupMessageSourceProvenance(
      symbols, sections, pokered::MessageId::OaksLabRivalGrampsIsntAround);
  if (!rival_source_provenance ||
      rival_source_provenance->source.label != "OaksLabRivalText.GrampsIsntAroundText" ||
      rival_source_provenance->source.address.bank != 0x07 ||
      rival_source_provenance->source.address.address != 0x50F3 || !rival_source_provenance->source.section ||
      rival_source_provenance->source.section->name != "Maps 4") {
    std::cerr << "expected OaksLab rival source provenance lookup\n";
    return 1;
  }
  const auto pokedex_source_provenance = pokered::oracle::LookupMessageSourceProvenance(
      symbols, sections, pokered::MessageId::OaksLabPokedex);
  if (!pokedex_source_provenance || pokedex_source_provenance->source.label != "OaksLabPokedexText" ||
      pokedex_source_provenance->source.address.bank != 0x07 ||
      pokedex_source_provenance->source.address.address != 0x5322 || !pokedex_source_provenance->source.section ||
      pokedex_source_provenance->source.section->name != "Maps 4") {
    std::cerr << "expected OaksLab pokedex source provenance lookup\n";
    return 1;
  }
  const auto oak_move_script_provenance = pokered::oracle::LookupMoveScriptProvenance(
      symbols,
      sections,
      pokered::WorldId::PalletTown,
      pokered::MoveBlocker::Script,
      pokered::MessageId::PalletTownOakHeyWaitDontGoOut);
  if (!oak_move_script_provenance || oak_move_script_provenance->script.label != "PalletTownDefaultScript" ||
      oak_move_script_provenance->script.address.bank != 0x06 ||
      oak_move_script_provenance->script.address.address != 0x4E81 ||
      !oak_move_script_provenance->script.section || oak_move_script_provenance->script.section->name != "Maps 2") {
    std::cerr << "expected PalletTown move-script provenance lookup\n";
    return 1;
  }
  if (pokered::oracle::LookupMessageProvenance(symbols, sections, pokered::MessageId::SaveOk) ||
      pokered::oracle::LookupMessageProvenance(symbols, sections, pokered::MessageId::OaksLabRival) ||
      pokered::oracle::LookupMessageSourceProvenance(symbols, sections, pokered::MessageId::SaveOk) ||
      pokered::oracle::LookupMessageSourceProvenance(symbols, sections, pokered::MessageId::OaksLabRival) ||
      pokered::oracle::LookupMoveScriptProvenance(
          symbols, sections, pokered::WorldId::PalletTown, pokered::MoveBlocker::Collision, pokered::MessageId::None) ||
      pokered::oracle::LookupMoveScriptProvenance(
          symbols, sections, pokered::WorldId::OaksLab, pokered::MoveBlocker::Script, pokered::MessageId::OaksLabPokedex)) {
    std::cerr << "expected native-only and abstract message ids to have no oracle provenance\n";
    return 1;
  }

  const pokered::MapData& map = pokered::GetMapData(pokered::WorldId::RedsHouse1F);
  const pokered::MapData& speech_house = pokered::GetMapData(pokered::WorldId::PewterSpeechHouse);
  const pokered::MapData& blues_house = pokered::GetMapData(pokered::WorldId::BluesHouse);
  const pokered::MapData& oaks_lab = pokered::GetMapData(pokered::WorldId::OaksLab);
  const pokered::MapData& pallet_town = pokered::GetMapData(pokered::WorldId::PalletTown);
  if (map.width != 8 || map.height != 8 || map.block_ids.size() != 16 || map.cells.size() != 64 ||
      map.warps.size() != 3 || map.bg_events.size() != 1 || map.npcs.size() != 1) {
    std::cerr << "map import shape mismatch\n";
    return 1;
  }
  const pokered::MapData& upstairs = pokered::GetMapData(pokered::WorldId::RedsHouse2F);
  if (upstairs.width != 8 || upstairs.height != 8 || upstairs.block_ids.size() != 16 || upstairs.cells.size() != 64 ||
      upstairs.warps.size() != 1 || !upstairs.bg_events.empty() || !upstairs.npcs.empty()) {
    std::cerr << "RedsHouse2F import shape mismatch\n";
    return 1;
  }
  if (speech_house.width != 8 || speech_house.height != 8 || speech_house.block_ids.size() != 16 ||
      speech_house.cells.size() != 64 || speech_house.warps.size() != 2 || !speech_house.bg_events.empty() ||
      speech_house.npcs.size() != 2) {
    std::cerr << "PewterSpeechHouse import shape mismatch\n";
    return 1;
  }
  if (blues_house.width != 8 || blues_house.height != 8 || blues_house.block_ids.size() != 16 ||
      blues_house.cells.size() != 64 || blues_house.warps.size() != 2 || !blues_house.bg_events.empty() ||
      blues_house.npcs.size() != 3) {
    std::cerr << "BluesHouse import shape mismatch\n";
    return 1;
  }
  if (oaks_lab.width != 10 || oaks_lab.height != 12 || oaks_lab.block_ids.size() != 30 ||
      oaks_lab.cells.size() != 120 || oaks_lab.warps.size() != 2 || !oaks_lab.bg_events.empty() ||
      oaks_lab.npcs.size() != 11) {
    std::cerr << "OaksLab import shape mismatch\n";
    return 1;
  }
  if (pallet_town.width != 20 || pallet_town.height != 18 || pallet_town.tileset_id != 0 ||
      pallet_town.border_block != 0x0B || pallet_town.block_ids.size() != 90 || pallet_town.cells.size() != 360 ||
      pallet_town.warps.size() != 3 || pallet_town.bg_events.size() != 4 || pallet_town.npcs.size() != 3) {
    std::cerr << "PalletTown import shape mismatch\n";
    return 1;
  }
  int pallet_north_exit_x = -1;
  for (int x = 0; x < pallet_town.width; ++x) {
    if (pokered::CanMoveTo(pallet_town, x, 2) && pokered::CanMoveTo(pallet_town, x, 1)) {
      pallet_north_exit_x = x;
      break;
    }
  }
  if (pallet_north_exit_x < 0) {
    std::cerr << "expected passable PalletTown north exit seam\n";
    return 1;
  }
  if (pokered::GetCell(map, 3, 1).behavior_tile != 0x16 || pokered::GetCell(map, 3, 1).passable) {
    std::cerr << "expected TV cell to import as solid tile 0x16\n";
    return 1;
  }
  if (pokered::RenderTileKind(map, 3, 1) != pokered::TileKind::Tv) {
    std::cerr << "expected TV interaction cell to render as a TV\n";
    return 1;
  }
  if (pokered::GetCell(map, 2, 7).behavior_tile != 0x14 || !pokered::GetCell(map, 2, 7).passable) {
    std::cerr << "expected door cell to import as passable tile 0x14\n";
    return 1;
  }
  if (pokered::MessagePageCount(pokered::MessageId::MomWakeUp) != 2 ||
      pokered::MessageText(pokered::MessageId::TvMovie, 1) != "I better go too.") {
    std::cerr << "expected source-backed paged room text import\n";
    return 1;
  }
  if (pokered::GetCell(map, 7, 1).behavior_tile != 0x1C ||
      pokered::RenderTileKind(map, 7, 1) != pokered::TileKind::Stairs) {
    std::cerr << "expected upstairs warp to import as tile 0x1c\n";
    return 1;
  }
  if (pokered::GetCell(upstairs, 7, 1).behavior_tile != 0x1A ||
      !pokered::GetCell(upstairs, 7, 1).passable ||
      pokered::RenderTileKind(upstairs, 7, 1) != pokered::TileKind::Stairs ||
      upstairs.warps.front().target_map != pokered::WorldId::RedsHouse1F ||
      upstairs.warps.front().target_warp != 3) {
    std::cerr << "expected RedsHouse2F to retain the stair warp back to RedsHouse1F\n";
    return 1;
  }
  if (pokered::GetCell(map, 3, 4).behavior_tile != 0x36 ||
      pokered::RenderTileKind(map, 3, 4) != pokered::TileKind::Table) {
    std::cerr << "expected central table tile to retain table rendering semantics\n";
    return 1;
  }
  if (!speech_house.warps.front().uses_last_map || speech_house.warps.front().target_warp != 6 ||
      !speech_house.warps.back().uses_last_map || speech_house.warps.back().target_warp != 6 ||
      pokered::RenderTileKind(speech_house, 2, 7) != pokered::TileKind::Door) {
    std::cerr << "expected PewterSpeechHouse to retain its deferred LAST_MAP door warps\n";
    return 1;
  }
  if (!blues_house.warps.front().uses_last_map || blues_house.warps.front().target_warp != 2 ||
      !blues_house.warps.back().uses_last_map || blues_house.warps.back().target_warp != 2 ||
      pokered::RenderTileKind(blues_house, 2, 7) != pokered::TileKind::Door ||
      pokered::CanMoveTo(blues_house, 2, 3) || pokered::CanMoveTo(blues_house, 6, 4) ||
      pokered::CanMoveTo(blues_house, 3, 3)) {
    std::cerr << "expected BluesHouse door and object metadata\n";
    return 1;
  }
  if (!oaks_lab.warps.front().uses_last_map || oaks_lab.warps.front().target_warp != 3 ||
      !oaks_lab.warps.back().uses_last_map || oaks_lab.warps.back().target_warp != 3 ||
      pokered::RenderTileKind(oaks_lab, 4, 11) != pokered::TileKind::Door ||
      pokered::RenderTileKind(oaks_lab, 5, 11) != pokered::TileKind::Door ||
      pokered::CanMoveTo(oaks_lab, 4, 3) || pokered::CanMoveTo(oaks_lab, 6, 3) ||
      pokered::CanMoveTo(oaks_lab, 2, 1) || pokered::CanMoveTo(oaks_lab, 1, 9)) {
    std::cerr << "expected OaksLab door and object metadata\n";
    return 1;
  }
  if (pallet_town.warps[0].x != 5 || pallet_town.warps[0].y != 5 ||
      pallet_town.warps[0].target_map != pokered::WorldId::RedsHouse1F || pallet_town.warps[0].target_warp != 1 ||
      pallet_town.warps[1].x != 13 || pallet_town.warps[1].y != 5 ||
      pallet_town.warps[1].target_map != pokered::WorldId::BluesHouse || pallet_town.warps[1].target_warp != 1 ||
      pallet_town.warps[2].x != 12 || pallet_town.warps[2].y != 11 ||
      pallet_town.warps[2].target_map != pokered::WorldId::OaksLab || pallet_town.warps[2].target_warp != 2 ||
      pokered::RenderTileKind(pallet_town, 5, 5) != pokered::TileKind::Door ||
      pokered::RenderTileKind(pallet_town, 13, 5) != pokered::TileKind::Door ||
      pokered::RenderTileKind(pallet_town, 12, 11) != pokered::TileKind::Door) {
    std::cerr << "expected PalletTown door warp metadata\n";
    return 1;
  }
  if (pokered::CanMoveTo(pallet_town, 8, 5) || pokered::CanMoveTo(pallet_town, 3, 8) ||
      pokered::CanMoveTo(pallet_town, 11, 14)) {
    std::cerr << "expected PalletTown NPC tiles to block movement\n";
    return 1;
  }
  if (pokered::MessagePageCount(pokered::MessageId::MomWakeUp) != 2 ||
      pokered::MessageText(pokered::MessageId::MomWakeUp, 1) != "PROF.OAK, next\ndoor, is looking\nfor you." ||
      pokered::MessagePageCount(pokered::MessageId::TvMovie) != 2) {
    std::cerr << "expected multi-page RedsHouse1F text to remain available\n";
    return 1;
  }
  if (pokered::MessagePageCount(pokered::MessageId::PewterSpeechHouseGambler) != 2 ||
      pokered::MessageText(pokered::MessageId::PewterSpeechHouseGambler, 1) !=
          "But, some moves\nmust be taught by\nthe trainer!" ||
      pokered::MessagePageCount(pokered::MessageId::PewterSpeechHouseYoungster) != 2 ||
      pokered::MessageText(pokered::MessageId::PewterSpeechHouseYoungster, 1) !=
          "But, it's not a\nsure thing!") {
    std::cerr << "expected PewterSpeechHouse NPC text import\n";
    return 1;
  }
  if (pokered::MessageText(pokered::MessageId::BluesHouseDaisyRivalAtLab, 0) !=
          "Hi PLAYER!\nRIVAL is out at\nGrandpa's lab." ||
      pokered::MessageText(pokered::MessageId::BluesHouseDaisyWalking, 0) !=
          "#MON are living\nthings! If they\nget tired, give\nthem a rest!" ||
      pokered::MessageText(pokered::MessageId::BluesHouseTownMap, 0) !=
          "It's a big map!\nThis is useful!") {
    std::cerr << "expected BluesHouse text import\n";
    return 1;
  }
  if (pokered::MessageText(pokered::MessageId::OaksLabRivalGrampsIsntAround, 0) !=
          "RIVAL: Yo\nPLAYER! Gramps\nisn't around!" ||
      pokered::MessageText(pokered::MessageId::OaksLabRivalMyPokemonLooksStronger, 0) !=
          "RIVAL: My\n#MON looks a\nlot stronger." ||
      pokered::MessageText(pokered::MessageId::OaksLabThoseArePokeBalls, 0) !=
          "Those are #\nBALLs. They\ncontain #MON!" ||
      pokered::MessageText(pokered::MessageId::OaksLabLastMon, 0) !=
          "That's PROF.OAK's\nlast #MON!" ||
      pokered::MessageText(pokered::MessageId::OaksLabOak1WhichPokemonDoYouWant, 0) !=
          "OAK: Now, PLAYER,\nwhich #MON do\nyou want?" ||
      pokered::MessageText(pokered::MessageId::OaksLabOak1YourPokemonCanFight, 0) !=
          "OAK: If a wild\n#MON appears,\nyour #MON can\nfight against it!" ||
      pokered::MessageText(pokered::MessageId::OaksLabPokedex, 0) !=
          "It's encyclopedia-\nlike, but the\npages are blank!" ||
      pokered::MessageText(pokered::MessageId::OaksLabOak2, 0) != "?" ||
      pokered::MessagePageCount(pokered::MessageId::OaksLabGirl) != 2 ||
      pokered::MessageText(pokered::MessageId::OaksLabGirl, 1) !=
          "Many #MON\ntrainers hold him\nin high regard!" ||
      pokered::MessageText(pokered::MessageId::OaksLabScientist, 0) !=
          "I study #MON as\nPROF.OAK's AIDE.") {
    std::cerr << "expected OaksLab text import\n";
    return 1;
  }
  if (pokered::MessagePageCount(pokered::MessageId::PalletTownGirl) != 2 ||
      pokered::MessagePageCount(pokered::MessageId::PalletTownOakHeyWaitDontGoOut) != 1 ||
      pokered::MessageText(pokered::MessageId::PalletTownOakHeyWaitDontGoOut, 0) !=
          "OAK: Hey! Wait!\nDon't go out!" ||
      pokered::MessageText(pokered::MessageId::PalletTownGirl, 1) !=
          "When they get\nstrong, they can\nprotect me!" ||
      pokered::MessagePageCount(pokered::MessageId::PalletTownFisher) != 2 ||
      pokered::MessageText(pokered::MessageId::PalletTownFisher, 1) !=
          "You can now store\nand recall items\nand #MON as\ndata via PC!" ||
      pokered::MessageText(pokered::MessageId::PalletTownOaksLabSign, 0) != "OAK #MON\nRESEARCH LAB" ||
      pokered::MessageText(pokered::MessageId::PalletTownSign, 0) !=
          "PALLET TOWN\nShades of your\njourney await!" ||
      pokered::MessageText(pokered::MessageId::PalletTownPlayersHouseSign, 0) != "PLAYER's house " ||
      pokered::MessageText(pokered::MessageId::PalletTownRivalsHouseSign, 0) != "RIVAL's house ") {
    std::cerr << "expected PalletTown text import\n";
    return 1;
  }

  pokered::WorldState oak_trigger {};
  oak_trigger.map_id = pokered::WorldId::PalletTown;
  oak_trigger.player = {pallet_north_exit_x, 2, pokered::Facing::Up};
  const pokered::MoveResult oak_warning = pokered::TryMoveWithResult(oak_trigger, pokered::Facing::Up);
  if (oak_warning.moved || oak_warning.message != pokered::MessageId::PalletTownOakHeyWaitDontGoOut ||
      oak_warning.warped || oak_warning.source_map != pokered::WorldId::PalletTown ||
      oak_warning.source_warp != 0 || oak_warning.target_map != pokered::WorldId::PalletTown ||
      oak_warning.target_warp != 0 || oak_warning.facing != pokered::Facing::Up ||
      oak_warning.from_x != pallet_north_exit_x || oak_warning.from_y != 2 || oak_warning.to_x != pallet_north_exit_x ||
      oak_warning.to_y != 1 || oak_warning.blocker != pokered::MoveBlocker::Script || oak_trigger.player.x != pallet_north_exit_x ||
      oak_trigger.player.y != 2 || oak_trigger.step_counter != 0) {
    std::cerr << "expected PalletTown Oak north-exit warning seam\n";
    return 1;
  }
  oak_trigger.got_starter = true;
  const pokered::MoveResult route_progress = pokered::TryMoveWithResult(oak_trigger, pokered::Facing::Up);
  if (!route_progress.moved || route_progress.message != pokered::MessageId::None || route_progress.warped ||
      route_progress.source_map != pokered::WorldId::PalletTown ||
      route_progress.target_map != pokered::WorldId::PalletTown || route_progress.facing != pokered::Facing::Up ||
      route_progress.from_x != pallet_north_exit_x || route_progress.from_y != 2 ||
      route_progress.to_x != pallet_north_exit_x || route_progress.to_y != 1 ||
      route_progress.blocker != pokered::MoveBlocker::None || oak_trigger.player.x != pallet_north_exit_x ||
      oak_trigger.player.y != 1 || oak_trigger.step_counter != 1) {
    std::cerr << "expected PalletTown north exit to reopen after starter flag\n";
    return 1;
  }
  pokered::WorldState wall_block {};
  wall_block.map_id = pokered::WorldId::RedsHouse1F;
  wall_block.player = {3, 2, pokered::Facing::Up};
  const pokered::MoveResult tv_block = pokered::TryMoveWithResult(wall_block, pokered::Facing::Up);
  if (tv_block.moved || tv_block.warped || tv_block.message != pokered::MessageId::None ||
      tv_block.source_map != pokered::WorldId::RedsHouse1F || tv_block.target_map != pokered::WorldId::RedsHouse1F ||
      tv_block.blocker != pokered::MoveBlocker::Collision || tv_block.from_x != 3 || tv_block.from_y != 2 ||
      tv_block.to_x != 3 || tv_block.to_y != 1 || wall_block.player.x != 3 || wall_block.player.y != 2) {
    std::cerr << "expected solid-tile move result blocker metadata\n";
    return 1;
  }
  pokered::WorldState npc_block {};
  npc_block.map_id = pokered::WorldId::PalletTown;
  npc_block.player = {8, 6, pokered::Facing::Up};
  const pokered::MoveResult girl_block = pokered::TryMoveWithResult(npc_block, pokered::Facing::Up);
  if (girl_block.moved || girl_block.warped || girl_block.message != pokered::MessageId::None ||
      girl_block.source_map != pokered::WorldId::PalletTown || girl_block.target_map != pokered::WorldId::PalletTown ||
      girl_block.blocker != pokered::MoveBlocker::Npc || girl_block.from_x != 8 || girl_block.from_y != 6 ||
      girl_block.to_x != 8 || girl_block.to_y != 5 || npc_block.player.x != 8 || npc_block.player.y != 6) {
    std::cerr << "expected NPC move result blocker metadata\n";
    return 1;
  }

  pokered::WorldState speech_world {};
  speech_world.map_id = pokered::WorldId::PewterSpeechHouse;
  speech_world.player.x = 2;
  speech_world.player.y = 4;
  speech_world.player.facing = pokered::Facing::Up;
  if (pokered::InteractionForFacingTile(speech_house, speech_world) != pokered::MessageId::PewterSpeechHouseGambler) {
    std::cerr << "expected PewterSpeechHouse gambler interaction\n";
    return 1;
  }
  speech_world.player.x = 4;
  speech_world.player.y = 6;
  if (pokered::InteractionForFacingTile(speech_house, speech_world) != pokered::MessageId::PewterSpeechHouseYoungster) {
    std::cerr << "expected PewterSpeechHouse youngster interaction\n";
    return 1;
  }
  speech_world.player.x = 2;
  speech_world.player.y = 6;
  speech_world.player.facing = pokered::Facing::Down;
  if (!pokered::TryMove(speech_world, pokered::Facing::Down) ||
      speech_world.map_id != pokered::WorldId::PewterSpeechHouse || speech_world.player.x != 2 ||
      speech_world.player.y != 7) {
    std::cerr << "expected PewterSpeechHouse door warp to remain local without a valid LAST_MAP target\n";
    return 1;
  }

  pokered::WorldState blues_world {};
  blues_world.map_id = pokered::WorldId::BluesHouse;
  blues_world.player.x = 2;
  blues_world.player.y = 4;
  blues_world.player.facing = pokered::Facing::Up;
  if (pokered::InteractionForFacingTile(blues_house, blues_world) != pokered::MessageId::BluesHouseDaisyRivalAtLab) {
    std::cerr << "expected BluesHouse seated Daisy interaction\n";
    return 1;
  }
  blues_world.player.x = 6;
  blues_world.player.y = 5;
  if (pokered::InteractionForFacingTile(blues_house, blues_world) != pokered::MessageId::BluesHouseDaisyWalking) {
    std::cerr << "expected BluesHouse walking Daisy interaction\n";
    return 1;
  }
  blues_world.player.x = 3;
  blues_world.player.y = 4;
  if (pokered::InteractionForFacingTile(blues_house, blues_world) != pokered::MessageId::BluesHouseTownMap) {
    std::cerr << "expected BluesHouse town map interaction\n";
    return 1;
  }

  pokered::WorldState oaks_world {};
  oaks_world.map_id = pokered::WorldId::OaksLab;
  oaks_world.player.x = 4;
  oaks_world.player.y = 4;
  oaks_world.player.facing = pokered::Facing::Up;
  if (pokered::InteractionForFacingTile(oaks_lab, oaks_world) != pokered::MessageId::OaksLabRivalGrampsIsntAround) {
    std::cerr << "expected OaksLab rival default interaction\n";
    return 1;
  }
  oaks_world.got_starter = true;
  if (pokered::InteractionForFacingTile(oaks_lab, oaks_world) != pokered::MessageId::OaksLabRivalMyPokemonLooksStronger) {
    std::cerr << "expected OaksLab rival post-starter interaction\n";
    return 1;
  }
  oaks_world.got_starter = false;
  oaks_world.player.x = 6;
  oaks_world.player.y = 4;
  if (pokered::InteractionForFacingTile(oaks_lab, oaks_world) != pokered::MessageId::OaksLabThoseArePokeBalls) {
    std::cerr << "expected OaksLab pokeball default interaction\n";
    return 1;
  }
  oaks_world.got_starter = true;
  if (pokered::InteractionForFacingTile(oaks_lab, oaks_world) != pokered::MessageId::OaksLabLastMon) {
    std::cerr << "expected OaksLab pokeball post-starter interaction\n";
    return 1;
  }
  oaks_world.got_starter = false;
  oaks_world.player.x = 5;
  oaks_world.player.y = 3;
  if (pokered::InteractionForFacingTile(oaks_lab, oaks_world) != pokered::MessageId::OaksLabOak1WhichPokemonDoYouWant) {
    std::cerr << "expected OaksLab Oak1 default interaction\n";
    return 1;
  }
  oaks_world.got_starter = true;
  if (pokered::InteractionForFacingTile(oaks_lab, oaks_world) != pokered::MessageId::OaksLabOak1YourPokemonCanFight) {
    std::cerr << "expected OaksLab Oak1 post-starter interaction\n";
    return 1;
  }
  oaks_world.player = {2, 2, pokered::Facing::Up};
  if (pokered::InteractionForFacingTile(oaks_lab, oaks_world) != pokered::MessageId::OaksLabPokedex) {
    std::cerr << "expected OaksLab pokedex interaction\n";
    return 1;
  }
  oaks_world.player = {1, 10, pokered::Facing::Up};
  if (pokered::InteractionForFacingTile(oaks_lab, oaks_world) != pokered::MessageId::OaksLabGirl) {
    std::cerr << "expected OaksLab girl interaction\n";
    return 1;
  }
  oaks_world.player.facing = pokered::Facing::Right;
  if (pokered::InteractionForFacingTile(oaks_lab, oaks_world) != pokered::MessageId::OaksLabScientist) {
    std::cerr << "expected OaksLab scientist interaction\n";
    return 1;
  }
  oaks_world.player = {1, 10, pokered::Facing::Left};
  if (pokered::InteractionForFacingTile(oaks_lab, oaks_world) != pokered::MessageId::None) {
    std::cerr << "expected OaksLab off-axis interaction miss\n";
    return 1;
  }

  pokered::WorldState pallet_world {};
  pallet_world.map_id = pokered::WorldId::PalletTown;
  pallet_world.player.x = 5;
  pallet_world.player.y = 6;
  pallet_world.player.facing = pokered::Facing::Up;
  const pokered::MoveResult reds_entry = pokered::TryMoveWithResult(pallet_world, pokered::Facing::Up);
  if (!reds_entry.moved || !reds_entry.warped || reds_entry.source_map != pokered::WorldId::PalletTown ||
      reds_entry.source_warp != 1 || reds_entry.target_map != pokered::WorldId::RedsHouse1F ||
      reds_entry.target_warp != 1 || reds_entry.message != pokered::MessageId::None ||
      pallet_world.map_id != pokered::WorldId::RedsHouse1F || pallet_world.player.x != 2 ||
      pallet_world.player.y != 7 ||
      pallet_world.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      pallet_world.last_warp != 1 || pallet_world.player.facing != pokered::Facing::Down) {
    std::cerr << "expected PalletTown door warp to enter RedsHouse1F\n";
    return 1;
  }

  pokered::WorldState blues_entry {};
  blues_entry.map_id = pokered::WorldId::PalletTown;
  blues_entry.player.x = 13;
  blues_entry.player.y = 6;
  blues_entry.player.facing = pokered::Facing::Up;
  if (!pokered::TryMove(blues_entry, pokered::Facing::Up) ||
      blues_entry.map_id != pokered::WorldId::BluesHouse || blues_entry.player.x != 2 ||
      blues_entry.player.y != 7 ||
      blues_entry.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      blues_entry.last_warp != 2 || blues_entry.player.facing != pokered::Facing::Down) {
    std::cerr << "expected PalletTown door warp to enter BluesHouse\n";
    return 1;
  }
  if (!pokered::TryMove(blues_entry, pokered::Facing::Up) || blues_entry.map_id != pokered::WorldId::BluesHouse ||
      blues_entry.player.x != 2 || blues_entry.player.y != 6) {
    std::cerr << "expected to step off the BluesHouse door tile\n";
    return 1;
  }
  if (!pokered::TryMove(blues_entry, pokered::Facing::Down) || blues_entry.map_id != pokered::WorldId::PalletTown ||
      blues_entry.player.x != 13 || blues_entry.player.y != 5 ||
      blues_entry.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_entry.last_warp != 1 || blues_entry.player.facing != pokered::Facing::Down) {
    std::cerr << "expected BluesHouse door warp to return to PalletTown\n";
    return 1;
  }

  pokered::WorldState oaks_entry {};
  oaks_entry.map_id = pokered::WorldId::PalletTown;
  oaks_entry.player.x = 12;
  oaks_entry.player.y = 12;
  oaks_entry.player.facing = pokered::Facing::Up;
  const pokered::MoveResult oaks_warp = pokered::TryMoveWithResult(oaks_entry, pokered::Facing::Up);
  if (!oaks_warp.moved || !oaks_warp.warped || oaks_warp.source_map != pokered::WorldId::PalletTown ||
      oaks_warp.source_warp != 3 || oaks_warp.target_map != pokered::WorldId::OaksLab ||
      oaks_warp.target_warp != 2 || oaks_warp.message != pokered::MessageId::None ||
      oaks_entry.map_id != pokered::WorldId::OaksLab || oaks_entry.player.x != 5 ||
      oaks_entry.player.y != 11 ||
      oaks_entry.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      oaks_entry.last_warp != 3 || oaks_entry.player.facing != pokered::Facing::Down) {
    std::cerr << "expected PalletTown door warp to enter OaksLab\n";
    return 1;
  }
  if (!pokered::TryMove(oaks_entry, pokered::Facing::Left) || oaks_entry.map_id != pokered::WorldId::PalletTown ||
      oaks_entry.player.x != 12 || oaks_entry.player.y != 11 ||
      oaks_entry.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_entry.last_warp != 1 || oaks_entry.player.facing != pokered::Facing::Down) {
    std::cerr << "expected OaksLab door warp to return to PalletTown\n";
    return 1;
  }

  pokered::WorldState warp_world {};
  warp_world.map_id = pokered::WorldId::RedsHouse1F;
  warp_world.player.x = 3;
  warp_world.player.y = 6;
  warp_world.player.facing = pokered::Facing::Down;
  warp_world.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  warp_world.last_warp = 1;
  const pokered::MoveResult house_exit = pokered::TryMoveWithResult(warp_world, pokered::Facing::Down);
  if (!house_exit.moved || !house_exit.warped || house_exit.source_map != pokered::WorldId::RedsHouse1F ||
      house_exit.source_warp != 2 || house_exit.target_map != pokered::WorldId::PalletTown ||
      house_exit.target_warp != 1 || house_exit.message != pokered::MessageId::None ||
      warp_world.map_id != pokered::WorldId::PalletTown ||
      warp_world.player.x != 5 || warp_world.player.y != 5 ||
      warp_world.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      warp_world.last_warp != 2 || warp_world.player.facing != pokered::Facing::Down ||
      warp_world.step_counter != 1) {
    std::cerr << "expected RedsHouse1F door warp to exit into PalletTown\n";
    return 1;
  }
  const pokered::MoveResult pallet_step = pokered::TryMoveWithResult(warp_world, pokered::Facing::Down);
  if (!pallet_step.moved || pallet_step.warped || pallet_step.message != pokered::MessageId::None ||
      warp_world.map_id != pokered::WorldId::PalletTown ||
      warp_world.player.x != 5 || warp_world.player.y != 6) {
    std::cerr << "expected to step off the PalletTown house door\n";
    return 1;
  }
  if (!pokered::TryMove(warp_world, pokered::Facing::Up) || warp_world.map_id != pokered::WorldId::RedsHouse1F ||
      warp_world.player.x != 2 || warp_world.player.y != 7 ||
      warp_world.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      warp_world.last_warp != 1 || warp_world.player.facing != pokered::Facing::Down) {
    std::cerr << "expected PalletTown house door to re-enter RedsHouse1F\n";
    return 1;
  }

  pokered::WorldState outdoor_walk {};
  outdoor_walk.map_id = pokered::WorldId::PalletTown;
  outdoor_walk.player.x = 4;
  outdoor_walk.player.y = 8;
  outdoor_walk.player.facing = pokered::Facing::Left;
  if (pokered::InteractionForFacingTile(pallet_town, outdoor_walk) != pokered::MessageId::PalletTownGirl) {
    std::cerr << "expected PalletTown girl interaction\n";
    return 1;
  }
  outdoor_walk.player.x = 10;
  outdoor_walk.player.y = 14;
  outdoor_walk.player.facing = pokered::Facing::Right;
  if (pokered::InteractionForFacingTile(pallet_town, outdoor_walk) != pokered::MessageId::PalletTownFisher) {
    std::cerr << "expected PalletTown fisher interaction\n";
    return 1;
  }
  outdoor_walk.player.x = 13;
  outdoor_walk.player.y = 14;
  outdoor_walk.player.facing = pokered::Facing::Up;
  if (pokered::InteractionForFacingTile(pallet_town, outdoor_walk) != pokered::MessageId::PalletTownOaksLabSign) {
    std::cerr << "expected PalletTown sign interaction\n";
    return 1;
  }

  pokered::WorldState stairs_world {};
  stairs_world.map_id = pokered::WorldId::RedsHouse1F;
  stairs_world.player.x = 7;
  stairs_world.player.y = 2;
  stairs_world.player.facing = pokered::Facing::Up;
  if (!pokered::TryMove(stairs_world, pokered::Facing::Up) ||
      stairs_world.map_id != pokered::WorldId::RedsHouse2F || stairs_world.player.x != 7 ||
      stairs_world.player.y != 1 ||
      stairs_world.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      stairs_world.last_warp != 3 || stairs_world.player.facing != pokered::Facing::Down) {
    std::cerr << "expected RedsHouse1F stair warp to enter RedsHouse2F\n";
    return 1;
  }
  if (!pokered::TryMove(stairs_world, pokered::Facing::Down) ||
      stairs_world.map_id != pokered::WorldId::RedsHouse2F || stairs_world.player.x != 7 ||
      stairs_world.player.y != 2) {
    std::cerr << "expected to step off RedsHouse2F stairs\n";
    return 1;
  }
  if (!pokered::TryMove(stairs_world, pokered::Facing::Up) ||
      stairs_world.map_id != pokered::WorldId::RedsHouse1F || stairs_world.player.x != 7 ||
      stairs_world.player.y != 1 ||
      stairs_world.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse2F) ||
      stairs_world.last_warp != 1 || stairs_world.player.facing != pokered::Facing::Down) {
    std::cerr << "expected RedsHouse2F stair warp to return to RedsHouse1F\n";
    return 1;
  }

  pokered::GameState state {};
  pokered::StartNewGameShortcut(state);
  state.world.map_id = pokered::WorldId::PalletTown;
  state.world.player.x = 4;
  state.world.player.y = 8;
  state.world.player.facing = pokered::Facing::Left;
  state.world.got_starter = true;
  state.world.step_counter = 42;
  state.world.rng_state = 0x1234ABCDu;
  state.world.last_map = static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F);
  state.world.last_warp = 2;

  const std::filesystem::path path_a = std::filesystem::temp_directory_path() / "pokered_native_test_a.bin";
  const std::filesystem::path path_b = std::filesystem::temp_directory_path() / "pokered_native_test_b.bin";

  if (!pokered::SaveSystem::Save(path_a, state) || !pokered::SaveSystem::Save(path_b, state)) {
    std::cerr << "save write failed\n";
    return 1;
  }

  if (ReadBytes(path_a) != ReadBytes(path_b)) {
    std::cerr << "save bytes are not deterministic\n";
    return 1;
  }

  const pokered::LoadResult loaded = pokered::SaveSystem::Load(path_a);
  if (loaded.status != pokered::LoadStatus::Ok) {
    std::cerr << "save load failed\n";
    return 1;
  }
  if (loaded.state.world.player.x != state.world.player.x ||
      loaded.state.world.player.y != state.world.player.y ||
      loaded.state.world.player.facing != state.world.player.facing ||
      loaded.state.world.map_id != state.world.map_id ||
      loaded.state.world.got_starter != state.world.got_starter ||
      loaded.state.world.last_map != state.world.last_map ||
      loaded.state.world.last_warp != state.world.last_warp ||
      loaded.state.world.step_counter != state.world.step_counter ||
      loaded.state.world.rng_state != state.world.rng_state) {
    std::cerr << "save round-trip mismatch\n";
    return 1;
  }

  std::vector<unsigned char> corrupted = ReadBytes(path_a);
  corrupted[12] ^= 0xFFu;
  WriteBytes(path_b, corrupted);
  if (pokered::SaveSystem::Load(path_b).status != pokered::LoadStatus::Corrupt) {
    std::cerr << "corrupted save should fail checksum validation\n";
    return 1;
  }

  std::cout << "save-tests-ok\n";
  return 0;
}
