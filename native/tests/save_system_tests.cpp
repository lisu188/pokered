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
  const auto pallet_script_provenance =
      pokered::oracle::LookupMapScriptProvenance(symbols, sections, pokered::WorldId::PalletTown);
  if (!pallet_script_provenance || pallet_script_provenance->world_id != pokered::WorldId::PalletTown ||
      pallet_script_provenance->script.label != "PalletTown_Script" ||
      pallet_script_provenance->script.address.bank != 0x06 ||
      pallet_script_provenance->script.address.address != 0x4E5B ||
      !pallet_script_provenance->script.section || pallet_script_provenance->script.section->name != "Maps 2" ||
      !pallet_script_provenance->script_pointers ||
      pallet_script_provenance->script_pointers->label != "PalletTown_ScriptPointers" ||
      pallet_script_provenance->script_pointers->address.bank != 0x06 ||
      pallet_script_provenance->script_pointers->address.address != 0x4E73 ||
      !pallet_script_provenance->script_pointers->section ||
      pallet_script_provenance->script_pointers->section->name != "Maps 2" ||
      !pallet_script_provenance->current_script ||
      pallet_script_provenance->current_script->label != "wPalletTownCurScript" ||
      pallet_script_provenance->current_script->address.bank != 0x00 ||
      pallet_script_provenance->current_script->address.address != 0xD5F1 ||
      !pallet_script_provenance->current_script->section ||
      pallet_script_provenance->current_script->section->memory_region != "WRAM0" ||
      pallet_script_provenance->current_script->section->name != "Main Data") {
    std::cerr << "expected PalletTown map-script provenance lookup\n";
    return 1;
  }
  const auto reds_house_1f_script_provenance =
      pokered::oracle::LookupMapScriptProvenance(symbols, sections, pokered::WorldId::RedsHouse1F);
  if (!reds_house_1f_script_provenance ||
      reds_house_1f_script_provenance->world_id != pokered::WorldId::RedsHouse1F ||
      reds_house_1f_script_provenance->script.label != "RedsHouse1F_Script" ||
      reds_house_1f_script_provenance->script.address.bank != 0x12 ||
      reds_house_1f_script_provenance->script.address.address != 0x4168 ||
      !reds_house_1f_script_provenance->script.section ||
      reds_house_1f_script_provenance->script.section->name != "Maps 8" ||
      reds_house_1f_script_provenance->script_pointers || reds_house_1f_script_provenance->current_script) {
    std::cerr << "expected RedsHouse1F map-script provenance lookup\n";
    return 1;
  }
  const auto oaks_lab_script_provenance =
      pokered::oracle::LookupMapScriptProvenance(symbols, sections, pokered::WorldId::OaksLab);
  if (!oaks_lab_script_provenance || oaks_lab_script_provenance->world_id != pokered::WorldId::OaksLab ||
      oaks_lab_script_provenance->script.label != "OaksLab_Script" ||
      oaks_lab_script_provenance->script.address.bank != 0x07 ||
      oaks_lab_script_provenance->script.address.address != 0x4B0E ||
      !oaks_lab_script_provenance->script.section || oaks_lab_script_provenance->script.section->name != "Maps 4" ||
      !oaks_lab_script_provenance->script_pointers ||
      oaks_lab_script_provenance->script_pointers->label != "OaksLab_ScriptPointers" ||
      oaks_lab_script_provenance->script_pointers->address.bank != 0x07 ||
      oaks_lab_script_provenance->script_pointers->address.address != 0x4B28 ||
      !oaks_lab_script_provenance->script_pointers->section ||
      oaks_lab_script_provenance->script_pointers->section->name != "Maps 4" ||
      !oaks_lab_script_provenance->current_script ||
      oaks_lab_script_provenance->current_script->label != "wOaksLabCurScript" ||
      oaks_lab_script_provenance->current_script->address.bank != 0x00 ||
      oaks_lab_script_provenance->current_script->address.address != 0xD5F0 ||
      !oaks_lab_script_provenance->current_script->section ||
      oaks_lab_script_provenance->current_script->section->memory_region != "WRAM0" ||
      oaks_lab_script_provenance->current_script->section->name != "Main Data") {
    std::cerr << "expected OaksLab map-script provenance lookup\n";
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
  const auto pallet_last_map_provenance = pokered::oracle::LookupLastMapProvenance(
      symbols, sections, pokered::WorldId::PalletTown, 1);
  if (!pallet_last_map_provenance || pallet_last_map_provenance->world_id != pokered::WorldId::PalletTown ||
      pallet_last_map_provenance->warp_id != 1 || pallet_last_map_provenance->object.label != "PalletTown_Object" ||
      pallet_last_map_provenance->object.address.bank != 0x06 ||
      pallet_last_map_provenance->object.address.address != 0x42C3 ||
      !pallet_last_map_provenance->object.section || pallet_last_map_provenance->object.section->name != "Maps 1") {
    std::cerr << "expected PalletTown last-map provenance lookup\n";
    return 1;
  }
  const auto house_last_map_provenance = pokered::oracle::LookupLastMapProvenance(
      symbols, sections, pokered::WorldId::RedsHouse1F, 2);
  if (!house_last_map_provenance || house_last_map_provenance->world_id != pokered::WorldId::RedsHouse1F ||
      house_last_map_provenance->warp_id != 2 || house_last_map_provenance->object.label != "RedsHouse1F_Object" ||
      house_last_map_provenance->object.address.bank != 0x12 ||
      house_last_map_provenance->object.address.address != 0x41E4 ||
      !house_last_map_provenance->object.section || house_last_map_provenance->object.section->name != "Maps 8") {
    std::cerr << "expected RedsHouse1F last-map provenance lookup\n";
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
  const auto girl_interaction_provenance = pokered::oracle::LookupInteractionProvenance(
      symbols,
      sections,
      pokered::WorldId::PalletTown,
      pokered::MessageId::PalletTownGirl,
      pokered::MessageId::PalletTownGirl);
  if (!girl_interaction_provenance || girl_interaction_provenance->object.label != "PalletTown_Object" ||
      girl_interaction_provenance->object.address.bank != 0x06 ||
      girl_interaction_provenance->object.address.address != 0x42C3 ||
      !girl_interaction_provenance->object.section || girl_interaction_provenance->object.section->name != "Maps 1" ||
      girl_interaction_provenance->origin_message != pokered::MessageId::PalletTownGirl ||
      girl_interaction_provenance->message_id != pokered::MessageId::PalletTownGirl ||
      girl_interaction_provenance->source.label != "PalletTownGirlText" ||
      girl_interaction_provenance->source.address.bank != 0x06 ||
      girl_interaction_provenance->source.address.address != 0x4FD3 ||
      !girl_interaction_provenance->source.section || girl_interaction_provenance->source.section->name != "Maps 2") {
    std::cerr << "expected PalletTown interaction provenance lookup\n";
    return 1;
  }
  const auto pokedex_interaction_provenance = pokered::oracle::LookupInteractionProvenance(
      symbols,
      sections,
      pokered::WorldId::OaksLab,
      pokered::MessageId::OaksLabPokedex,
      pokered::MessageId::OaksLabPokedex);
  if (!pokedex_interaction_provenance || pokedex_interaction_provenance->object.label != "OaksLab_Object" ||
      pokedex_interaction_provenance->object.address.bank != 0x07 ||
      pokedex_interaction_provenance->object.address.address != 0x540A ||
      !pokedex_interaction_provenance->object.section ||
      pokedex_interaction_provenance->object.section->name != "Maps 4" ||
      pokedex_interaction_provenance->origin_message != pokered::MessageId::OaksLabPokedex ||
      pokedex_interaction_provenance->message_id != pokered::MessageId::OaksLabPokedex ||
      pokedex_interaction_provenance->source.label != "OaksLabPokedexText" ||
      pokedex_interaction_provenance->source.address.bank != 0x07 ||
      pokedex_interaction_provenance->source.address.address != 0x5322 ||
      !pokedex_interaction_provenance->source.section ||
      pokedex_interaction_provenance->source.section->name != "Maps 4") {
    std::cerr << "expected OaksLab interaction provenance lookup\n";
    return 1;
  }
  pokered::WorldState girl_facing_world {};
  girl_facing_world.map_id = pokered::WorldId::PalletTown;
  girl_facing_world.player = {4, 8, pokered::Facing::Left};
  const auto girl_facing_provenance =
      pokered::oracle::LookupFacingProvenance(symbols, sections, girl_facing_world);
  if (!girl_facing_provenance || girl_facing_provenance->world_id != pokered::WorldId::PalletTown ||
      girl_facing_provenance->facing != pokered::Facing::Left ||
      girl_facing_provenance->kind != pokered::InteractionKind::Npc || girl_facing_provenance->target_x != 3 ||
      girl_facing_provenance->target_y != 8 ||
      girl_facing_provenance->origin_message != pokered::MessageId::PalletTownGirl ||
      girl_facing_provenance->message_id != pokered::MessageId::PalletTownGirl ||
      girl_facing_provenance->object.label != "PalletTown_Object" ||
      girl_facing_provenance->source.label != "PalletTownGirlText") {
    std::cerr << "expected PalletTown girl facing provenance lookup\n";
    return 1;
  }
  pokered::WorldState tv_facing_world {};
  tv_facing_world.map_id = pokered::WorldId::RedsHouse1F;
  tv_facing_world.player = {2, 1, pokered::Facing::Right};
  const auto tv_facing_provenance =
      pokered::oracle::LookupFacingProvenance(symbols, sections, tv_facing_world);
  if (!tv_facing_provenance || tv_facing_provenance->world_id != pokered::WorldId::RedsHouse1F ||
      tv_facing_provenance->facing != pokered::Facing::Right ||
      tv_facing_provenance->kind != pokered::InteractionKind::BgEvent || tv_facing_provenance->target_x != 3 ||
      tv_facing_provenance->target_y != 1 ||
      tv_facing_provenance->origin_message != pokered::MessageId::TvMovie ||
      tv_facing_provenance->message_id != pokered::MessageId::TvWrongSide ||
      tv_facing_provenance->object.label != "RedsHouse1F_Object" ||
      tv_facing_provenance->source.label != "RedsHouse1FTVText.WrongSideText") {
    std::cerr << "expected RedsHouse1F TV facing provenance lookup\n";
    return 1;
  }
  pokered::WorldState rival_facing_world {};
  rival_facing_world.map_id = pokered::WorldId::OaksLab;
  rival_facing_world.player = {4, 4, pokered::Facing::Up};
  rival_facing_world.got_starter = true;
  const auto rival_facing_provenance =
      pokered::oracle::LookupFacingProvenance(symbols, sections, rival_facing_world);
  if (!rival_facing_provenance || rival_facing_provenance->world_id != pokered::WorldId::OaksLab ||
      rival_facing_provenance->facing != pokered::Facing::Up ||
      rival_facing_provenance->kind != pokered::InteractionKind::Npc || rival_facing_provenance->target_x != 4 ||
      rival_facing_provenance->target_y != 3 ||
      rival_facing_provenance->origin_message != pokered::MessageId::OaksLabRival ||
      rival_facing_provenance->message_id != pokered::MessageId::OaksLabRivalMyPokemonLooksStronger ||
      rival_facing_provenance->object.label != "OaksLab_Object" ||
      rival_facing_provenance->source.label != "OaksLabRivalText.MyPokemonLooksStrongerText") {
    std::cerr << "expected OaksLab rival facing provenance lookup\n";
    return 1;
  }
  const auto girl_facing_message =
      pokered::oracle::LookupFacingMessageProvenance(symbols, sections, girl_facing_world);
  if (!girl_facing_message || girl_facing_message->world_id != pokered::WorldId::PalletTown ||
      girl_facing_message->facing != pokered::Facing::Left ||
      girl_facing_message->kind != pokered::InteractionKind::Npc || girl_facing_message->target_x != 3 ||
      girl_facing_message->target_y != 8 ||
      girl_facing_message->origin_message != pokered::MessageId::PalletTownGirl ||
      girl_facing_message->message_id != pokered::MessageId::PalletTownGirl ||
      girl_facing_message->text.label != "_PalletTownGirlText" ||
      girl_facing_message->text.address.bank != 0x29 ||
      girl_facing_message->text.address.address != 0x42DC || !girl_facing_message->text.section ||
      girl_facing_message->text.section->name != "Text 10") {
    std::cerr << "expected PalletTown girl facing message provenance lookup\n";
    return 1;
  }
  const auto tv_facing_message =
      pokered::oracle::LookupFacingMessageProvenance(symbols, sections, tv_facing_world);
  if (!tv_facing_message || tv_facing_message->origin_message != pokered::MessageId::TvMovie ||
      tv_facing_message->message_id != pokered::MessageId::TvWrongSide ||
      tv_facing_message->text.label != "_RedsHouse1FTVWrongSideText" ||
      tv_facing_message->text.address.bank != 0x25 ||
      tv_facing_message->text.address.address != 0x4C29 || !tv_facing_message->text.section ||
      tv_facing_message->text.section->name != "Text 6") {
    std::cerr << "expected RedsHouse1F TV facing message provenance lookup\n";
    return 1;
  }
  const auto rival_facing_message =
      pokered::oracle::LookupFacingMessageProvenance(symbols, sections, rival_facing_world);
  if (!rival_facing_message || rival_facing_message->origin_message != pokered::MessageId::OaksLabRival ||
      rival_facing_message->message_id != pokered::MessageId::OaksLabRivalMyPokemonLooksStronger ||
      rival_facing_message->text.label != "_OaksLabRivalMyPokemonLooksStrongerText" ||
      rival_facing_message->text.address.bank != 0x25 ||
      rival_facing_message->text.address.address != 0x4DBD || !rival_facing_message->text.section ||
      rival_facing_message->text.section->name != "Text 6") {
    std::cerr << "expected OaksLab rival facing message provenance lookup\n";
    return 1;
  }
  pokered::WorldState mom_default_facing_branch_world {};
  mom_default_facing_branch_world.map_id = pokered::WorldId::RedsHouse1F;
  mom_default_facing_branch_world.player = {5, 5, pokered::Facing::Up};
  const auto mom_default_facing_branch =
      pokered::oracle::LookupFacingBranchProvenance(symbols, sections, mom_default_facing_branch_world);
  if (!mom_default_facing_branch || mom_default_facing_branch->world_id != pokered::WorldId::RedsHouse1F ||
      mom_default_facing_branch->facing != pokered::Facing::Up || mom_default_facing_branch->target_x != 5 ||
      mom_default_facing_branch->target_y != 4 ||
      mom_default_facing_branch->origin_message != pokered::MessageId::MomWakeUp ||
      mom_default_facing_branch->message_id != pokered::MessageId::MomWakeUp ||
      mom_default_facing_branch->branch.label != "RedsHouse1FMomText") {
    std::cerr << "expected RedsHouse1F mom default facing branch provenance lookup\n";
    return 1;
  }
  pokered::WorldState mom_rest_facing_branch_world {};
  mom_rest_facing_branch_world.map_id = pokered::WorldId::RedsHouse1F;
  mom_rest_facing_branch_world.player = {5, 5, pokered::Facing::Up};
  mom_rest_facing_branch_world.got_starter = true;
  const auto mom_rest_facing_branch =
      pokered::oracle::LookupFacingBranchProvenance(symbols, sections, mom_rest_facing_branch_world);
  if (!mom_rest_facing_branch || mom_rest_facing_branch->message_id != pokered::MessageId::MomRest ||
      mom_rest_facing_branch->branch.label != "RedsHouse1FMomText.heal") {
    std::cerr << "expected RedsHouse1F mom rest facing branch provenance lookup\n";
    return 1;
  }
  pokered::WorldState tv_facing_branch_world {};
  tv_facing_branch_world.map_id = pokered::WorldId::RedsHouse1F;
  tv_facing_branch_world.player = {2, 1, pokered::Facing::Right};
  const auto tv_facing_branch =
      pokered::oracle::LookupFacingBranchProvenance(symbols, sections, tv_facing_branch_world);
  if (!tv_facing_branch || tv_facing_branch->origin_message != pokered::MessageId::TvMovie ||
      tv_facing_branch->message_id != pokered::MessageId::TvWrongSide ||
      tv_facing_branch->branch.label != "RedsHouse1FTVText") {
    std::cerr << "expected RedsHouse1F TV facing branch provenance lookup\n";
    return 1;
  }
  pokered::WorldState rival_facing_branch_world {};
  rival_facing_branch_world.map_id = pokered::WorldId::OaksLab;
  rival_facing_branch_world.player = {4, 4, pokered::Facing::Up};
  rival_facing_branch_world.got_starter = true;
  const auto rival_facing_branch =
      pokered::oracle::LookupFacingBranchProvenance(symbols, sections, rival_facing_branch_world);
  if (!rival_facing_branch || rival_facing_branch->origin_message != pokered::MessageId::OaksLabRival ||
      rival_facing_branch->message_id != pokered::MessageId::OaksLabRivalMyPokemonLooksStronger ||
      rival_facing_branch->branch.label != "OaksLabRivalText.afterChooseMon") {
    std::cerr << "expected OaksLab rival facing branch provenance lookup\n";
    return 1;
  }
  pokered::WorldState mom_facing_gate_world {};
  mom_facing_gate_world.map_id = pokered::WorldId::RedsHouse1F;
  mom_facing_gate_world.player = {5, 5, pokered::Facing::Up};
  const auto mom_facing_gate =
      pokered::oracle::LookupFacingStateGateProvenance(symbols, sections, mom_facing_gate_world);
  if (!mom_facing_gate || mom_facing_gate->world_id != pokered::WorldId::RedsHouse1F ||
      mom_facing_gate->facing != pokered::Facing::Up || mom_facing_gate->target_x != 5 ||
      mom_facing_gate->target_y != 4 || mom_facing_gate->origin_message != pokered::MessageId::MomWakeUp ||
      mom_facing_gate->message_id != pokered::MessageId::MomWakeUp || mom_facing_gate->gate_value ||
      mom_facing_gate->gate_source.gate != pokered::StateGate::GotStarter ||
      mom_facing_gate->gate_source.condition_label != "BIT_GOT_STARTER" ||
      !mom_facing_gate->gate_source.state_symbol || mom_facing_gate->gate_source.context_symbol ||
      mom_facing_gate->gate_source.state_symbol->label != "wStatusFlags4" ||
      mom_facing_gate->gate_source.state_symbol->address.bank != 0x00 ||
      mom_facing_gate->gate_source.state_symbol->address.address != 0xD72E ||
      !mom_facing_gate->gate_source.state_symbol->section ||
      mom_facing_gate->gate_source.state_symbol->section->memory_region != "WRAM0" ||
      mom_facing_gate->gate_source.state_symbol->section->name != "Main Data") {
    std::cerr << "expected RedsHouse1F mom facing gate provenance lookup\n";
    return 1;
  }
  pokered::WorldState tv_facing_gate_world {};
  tv_facing_gate_world.map_id = pokered::WorldId::RedsHouse1F;
  tv_facing_gate_world.player = {2, 1, pokered::Facing::Right};
  const auto tv_facing_gate =
      pokered::oracle::LookupFacingStateGateProvenance(symbols, sections, tv_facing_gate_world);
  if (!tv_facing_gate || tv_facing_gate->origin_message != pokered::MessageId::TvMovie ||
      tv_facing_gate->message_id != pokered::MessageId::TvWrongSide || tv_facing_gate->gate_value ||
      tv_facing_gate->gate_source.gate != pokered::StateGate::FacingUp ||
      tv_facing_gate->gate_source.condition_label != "SPRITE_FACING_UP" ||
      !tv_facing_gate->gate_source.state_symbol || tv_facing_gate->gate_source.context_symbol ||
      tv_facing_gate->gate_source.state_symbol->label != "wSpritePlayerStateData1FacingDirection") {
    std::cerr << "expected RedsHouse1F TV facing gate provenance lookup\n";
    return 1;
  }
  pokered::WorldState rival_facing_gate_world {};
  rival_facing_gate_world.map_id = pokered::WorldId::OaksLab;
  rival_facing_gate_world.player = {4, 4, pokered::Facing::Up};
  rival_facing_gate_world.got_starter = true;
  const auto rival_facing_gate =
      pokered::oracle::LookupFacingStateGateProvenance(symbols, sections, rival_facing_gate_world);
  if (!rival_facing_gate || rival_facing_gate->origin_message != pokered::MessageId::OaksLabRival ||
      rival_facing_gate->message_id != pokered::MessageId::OaksLabRivalMyPokemonLooksStronger ||
      !rival_facing_gate->gate_value || rival_facing_gate->gate_source.gate != pokered::StateGate::GotStarter ||
      rival_facing_gate->gate_source.condition_label != "BIT_GOT_STARTER" ||
      !rival_facing_gate->gate_source.state_symbol || rival_facing_gate->gate_source.context_symbol ||
      rival_facing_gate->gate_source.state_symbol->label != "wStatusFlags4") {
    std::cerr << "expected OaksLab rival facing gate provenance lookup\n";
    return 1;
  }
  const auto mom_branch_provenance = pokered::oracle::LookupInteractionBranchProvenance(
      symbols,
      sections,
      pokered::WorldId::RedsHouse1F,
      pokered::MessageId::MomWakeUp,
      pokered::MessageId::MomRest);
  if (!mom_branch_provenance || mom_branch_provenance->branch.label != "RedsHouse1FMomText.heal" ||
      mom_branch_provenance->branch.address.bank != 0x12 ||
      mom_branch_provenance->branch.address.address != 0x417F || !mom_branch_provenance->branch.section ||
      mom_branch_provenance->branch.section->name != "Maps 8") {
    std::cerr << "expected RedsHouse1F mom branch provenance lookup\n";
    return 1;
  }
  const auto mom_default_branch_provenance = pokered::oracle::LookupInteractionBranchProvenance(
      symbols,
      sections,
      pokered::WorldId::RedsHouse1F,
      pokered::MessageId::MomWakeUp,
      pokered::MessageId::MomWakeUp);
  if (!mom_default_branch_provenance || mom_default_branch_provenance->branch.label != "RedsHouse1FMomText" ||
      mom_default_branch_provenance->branch.address.bank != 0x12 ||
      mom_default_branch_provenance->branch.address.address != 0x416F ||
      !mom_default_branch_provenance->branch.section ||
      mom_default_branch_provenance->branch.section->name != "Maps 8") {
    std::cerr << "expected RedsHouse1F mom default branch provenance lookup\n";
    return 1;
  }
  const auto tv_branch_provenance = pokered::oracle::LookupInteractionBranchProvenance(
      symbols,
      sections,
      pokered::WorldId::RedsHouse1F,
      pokered::MessageId::TvMovie,
      pokered::MessageId::TvWrongSide);
  if (!tv_branch_provenance || tv_branch_provenance->branch.label != "RedsHouse1FTVText" ||
      tv_branch_provenance->branch.address.bank != 0x12 ||
      tv_branch_provenance->branch.address.address != 0x41C6 || !tv_branch_provenance->branch.section ||
      tv_branch_provenance->branch.section->name != "Maps 8") {
    std::cerr << "expected RedsHouse1F TV branch provenance lookup\n";
    return 1;
  }
  const auto rival_branch_provenance = pokered::oracle::LookupInteractionBranchProvenance(
      symbols,
      sections,
      pokered::WorldId::OaksLab,
      pokered::MessageId::OaksLabRival,
      pokered::MessageId::OaksLabRivalMyPokemonLooksStronger);
  if (!rival_branch_provenance || rival_branch_provenance->branch.label != "OaksLabRivalText.afterChooseMon" ||
      rival_branch_provenance->branch.address.bank != 0x07 ||
      rival_branch_provenance->branch.address.address != 0x50EA || !rival_branch_provenance->branch.section ||
      rival_branch_provenance->branch.section->name != "Maps 4") {
    std::cerr << "expected OaksLab rival branch provenance lookup\n";
    return 1;
  }
  const auto oak_branch_provenance = pokered::oracle::LookupInteractionBranchProvenance(
      symbols,
      sections,
      pokered::WorldId::OaksLab,
      pokered::MessageId::OaksLabOak1,
      pokered::MessageId::OaksLabOak1WhichPokemonDoYouWant);
  if (!oak_branch_provenance || oak_branch_provenance->branch.label != "OaksLabOak1Text.check_for_poke_balls" ||
      oak_branch_provenance->branch.address.bank != 0x07 ||
      oak_branch_provenance->branch.address.address != 0x5279 || !oak_branch_provenance->branch.section ||
      oak_branch_provenance->branch.section->name != "Maps 4") {
    std::cerr << "expected OaksLab Oak1 branch provenance lookup\n";
    return 1;
  }
  const auto oak_gate_provenance = pokered::oracle::LookupMoveStateGateProvenance(symbols,
                                                                                   sections,
                                                                                   pokered::WorldId::PalletTown,
                                                                                   pokered::MoveBlocker::Script,
                                                                                   pokered::MessageId::PalletTownOakHeyWaitDontGoOut,
                                                                                   pokered::StateGate::GotStarter);
  if (!oak_gate_provenance || oak_gate_provenance->condition_label != "EVENT_FOLLOWED_OAK_INTO_LAB" ||
      oak_gate_provenance->state_symbol || !oak_gate_provenance->context_symbol ||
      oak_gate_provenance->context_symbol->label != "PalletTownDefaultScript" ||
      oak_gate_provenance->context_symbol->address.bank != 0x06 ||
      oak_gate_provenance->context_symbol->address.address != 0x4E81 ||
      !oak_gate_provenance->context_symbol->section ||
      oak_gate_provenance->context_symbol->section->name != "Maps 2") {
    std::cerr << "expected PalletTown Oak gate provenance lookup\n";
    return 1;
  }
  const auto mom_gate_provenance = pokered::oracle::LookupInteractionStateGateProvenance(
      symbols,
      sections,
      pokered::WorldId::RedsHouse1F,
      pokered::MessageId::MomWakeUp,
      pokered::MessageId::MomRest,
      pokered::StateGate::GotStarter);
  if (!mom_gate_provenance || mom_gate_provenance->condition_label != "BIT_GOT_STARTER" ||
      !mom_gate_provenance->state_symbol || mom_gate_provenance->context_symbol ||
      mom_gate_provenance->state_symbol->label != "wStatusFlags4" ||
      mom_gate_provenance->state_symbol->address.bank != 0x00 ||
      mom_gate_provenance->state_symbol->address.address != 0xD72E ||
      !mom_gate_provenance->state_symbol->section ||
      mom_gate_provenance->state_symbol->section->memory_region != "WRAM0" ||
      mom_gate_provenance->state_symbol->section->name != "Main Data") {
    std::cerr << "expected RedsHouse1F mom gate provenance lookup\n";
    return 1;
  }
  const auto tv_gate_provenance = pokered::oracle::LookupInteractionStateGateProvenance(
      symbols,
      sections,
      pokered::WorldId::RedsHouse1F,
      pokered::MessageId::TvMovie,
      pokered::MessageId::TvWrongSide,
      pokered::StateGate::FacingUp);
  if (!tv_gate_provenance || tv_gate_provenance->condition_label != "SPRITE_FACING_UP" ||
      !tv_gate_provenance->state_symbol || tv_gate_provenance->context_symbol ||
      tv_gate_provenance->state_symbol->label != "wSpritePlayerStateData1FacingDirection" ||
      tv_gate_provenance->state_symbol->address.bank != 0x00 ||
      tv_gate_provenance->state_symbol->address.address != 0xC109 ||
      !tv_gate_provenance->state_symbol->section ||
      tv_gate_provenance->state_symbol->section->memory_region != "WRAM0" ||
      tv_gate_provenance->state_symbol->section->name != "Sprite State Data") {
    std::cerr << "expected RedsHouse1F TV gate provenance lookup\n";
    return 1;
  }
  if (pokered::oracle::LookupMessageProvenance(symbols, sections, pokered::MessageId::SaveOk) ||
      pokered::oracle::LookupMessageProvenance(symbols, sections, pokered::MessageId::OaksLabRival) ||
      pokered::oracle::LookupMessageSourceProvenance(symbols, sections, pokered::MessageId::SaveOk) ||
      pokered::oracle::LookupMessageSourceProvenance(symbols, sections, pokered::MessageId::OaksLabRival) ||
      pokered::oracle::LookupMoveScriptProvenance(
          symbols, sections, pokered::WorldId::PalletTown, pokered::MoveBlocker::Collision, pokered::MessageId::None) ||
      pokered::oracle::LookupMoveStateGateProvenance(
          symbols,
          sections,
          pokered::WorldId::PalletTown,
          pokered::MoveBlocker::Collision,
          pokered::MessageId::None,
          pokered::StateGate::GotStarter) ||
      pokered::oracle::LookupMoveScriptProvenance(
          symbols, sections, pokered::WorldId::OaksLab, pokered::MoveBlocker::Script, pokered::MessageId::OaksLabPokedex) ||
      pokered::oracle::LookupLastMapProvenance(symbols, sections, pokered::WorldId::PalletTown, 0) ||
      pokered::oracle::LookupLastMapProvenance(symbols, sections, pokered::WorldId::PalletTown, 4) ||
      pokered::oracle::LookupInteractionProvenance(
          symbols,
          sections,
          pokered::WorldId::PalletTown,
          pokered::MessageId::SaveOk,
          pokered::MessageId::SaveOk) ||
      pokered::oracle::LookupInteractionProvenance(
          symbols,
          sections,
          pokered::WorldId::PalletTown,
          pokered::MessageId::None,
          pokered::MessageId::None) ||
      pokered::oracle::LookupFacingMessageProvenance(symbols, sections, pokered::WorldState {
                                                                       .map_id = pokered::WorldId::PalletTown,
                                                                       .player = {10, 10, pokered::Facing::Right},
                                                                   }) ||
      pokered::oracle::LookupFacingProvenance(symbols, sections, pokered::WorldState {
                                                                .map_id = pokered::WorldId::PalletTown,
                                                                .player = {10, 10, pokered::Facing::Right},
                                                            }) ||
      pokered::oracle::LookupFacingBranchProvenance(symbols, sections, pokered::WorldState {
                                                                       .map_id = pokered::WorldId::PalletTown,
                                                                       .player = {4, 8, pokered::Facing::Left},
                                                                   }) ||
      pokered::oracle::LookupFacingBranchProvenance(symbols, sections, pokered::WorldState {
                                                                       .map_id = pokered::WorldId::PalletTown,
                                                                       .player = {10, 10, pokered::Facing::Right},
                                                                   }) ||
      pokered::oracle::LookupFacingStateGateProvenance(symbols, sections, pokered::WorldState {
                                                                          .map_id = pokered::WorldId::PalletTown,
                                                                          .player = {4, 8, pokered::Facing::Left},
                                                                      }) ||
      pokered::oracle::LookupFacingStateGateProvenance(symbols, sections, pokered::WorldState {
                                                                          .map_id = pokered::WorldId::PalletTown,
                                                                          .player = {10, 10, pokered::Facing::Right},
                                                                   }) ||
      pokered::oracle::LookupInteractionBranchProvenance(
          symbols,
          sections,
          pokered::WorldId::PalletTown,
          pokered::MessageId::PalletTownGirl,
          pokered::MessageId::PalletTownGirl) ||
      pokered::oracle::LookupInteractionBranchProvenance(
          symbols,
          sections,
          pokered::WorldId::PalletTown,
          pokered::MessageId::None,
          pokered::MessageId::None) ||
      pokered::oracle::LookupInteractionStateGateProvenance(
          symbols,
          sections,
          pokered::WorldId::PalletTown,
          pokered::MessageId::PalletTownGirl,
          pokered::MessageId::PalletTownGirl,
          pokered::StateGate::GotStarter) ||
      pokered::oracle::LookupInteractionStateGateProvenance(
          symbols,
          sections,
          pokered::WorldId::RedsHouse1F,
          pokered::MessageId::SaveOk,
          pokered::MessageId::SaveOk,
          pokered::StateGate::GotStarter)) {
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
      oak_warning.to_y != 1 || oak_warning.blocker != pokered::MoveBlocker::Script ||
      oak_warning.state_gate != pokered::StateGate::GotStarter || oak_warning.state_gate_value ||
      oak_trigger.player.x != pallet_north_exit_x || oak_trigger.player.y != 2 || oak_trigger.step_counter != 0) {
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
      route_progress.blocker != pokered::MoveBlocker::None ||
      route_progress.state_gate != pokered::StateGate::None || route_progress.state_gate_value ||
      oak_trigger.player.x != pallet_north_exit_x || oak_trigger.player.y != 1 || oak_trigger.step_counter != 1) {
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
      tv_block.to_x != 3 || tv_block.to_y != 1 || tv_block.state_gate != pokered::StateGate::None ||
      tv_block.state_gate_value || wall_block.player.x != 3 || wall_block.player.y != 2) {
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
      girl_block.to_x != 8 || girl_block.to_y != 5 || girl_block.state_gate != pokered::StateGate::None ||
      girl_block.state_gate_value || npc_block.player.x != 8 || npc_block.player.y != 6) {
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

  pokered::WorldState reds_house_world {};
  reds_house_world.map_id = pokered::WorldId::RedsHouse1F;
  reds_house_world.player = {5, 5, pokered::Facing::Up};
  const pokered::InteractionResult mom_default = pokered::InspectFacingTile(map, reds_house_world);
  if (mom_default.kind != pokered::InteractionKind::Npc ||
      mom_default.origin_message != pokered::MessageId::MomWakeUp ||
      mom_default.message != pokered::MessageId::MomWakeUp ||
      mom_default.state_gate != pokered::StateGate::GotStarter || mom_default.state_gate_value) {
    std::cerr << "expected RedsHouse1F mom default interaction result metadata\n";
    return 1;
  }
  reds_house_world.got_starter = true;
  const pokered::InteractionResult mom_rest = pokered::InspectFacingTile(map, reds_house_world);
  if (mom_rest.kind != pokered::InteractionKind::Npc ||
      mom_rest.origin_message != pokered::MessageId::MomWakeUp ||
      mom_rest.message != pokered::MessageId::MomRest ||
      mom_rest.state_gate != pokered::StateGate::GotStarter || !mom_rest.state_gate_value) {
    std::cerr << "expected RedsHouse1F mom rest interaction result metadata\n";
    return 1;
  }
  reds_house_world.got_starter = false;
  reds_house_world.player = {3, 2, pokered::Facing::Up};
  const pokered::InteractionResult tv_front = pokered::InspectFacingTile(map, reds_house_world);
  if (tv_front.kind != pokered::InteractionKind::BgEvent ||
      tv_front.origin_message != pokered::MessageId::TvMovie ||
      tv_front.message != pokered::MessageId::TvMovie ||
      tv_front.state_gate != pokered::StateGate::FacingUp || !tv_front.state_gate_value) {
    std::cerr << "expected RedsHouse1F TV front interaction result metadata\n";
    return 1;
  }
  reds_house_world.player = {2, 1, pokered::Facing::Right};
  const pokered::InteractionResult tv_side = pokered::InspectFacingTile(map, reds_house_world);
  if (tv_side.kind != pokered::InteractionKind::BgEvent ||
      tv_side.origin_message != pokered::MessageId::TvMovie ||
      tv_side.message != pokered::MessageId::TvWrongSide ||
      tv_side.state_gate != pokered::StateGate::FacingUp || tv_side.state_gate_value) {
    std::cerr << "expected RedsHouse1F TV side interaction result metadata\n";
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
      reds_entry.target_warp != 1 || reds_entry.message != pokered::MessageId::None || reds_entry.to_x != 2 ||
      reds_entry.to_y != 7 ||
      pallet_world.map_id != pokered::WorldId::RedsHouse1F || pallet_world.player.x != 2 ||
      pallet_world.player.y != 7 ||
      pallet_world.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      pallet_world.last_warp != 1 || pallet_world.player.facing != pokered::Facing::Down ||
      pallet_world.step_counter != 1) {
    std::cerr << "expected PalletTown door warp to enter RedsHouse1F\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::RedsHouse1F), 2, 6) != pokered::MoveBlocker::None) {
    std::cerr << "expected RedsHouse1F interior forward tile to remain passable\n";
    return 1;
  }
  pokered::WorldState reds_forward_step = pallet_world;
  const pokered::MoveResult reds_forward_move = pokered::TryMoveWithResult(reds_forward_step, pokered::Facing::Up);
  if (!reds_forward_move.moved || reds_forward_move.warped ||
      reds_forward_move.source_map != pokered::WorldId::RedsHouse1F || reds_forward_move.source_warp != 0 ||
      reds_forward_move.target_map != pokered::WorldId::RedsHouse1F || reds_forward_move.target_warp != 0 ||
      reds_forward_move.message != pokered::MessageId::None || reds_forward_move.to_x != 2 ||
      reds_forward_move.to_y != 6 || reds_forward_move.blocker != pokered::MoveBlocker::None ||
      reds_forward_step.map_id != pokered::WorldId::RedsHouse1F || reds_forward_step.player.x != 2 ||
      reds_forward_step.player.y != 6 || reds_forward_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      reds_forward_step.last_warp != 1 || reds_forward_step.player.facing != pokered::Facing::Up ||
      reds_forward_step.step_counter != 2) {
    std::cerr << "expected RedsHouse1F to step off the door tile into the room\n";
    return 1;
  }
  pokered::WorldState reds_left_return {};
  reds_left_return.map_id = pokered::WorldId::RedsHouse1F;
  reds_left_return.player = {2, 6, pokered::Facing::Down};
  reds_left_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  reds_left_return.last_warp = 1;
  const pokered::MoveResult reds_left_return_move =
      pokered::TryMoveWithResult(reds_left_return, pokered::Facing::Down);
  if (!reds_left_return_move.moved || !reds_left_return_move.warped ||
      reds_left_return_move.source_map != pokered::WorldId::RedsHouse1F || reds_left_return_move.source_warp != 1 ||
      reds_left_return_move.target_map != pokered::WorldId::PalletTown || reds_left_return_move.target_warp != 1 ||
      reds_left_return_move.message != pokered::MessageId::None || reds_left_return_move.to_x != 5 ||
      reds_left_return_move.to_y != 6 || reds_left_return_move.blocker != pokered::MoveBlocker::None ||
      reds_left_return.map_id != pokered::WorldId::PalletTown || reds_left_return.player.x != 5 ||
      reds_left_return.player.y != 6 ||
      reds_left_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      reds_left_return.last_warp != 1 || reds_left_return.player.facing != pokered::Facing::Down ||
      reds_left_return.step_counter != 1) {
    std::cerr << "expected RedsHouse1F left return tile to exit into PalletTown\n";
    return 1;
  }
  pokered::WorldState reds_right_return {};
  reds_right_return.map_id = pokered::WorldId::RedsHouse1F;
  reds_right_return.player = {3, 6, pokered::Facing::Down};
  reds_right_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  reds_right_return.last_warp = 1;
  const pokered::MoveResult reds_right_return_move =
      pokered::TryMoveWithResult(reds_right_return, pokered::Facing::Down);
  if (!reds_right_return_move.moved || !reds_right_return_move.warped ||
      reds_right_return_move.source_map != pokered::WorldId::RedsHouse1F ||
      reds_right_return_move.source_warp != 2 ||
      reds_right_return_move.target_map != pokered::WorldId::PalletTown ||
      reds_right_return_move.target_warp != 1 ||
      reds_right_return_move.message != pokered::MessageId::None || reds_right_return_move.to_x != 5 ||
      reds_right_return_move.to_y != 6 || reds_right_return_move.blocker != pokered::MoveBlocker::None ||
      reds_right_return.map_id != pokered::WorldId::PalletTown || reds_right_return.player.x != 5 ||
      reds_right_return.player.y != 6 ||
      reds_right_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      reds_right_return.last_warp != 2 || reds_right_return.player.facing != pokered::Facing::Down ||
      reds_right_return.step_counter != 1) {
    std::cerr << "expected RedsHouse1F right return tile to exit into PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::RedsHouse1F), 3, 6) != pokered::MoveBlocker::None) {
    std::cerr << "expected RedsHouse1F paired interior forward tile to remain passable\n";
    return 1;
  }
  pokered::WorldState reds_right_forward_step {};
  reds_right_forward_step.map_id = pokered::WorldId::RedsHouse1F;
  reds_right_forward_step.player = {3, 7, pokered::Facing::Up};
  reds_right_forward_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  reds_right_forward_step.last_warp = 1;
  const pokered::MoveResult reds_right_forward_move =
      pokered::TryMoveWithResult(reds_right_forward_step, pokered::Facing::Up);
  if (!reds_right_forward_move.moved || reds_right_forward_move.warped ||
      reds_right_forward_move.source_map != pokered::WorldId::RedsHouse1F ||
      reds_right_forward_move.source_warp != 0 || reds_right_forward_move.target_map != pokered::WorldId::RedsHouse1F ||
      reds_right_forward_move.target_warp != 0 || reds_right_forward_move.message != pokered::MessageId::None ||
      reds_right_forward_move.to_x != 3 || reds_right_forward_move.to_y != 6 ||
      reds_right_forward_move.blocker != pokered::MoveBlocker::None ||
      reds_right_forward_step.map_id != pokered::WorldId::RedsHouse1F || reds_right_forward_step.player.x != 3 ||
      reds_right_forward_step.player.y != 6 ||
      reds_right_forward_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      reds_right_forward_step.last_warp != 1 || reds_right_forward_step.player.facing != pokered::Facing::Up ||
      reds_right_forward_step.step_counter != 1) {
    std::cerr << "expected RedsHouse1F paired doorway forward step to stay inside the house\n";
    return 1;
  }
  const pokered::MoveResult reds_immediate_exit = pokered::TryMoveWithResult(pallet_world, pokered::Facing::Down);
  if (!reds_immediate_exit.moved || !reds_immediate_exit.warped ||
      reds_immediate_exit.source_map != pokered::WorldId::RedsHouse1F || reds_immediate_exit.source_warp != 1 ||
      reds_immediate_exit.target_map != pokered::WorldId::PalletTown || reds_immediate_exit.target_warp != 1 ||
      reds_immediate_exit.message != pokered::MessageId::None || reds_immediate_exit.to_x != 5 ||
      reds_immediate_exit.to_y != 6 || reds_immediate_exit.blocker != pokered::MoveBlocker::None ||
      pallet_world.map_id != pokered::WorldId::PalletTown || pallet_world.player.x != 5 ||
      pallet_world.player.y != 6 || pallet_world.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      pallet_world.last_warp != 1 || pallet_world.player.facing != pokered::Facing::Down ||
      pallet_world.step_counter != 1) {
    std::cerr << "expected RedsHouse1F immediate door re-exit into PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::RedsHouse1F), 1, 7) != pokered::MoveBlocker::None) {
    std::cerr << "expected RedsHouse1F left doorway edge to remain passable\n";
    return 1;
  }
  pokered::WorldState reds_left_step {};
  reds_left_step.map_id = pokered::WorldId::RedsHouse1F;
  reds_left_step.player = {2, 7, pokered::Facing::Left};
  reds_left_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  reds_left_step.last_warp = 1;
  const pokered::MoveResult reds_left_move = pokered::TryMoveWithResult(reds_left_step, pokered::Facing::Left);
  if (!reds_left_move.moved || reds_left_move.warped || reds_left_move.source_map != pokered::WorldId::RedsHouse1F ||
      reds_left_move.source_warp != 0 || reds_left_move.target_map != pokered::WorldId::RedsHouse1F ||
      reds_left_move.target_warp != 0 || reds_left_move.message != pokered::MessageId::None ||
      reds_left_move.to_x != 1 || reds_left_move.to_y != 7 || reds_left_move.blocker != pokered::MoveBlocker::None ||
      reds_left_step.map_id != pokered::WorldId::RedsHouse1F || reds_left_step.player.x != 1 ||
      reds_left_step.player.y != 7 ||
      reds_left_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      reds_left_step.last_warp != 1 || reds_left_step.player.facing != pokered::Facing::Left ||
      reds_left_step.step_counter != 1) {
    std::cerr << "expected RedsHouse1F left-hand doorway step to stay inside the house\n";
    return 1;
  }
  pokered::WorldState reds_entry_side_return {};
  reds_entry_side_return.map_id = pokered::WorldId::RedsHouse1F;
  reds_entry_side_return.player = {1, 7, pokered::Facing::Right};
  reds_entry_side_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  reds_entry_side_return.last_warp = 1;
  const pokered::MoveResult reds_entry_side_return_move =
      pokered::TryMoveWithResult(reds_entry_side_return, pokered::Facing::Right);
  if (!reds_entry_side_return_move.moved || !reds_entry_side_return_move.warped ||
      reds_entry_side_return_move.source_map != pokered::WorldId::RedsHouse1F ||
      reds_entry_side_return_move.source_warp != 1 ||
      reds_entry_side_return_move.target_map != pokered::WorldId::PalletTown ||
      reds_entry_side_return_move.target_warp != 1 ||
      reds_entry_side_return_move.message != pokered::MessageId::None ||
      reds_entry_side_return_move.to_x != 5 || reds_entry_side_return_move.to_y != 6 ||
      reds_entry_side_return_move.blocker != pokered::MoveBlocker::None ||
      reds_entry_side_return.map_id != pokered::WorldId::PalletTown || reds_entry_side_return.player.x != 5 ||
      reds_entry_side_return.player.y != 6 ||
      reds_entry_side_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      reds_entry_side_return.last_warp != 1 ||
      reds_entry_side_return.player.facing != pokered::Facing::Down ||
      reds_entry_side_return.step_counter != 1) {
    std::cerr << "expected RedsHouse1F lateral return onto the entry doorway to exit into PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::RedsHouse1F), 4, 7) != pokered::MoveBlocker::None) {
    std::cerr << "expected RedsHouse1F right doorway edge to remain passable\n";
    return 1;
  }
  pokered::WorldState reds_right_step {};
  reds_right_step.map_id = pokered::WorldId::RedsHouse1F;
  reds_right_step.player = {3, 7, pokered::Facing::Right};
  reds_right_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  reds_right_step.last_warp = 1;
  const pokered::MoveResult reds_right_move = pokered::TryMoveWithResult(reds_right_step, pokered::Facing::Right);
  if (!reds_right_move.moved || reds_right_move.warped || reds_right_move.source_map != pokered::WorldId::RedsHouse1F ||
      reds_right_move.source_warp != 0 || reds_right_move.target_map != pokered::WorldId::RedsHouse1F ||
      reds_right_move.target_warp != 0 || reds_right_move.message != pokered::MessageId::None ||
      reds_right_move.to_x != 4 || reds_right_move.to_y != 7 || reds_right_move.blocker != pokered::MoveBlocker::None ||
      reds_right_step.map_id != pokered::WorldId::RedsHouse1F || reds_right_step.player.x != 4 ||
      reds_right_step.player.y != 7 ||
      reds_right_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      reds_right_step.last_warp != 1 || reds_right_step.player.facing != pokered::Facing::Right ||
      reds_right_step.step_counter != 1) {
    std::cerr << "expected RedsHouse1F right-hand doorway step to stay inside the house\n";
    return 1;
  }
  pokered::WorldState reds_paired_side_return {};
  reds_paired_side_return.map_id = pokered::WorldId::RedsHouse1F;
  reds_paired_side_return.player = {4, 7, pokered::Facing::Left};
  reds_paired_side_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  reds_paired_side_return.last_warp = 1;
  const pokered::MoveResult reds_paired_side_return_move =
      pokered::TryMoveWithResult(reds_paired_side_return, pokered::Facing::Left);
  if (!reds_paired_side_return_move.moved || !reds_paired_side_return_move.warped ||
      reds_paired_side_return_move.source_map != pokered::WorldId::RedsHouse1F ||
      reds_paired_side_return_move.source_warp != 2 ||
      reds_paired_side_return_move.target_map != pokered::WorldId::PalletTown ||
      reds_paired_side_return_move.target_warp != 1 ||
      reds_paired_side_return_move.message != pokered::MessageId::None ||
      reds_paired_side_return_move.to_x != 5 || reds_paired_side_return_move.to_y != 6 ||
      reds_paired_side_return_move.blocker != pokered::MoveBlocker::None ||
      reds_paired_side_return.map_id != pokered::WorldId::PalletTown || reds_paired_side_return.player.x != 5 ||
      reds_paired_side_return.player.y != 6 ||
      reds_paired_side_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      reds_paired_side_return.last_warp != 2 ||
      reds_paired_side_return.player.facing != pokered::Facing::Down ||
      reds_paired_side_return.step_counter != 1) {
    std::cerr << "expected RedsHouse1F lateral return onto the paired doorway to exit into PalletTown\n";
    return 1;
  }
  pokered::WorldState reds_paired_left_exit {};
  reds_paired_left_exit.map_id = pokered::WorldId::RedsHouse1F;
  reds_paired_left_exit.player = {3, 7, pokered::Facing::Left};
  reds_paired_left_exit.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  reds_paired_left_exit.last_warp = 1;
  const pokered::MoveResult reds_paired_left_warp =
      pokered::TryMoveWithResult(reds_paired_left_exit, pokered::Facing::Left);
  if (!reds_paired_left_warp.moved || !reds_paired_left_warp.warped ||
      reds_paired_left_warp.source_map != pokered::WorldId::RedsHouse1F ||
      reds_paired_left_warp.source_warp != 1 ||
      reds_paired_left_warp.target_map != pokered::WorldId::PalletTown ||
      reds_paired_left_warp.target_warp != 1 ||
      reds_paired_left_warp.message != pokered::MessageId::None || reds_paired_left_warp.to_x != 5 ||
      reds_paired_left_warp.to_y != 6 || reds_paired_left_warp.blocker != pokered::MoveBlocker::None ||
      reds_paired_left_exit.map_id != pokered::WorldId::PalletTown || reds_paired_left_exit.player.x != 5 ||
      reds_paired_left_exit.player.y != 6 ||
      reds_paired_left_exit.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      reds_paired_left_exit.last_warp != 1 || reds_paired_left_exit.player.facing != pokered::Facing::Down ||
      reds_paired_left_exit.step_counter != 1) {
    std::cerr << "expected RedsHouse1F paired doorway reverse span to exit into PalletTown\n";
    return 1;
  }
  pokered::WorldState reds_right_immediate_exit {};
  reds_right_immediate_exit.map_id = pokered::WorldId::RedsHouse1F;
  reds_right_immediate_exit.player = {3, 7, pokered::Facing::Down};
  reds_right_immediate_exit.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  reds_right_immediate_exit.last_warp = 1;
  const pokered::MoveResult reds_right_blocked_exit =
      pokered::TryMoveWithResult(reds_right_immediate_exit, pokered::Facing::Down);
  if (!reds_right_blocked_exit.moved || !reds_right_blocked_exit.warped ||
      reds_right_blocked_exit.source_map != pokered::WorldId::RedsHouse1F ||
      reds_right_blocked_exit.source_warp != 2 ||
      reds_right_blocked_exit.target_map != pokered::WorldId::PalletTown ||
      reds_right_blocked_exit.target_warp != 1 || reds_right_blocked_exit.message != pokered::MessageId::None ||
      reds_right_blocked_exit.to_x != 5 || reds_right_blocked_exit.to_y != 6 ||
      reds_right_blocked_exit.blocker != pokered::MoveBlocker::None ||
      reds_right_immediate_exit.map_id != pokered::WorldId::PalletTown || reds_right_immediate_exit.player.x != 5 ||
      reds_right_immediate_exit.player.y != 6 ||
      reds_right_immediate_exit.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      reds_right_immediate_exit.last_warp != 2 ||
      reds_right_immediate_exit.player.facing != pokered::Facing::Down ||
      reds_right_immediate_exit.step_counter != 0) {
    std::cerr << "expected RedsHouse1F right-hand immediate door re-exit into PalletTown\n";
    return 1;
  }

  pokered::WorldState blues_entry {};
  blues_entry.map_id = pokered::WorldId::PalletTown;
  blues_entry.player.x = 13;
  blues_entry.player.y = 6;
  blues_entry.player.facing = pokered::Facing::Up;
  const pokered::MoveResult blues_warp = pokered::TryMoveWithResult(blues_entry, pokered::Facing::Up);
  if (!blues_warp.moved || !blues_warp.warped || blues_warp.source_map != pokered::WorldId::PalletTown ||
      blues_warp.source_warp != 2 || blues_warp.target_map != pokered::WorldId::BluesHouse ||
      blues_warp.target_warp != 1 || blues_warp.message != pokered::MessageId::None || blues_warp.to_x != 2 ||
      blues_warp.to_y != 7 || blues_entry.map_id != pokered::WorldId::BluesHouse || blues_entry.player.x != 2 ||
      blues_entry.player.y != 7 ||
      blues_entry.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      blues_entry.last_warp != 2 || blues_entry.player.facing != pokered::Facing::Down ||
      blues_entry.step_counter != 1) {
    std::cerr << "expected PalletTown door warp to enter BluesHouse\n";
    return 1;
  }
  const pokered::MoveResult blues_immediate_exit = pokered::TryMoveWithResult(blues_entry, pokered::Facing::Down);
  if (!blues_immediate_exit.moved || !blues_immediate_exit.warped ||
      blues_immediate_exit.source_map != pokered::WorldId::BluesHouse ||
      blues_immediate_exit.source_warp != 1 || blues_immediate_exit.target_map != pokered::WorldId::PalletTown ||
      blues_immediate_exit.target_warp != 2 || blues_immediate_exit.message != pokered::MessageId::None ||
      blues_immediate_exit.to_x != 13 || blues_immediate_exit.to_y != 6 ||
      blues_immediate_exit.blocker != pokered::MoveBlocker::None ||
      blues_entry.map_id != pokered::WorldId::PalletTown || blues_entry.player.x != 13 ||
      blues_entry.player.y != 6 || blues_entry.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_entry.last_warp != 1 || blues_entry.player.facing != pokered::Facing::Down ||
      blues_entry.step_counter != 1) {
    std::cerr << "expected BluesHouse immediate door re-exit into PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::BluesHouse), 4, 7) != pokered::MoveBlocker::None) {
    std::cerr << "expected BluesHouse right doorway edge to remain passable\n";
    return 1;
  }
  pokered::WorldState blues_right_step {};
  blues_right_step.map_id = pokered::WorldId::BluesHouse;
  blues_right_step.player = {3, 7, pokered::Facing::Right};
  blues_right_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_right_step.last_warp = 2;
  const pokered::MoveResult blues_right_move = pokered::TryMoveWithResult(blues_right_step, pokered::Facing::Right);
  if (!blues_right_move.moved || blues_right_move.warped || blues_right_move.source_map != pokered::WorldId::BluesHouse ||
      blues_right_move.source_warp != 0 || blues_right_move.target_map != pokered::WorldId::BluesHouse ||
      blues_right_move.target_warp != 0 || blues_right_move.message != pokered::MessageId::None ||
      blues_right_move.to_x != 4 || blues_right_move.to_y != 7 ||
      blues_right_move.blocker != pokered::MoveBlocker::None ||
      blues_right_step.map_id != pokered::WorldId::BluesHouse || blues_right_step.player.x != 4 ||
      blues_right_step.player.y != 7 ||
      blues_right_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      blues_right_step.last_warp != 2 || blues_right_step.player.facing != pokered::Facing::Right ||
      blues_right_step.step_counter != 1) {
    std::cerr << "expected BluesHouse right-hand doorway step to stay inside the house\n";
    return 1;
  }
  pokered::WorldState blues_paired_side_return {};
  blues_paired_side_return.map_id = pokered::WorldId::BluesHouse;
  blues_paired_side_return.player = {4, 7, pokered::Facing::Left};
  blues_paired_side_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_paired_side_return.last_warp = 2;
  const pokered::MoveResult blues_paired_side_return_move =
      pokered::TryMoveWithResult(blues_paired_side_return, pokered::Facing::Left);
  if (!blues_paired_side_return_move.moved || !blues_paired_side_return_move.warped ||
      blues_paired_side_return_move.source_map != pokered::WorldId::BluesHouse ||
      blues_paired_side_return_move.source_warp != 2 ||
      blues_paired_side_return_move.target_map != pokered::WorldId::PalletTown ||
      blues_paired_side_return_move.target_warp != 2 ||
      blues_paired_side_return_move.message != pokered::MessageId::None ||
      blues_paired_side_return_move.to_x != 13 || blues_paired_side_return_move.to_y != 6 ||
      blues_paired_side_return_move.blocker != pokered::MoveBlocker::None ||
      blues_paired_side_return.map_id != pokered::WorldId::PalletTown || blues_paired_side_return.player.x != 13 ||
      blues_paired_side_return.player.y != 6 ||
      blues_paired_side_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_paired_side_return.last_warp != 2 ||
      blues_paired_side_return.player.facing != pokered::Facing::Down ||
      blues_paired_side_return.step_counter != 1) {
    std::cerr << "expected BluesHouse lateral return onto the paired doorway to exit into PalletTown\n";
    return 1;
  }
  pokered::WorldState blues_paired_left_exit {};
  blues_paired_left_exit.map_id = pokered::WorldId::BluesHouse;
  blues_paired_left_exit.player = {3, 7, pokered::Facing::Left};
  blues_paired_left_exit.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_paired_left_exit.last_warp = 2;
  const pokered::MoveResult blues_paired_left_warp =
      pokered::TryMoveWithResult(blues_paired_left_exit, pokered::Facing::Left);
  if (!blues_paired_left_warp.moved || !blues_paired_left_warp.warped ||
      blues_paired_left_warp.source_map != pokered::WorldId::BluesHouse ||
      blues_paired_left_warp.source_warp != 1 ||
      blues_paired_left_warp.target_map != pokered::WorldId::PalletTown ||
      blues_paired_left_warp.target_warp != 2 ||
      blues_paired_left_warp.message != pokered::MessageId::None || blues_paired_left_warp.to_x != 13 ||
      blues_paired_left_warp.to_y != 6 || blues_paired_left_warp.blocker != pokered::MoveBlocker::None ||
      blues_paired_left_exit.map_id != pokered::WorldId::PalletTown || blues_paired_left_exit.player.x != 13 ||
      blues_paired_left_exit.player.y != 6 ||
      blues_paired_left_exit.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_paired_left_exit.last_warp != 1 || blues_paired_left_exit.player.facing != pokered::Facing::Down ||
      blues_paired_left_exit.step_counter != 1) {
    std::cerr << "expected BluesHouse paired doorway reverse span to exit into PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::BluesHouse), 1, 7) != pokered::MoveBlocker::None) {
    std::cerr << "expected BluesHouse left doorway edge to remain passable\n";
    return 1;
  }
  pokered::WorldState blues_left_step {};
  blues_left_step.map_id = pokered::WorldId::BluesHouse;
  blues_left_step.player = {2, 7, pokered::Facing::Left};
  blues_left_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_left_step.last_warp = 2;
  const pokered::MoveResult blues_left_move = pokered::TryMoveWithResult(blues_left_step, pokered::Facing::Left);
  if (!blues_left_move.moved || blues_left_move.warped || blues_left_move.source_map != pokered::WorldId::BluesHouse ||
      blues_left_move.source_warp != 0 || blues_left_move.target_map != pokered::WorldId::BluesHouse ||
      blues_left_move.target_warp != 0 || blues_left_move.message != pokered::MessageId::None ||
      blues_left_move.to_x != 1 || blues_left_move.to_y != 7 || blues_left_move.blocker != pokered::MoveBlocker::None ||
      blues_left_step.map_id != pokered::WorldId::BluesHouse || blues_left_step.player.x != 1 ||
      blues_left_step.player.y != 7 ||
      blues_left_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      blues_left_step.last_warp != 2 || blues_left_step.player.facing != pokered::Facing::Left ||
      blues_left_step.step_counter != 1) {
    std::cerr << "expected BluesHouse left-hand doorway step to stay inside the house\n";
    return 1;
  }
  pokered::WorldState blues_entry_side_return {};
  blues_entry_side_return.map_id = pokered::WorldId::BluesHouse;
  blues_entry_side_return.player = {1, 7, pokered::Facing::Right};
  blues_entry_side_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_entry_side_return.last_warp = 2;
  const pokered::MoveResult blues_entry_side_return_move =
      pokered::TryMoveWithResult(blues_entry_side_return, pokered::Facing::Right);
  if (!blues_entry_side_return_move.moved || !blues_entry_side_return_move.warped ||
      blues_entry_side_return_move.source_map != pokered::WorldId::BluesHouse ||
      blues_entry_side_return_move.source_warp != 1 ||
      blues_entry_side_return_move.target_map != pokered::WorldId::PalletTown ||
      blues_entry_side_return_move.target_warp != 2 ||
      blues_entry_side_return_move.message != pokered::MessageId::None ||
      blues_entry_side_return_move.to_x != 13 || blues_entry_side_return_move.to_y != 6 ||
      blues_entry_side_return_move.blocker != pokered::MoveBlocker::None ||
      blues_entry_side_return.map_id != pokered::WorldId::PalletTown || blues_entry_side_return.player.x != 13 ||
      blues_entry_side_return.player.y != 6 ||
      blues_entry_side_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_entry_side_return.last_warp != 1 ||
      blues_entry_side_return.player.facing != pokered::Facing::Down ||
      blues_entry_side_return.step_counter != 1) {
    std::cerr << "expected BluesHouse lateral return onto the entry doorway to exit into PalletTown\n";
    return 1;
  }
  pokered::WorldState blues_right_immediate_exit {};
  blues_right_immediate_exit.map_id = pokered::WorldId::BluesHouse;
  blues_right_immediate_exit.player = {3, 7, pokered::Facing::Down};
  blues_right_immediate_exit.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_right_immediate_exit.last_warp = 2;
  const pokered::MoveResult blues_right_blocked_exit =
      pokered::TryMoveWithResult(blues_right_immediate_exit, pokered::Facing::Down);
  if (!blues_right_blocked_exit.moved || !blues_right_blocked_exit.warped ||
      blues_right_blocked_exit.source_map != pokered::WorldId::BluesHouse ||
      blues_right_blocked_exit.source_warp != 2 ||
      blues_right_blocked_exit.target_map != pokered::WorldId::PalletTown ||
      blues_right_blocked_exit.target_warp != 2 || blues_right_blocked_exit.message != pokered::MessageId::None ||
      blues_right_blocked_exit.to_x != 13 || blues_right_blocked_exit.to_y != 6 ||
      blues_right_blocked_exit.blocker != pokered::MoveBlocker::None ||
      blues_right_immediate_exit.map_id != pokered::WorldId::PalletTown ||
      blues_right_immediate_exit.player.x != 13 || blues_right_immediate_exit.player.y != 6 ||
      blues_right_immediate_exit.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_right_immediate_exit.last_warp != 2 ||
      blues_right_immediate_exit.player.facing != pokered::Facing::Down ||
      blues_right_immediate_exit.step_counter != 0) {
    std::cerr << "expected BluesHouse right-hand immediate door re-exit into PalletTown\n";
    return 1;
  }
  pokered::WorldState blues_step {};
  blues_step.map_id = pokered::WorldId::BluesHouse;
  blues_step.player = {2, 7, pokered::Facing::Down};
  blues_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_step.last_warp = 2;
  if (!pokered::TryMove(blues_step, pokered::Facing::Up) || blues_step.map_id != pokered::WorldId::BluesHouse ||
      blues_step.player.x != 2 || blues_step.player.y != 6 || blues_step.step_counter != 1) {
    std::cerr << "expected to step off the BluesHouse door tile\n";
    return 1;
  }
  pokered::WorldState blues_left_return {};
  blues_left_return.map_id = pokered::WorldId::BluesHouse;
  blues_left_return.player = {2, 6, pokered::Facing::Down};
  blues_left_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_left_return.last_warp = 2;
  const pokered::MoveResult blues_left_return_move =
      pokered::TryMoveWithResult(blues_left_return, pokered::Facing::Down);
  if (!blues_left_return_move.moved || !blues_left_return_move.warped ||
      blues_left_return_move.source_map != pokered::WorldId::BluesHouse || blues_left_return_move.source_warp != 1 ||
      blues_left_return_move.target_map != pokered::WorldId::PalletTown || blues_left_return_move.target_warp != 2 ||
      blues_left_return_move.message != pokered::MessageId::None || blues_left_return_move.to_x != 13 ||
      blues_left_return_move.to_y != 6 || blues_left_return_move.blocker != pokered::MoveBlocker::None ||
      blues_left_return.map_id != pokered::WorldId::PalletTown || blues_left_return.player.x != 13 ||
      blues_left_return.player.y != 6 ||
      blues_left_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_left_return.last_warp != 1 || blues_left_return.player.facing != pokered::Facing::Down ||
      blues_left_return.step_counter != 1) {
    std::cerr << "expected BluesHouse left return tile to exit into PalletTown\n";
    return 1;
  }
  pokered::WorldState blues_right_return {};
  blues_right_return.map_id = pokered::WorldId::BluesHouse;
  blues_right_return.player = {3, 6, pokered::Facing::Down};
  blues_right_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_right_return.last_warp = 2;
  const pokered::MoveResult blues_right_return_move =
      pokered::TryMoveWithResult(blues_right_return, pokered::Facing::Down);
  if (!blues_right_return_move.moved || !blues_right_return_move.warped ||
      blues_right_return_move.source_map != pokered::WorldId::BluesHouse ||
      blues_right_return_move.source_warp != 2 ||
      blues_right_return_move.target_map != pokered::WorldId::PalletTown ||
      blues_right_return_move.target_warp != 2 ||
      blues_right_return_move.message != pokered::MessageId::None || blues_right_return_move.to_x != 13 ||
      blues_right_return_move.to_y != 6 || blues_right_return_move.blocker != pokered::MoveBlocker::None ||
      blues_right_return.map_id != pokered::WorldId::PalletTown || blues_right_return.player.x != 13 ||
      blues_right_return.player.y != 6 ||
      blues_right_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_right_return.last_warp != 2 || blues_right_return.player.facing != pokered::Facing::Down ||
      blues_right_return.step_counter != 1) {
    std::cerr << "expected BluesHouse right return tile to exit into PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::BluesHouse), 3, 6) != pokered::MoveBlocker::None) {
    std::cerr << "expected BluesHouse paired interior forward tile to remain passable\n";
    return 1;
  }
  pokered::WorldState blues_right_forward_step {};
  blues_right_forward_step.map_id = pokered::WorldId::BluesHouse;
  blues_right_forward_step.player = {3, 7, pokered::Facing::Up};
  blues_right_forward_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_right_forward_step.last_warp = 2;
  const pokered::MoveResult blues_right_forward_move =
      pokered::TryMoveWithResult(blues_right_forward_step, pokered::Facing::Up);
  if (!blues_right_forward_move.moved || blues_right_forward_move.warped ||
      blues_right_forward_move.source_map != pokered::WorldId::BluesHouse ||
      blues_right_forward_move.source_warp != 0 || blues_right_forward_move.target_map != pokered::WorldId::BluesHouse ||
      blues_right_forward_move.target_warp != 0 || blues_right_forward_move.message != pokered::MessageId::None ||
      blues_right_forward_move.to_x != 3 || blues_right_forward_move.to_y != 6 ||
      blues_right_forward_move.blocker != pokered::MoveBlocker::None ||
      blues_right_forward_step.map_id != pokered::WorldId::BluesHouse || blues_right_forward_step.player.x != 3 ||
      blues_right_forward_step.player.y != 6 ||
      blues_right_forward_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      blues_right_forward_step.last_warp != 2 || blues_right_forward_step.player.facing != pokered::Facing::Up ||
      blues_right_forward_step.step_counter != 1) {
    std::cerr << "expected BluesHouse paired doorway forward step to stay inside the house\n";
    return 1;
  }
  pokered::WorldState blues_right_exit {};
  blues_right_exit.map_id = pokered::WorldId::BluesHouse;
  blues_right_exit.player = {3, 6, pokered::Facing::Down};
  blues_right_exit.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  blues_right_exit.last_warp = 2;
  const pokered::MoveResult blues_right_warp = pokered::TryMoveWithResult(blues_right_exit, pokered::Facing::Down);
  if (!blues_right_warp.moved || !blues_right_warp.warped ||
      blues_right_warp.source_map != pokered::WorldId::BluesHouse ||
      blues_right_warp.source_warp != 2 || blues_right_warp.target_map != pokered::WorldId::PalletTown ||
      blues_right_warp.target_warp != 2 || blues_right_warp.message != pokered::MessageId::None ||
      blues_right_warp.to_x != 13 || blues_right_warp.to_y != 6 ||
      blues_right_exit.map_id != pokered::WorldId::PalletTown || blues_right_exit.player.x != 13 ||
      blues_right_exit.player.y != 6 ||
      blues_right_exit.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_right_exit.last_warp != 2 || blues_right_exit.player.facing != pokered::Facing::Down ||
      blues_right_exit.step_counter != 1) {
    std::cerr << "expected BluesHouse right-hand door tile to exit into PalletTown\n";
    return 1;
  }
  const pokered::MoveResult blues_reentry = pokered::TryMoveWithResult(blues_right_exit, pokered::Facing::Up);
  if (!blues_reentry.moved || !blues_reentry.warped || blues_reentry.source_map != pokered::WorldId::PalletTown ||
      blues_reentry.source_warp != 2 || blues_reentry.target_map != pokered::WorldId::BluesHouse ||
      blues_reentry.target_warp != 1 || blues_reentry.message != pokered::MessageId::None ||
      blues_reentry.to_x != 2 || blues_reentry.to_y != 7 || blues_right_exit.map_id != pokered::WorldId::BluesHouse ||
      blues_right_exit.player.x != 2 || blues_right_exit.player.y != 7 ||
      blues_right_exit.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      blues_right_exit.last_warp != 2 || blues_right_exit.player.facing != pokered::Facing::Down ||
      blues_right_exit.step_counter != 2) {
    std::cerr << "expected PalletTown house door to re-enter BluesHouse\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::PalletTown), 12, 6) != pokered::MoveBlocker::None) {
    std::cerr << "expected BluesHouse outdoor landing tile left edge to remain passable\n";
    return 1;
  }
  pokered::WorldState blues_outdoor_left_step {};
  blues_outdoor_left_step.map_id = pokered::WorldId::PalletTown;
  blues_outdoor_left_step.player = {13, 6, pokered::Facing::Left};
  blues_outdoor_left_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::BluesHouse);
  blues_outdoor_left_step.last_warp = 2;
  const pokered::MoveResult blues_outdoor_left_move =
      pokered::TryMoveWithResult(blues_outdoor_left_step, pokered::Facing::Left);
  if (!blues_outdoor_left_move.moved || blues_outdoor_left_move.warped ||
      blues_outdoor_left_move.source_map != pokered::WorldId::PalletTown ||
      blues_outdoor_left_move.source_warp != 0 || blues_outdoor_left_move.target_map != pokered::WorldId::PalletTown ||
      blues_outdoor_left_move.target_warp != 0 || blues_outdoor_left_move.message != pokered::MessageId::None ||
      blues_outdoor_left_move.to_x != 12 || blues_outdoor_left_move.to_y != 6 ||
      blues_outdoor_left_move.blocker != pokered::MoveBlocker::None ||
      blues_outdoor_left_step.map_id != pokered::WorldId::PalletTown ||
      blues_outdoor_left_step.player.x != 12 || blues_outdoor_left_step.player.y != 6 ||
      blues_outdoor_left_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_outdoor_left_step.last_warp != 2 ||
      blues_outdoor_left_step.player.facing != pokered::Facing::Left ||
      blues_outdoor_left_step.step_counter != 1) {
    std::cerr << "expected BluesHouse outdoor landing tile left step to stay in PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::PalletTown), 14, 6) != pokered::MoveBlocker::None) {
    std::cerr << "expected BluesHouse outdoor landing tile right edge to remain passable\n";
    return 1;
  }
  pokered::WorldState blues_outdoor_right_step {};
  blues_outdoor_right_step.map_id = pokered::WorldId::PalletTown;
  blues_outdoor_right_step.player = {13, 6, pokered::Facing::Right};
  blues_outdoor_right_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::BluesHouse);
  blues_outdoor_right_step.last_warp = 2;
  const pokered::MoveResult blues_outdoor_right_move =
      pokered::TryMoveWithResult(blues_outdoor_right_step, pokered::Facing::Right);
  if (!blues_outdoor_right_move.moved || blues_outdoor_right_move.warped ||
      blues_outdoor_right_move.source_map != pokered::WorldId::PalletTown ||
      blues_outdoor_right_move.source_warp != 0 ||
      blues_outdoor_right_move.target_map != pokered::WorldId::PalletTown ||
      blues_outdoor_right_move.target_warp != 0 || blues_outdoor_right_move.message != pokered::MessageId::None ||
      blues_outdoor_right_move.to_x != 14 || blues_outdoor_right_move.to_y != 6 ||
      blues_outdoor_right_move.blocker != pokered::MoveBlocker::None ||
      blues_outdoor_right_step.map_id != pokered::WorldId::PalletTown ||
      blues_outdoor_right_step.player.x != 14 || blues_outdoor_right_step.player.y != 6 ||
      blues_outdoor_right_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_outdoor_right_step.last_warp != 2 ||
      blues_outdoor_right_step.player.facing != pokered::Facing::Right ||
      blues_outdoor_right_step.step_counter != 1) {
    std::cerr << "expected BluesHouse outdoor landing tile right step to stay in PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::PalletTown), 13, 7) != pokered::MoveBlocker::None) {
    std::cerr << "expected BluesHouse outdoor landing tile forward edge to remain passable\n";
    return 1;
  }
  pokered::WorldState blues_outdoor_forward_step {};
  blues_outdoor_forward_step.map_id = pokered::WorldId::PalletTown;
  blues_outdoor_forward_step.player = {13, 6, pokered::Facing::Down};
  blues_outdoor_forward_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::BluesHouse);
  blues_outdoor_forward_step.last_warp = 2;
  const pokered::MoveResult blues_outdoor_forward_move =
      pokered::TryMoveWithResult(blues_outdoor_forward_step, pokered::Facing::Down);
  if (!blues_outdoor_forward_move.moved || blues_outdoor_forward_move.warped ||
      blues_outdoor_forward_move.source_map != pokered::WorldId::PalletTown ||
      blues_outdoor_forward_move.source_warp != 0 ||
      blues_outdoor_forward_move.target_map != pokered::WorldId::PalletTown ||
      blues_outdoor_forward_move.target_warp != 0 ||
      blues_outdoor_forward_move.message != pokered::MessageId::None ||
      blues_outdoor_forward_move.to_x != 13 || blues_outdoor_forward_move.to_y != 7 ||
      blues_outdoor_forward_move.blocker != pokered::MoveBlocker::None ||
      blues_outdoor_forward_step.map_id != pokered::WorldId::PalletTown ||
      blues_outdoor_forward_step.player.x != 13 || blues_outdoor_forward_step.player.y != 7 ||
      blues_outdoor_forward_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::BluesHouse) ||
      blues_outdoor_forward_step.last_warp != 2 ||
      blues_outdoor_forward_step.player.facing != pokered::Facing::Down ||
      blues_outdoor_forward_step.step_counter != 1) {
    std::cerr << "expected BluesHouse outdoor landing tile forward step to stay in PalletTown\n";
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
      oaks_warp.target_warp != 2 || oaks_warp.message != pokered::MessageId::None || oaks_warp.to_x != 5 ||
      oaks_warp.to_y != 11 ||
      oaks_entry.map_id != pokered::WorldId::OaksLab || oaks_entry.player.x != 5 ||
      oaks_entry.player.y != 11 ||
      oaks_entry.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      oaks_entry.last_warp != 3 || oaks_entry.player.facing != pokered::Facing::Down ||
      oaks_entry.step_counter != 1) {
    std::cerr << "expected PalletTown door warp to enter OaksLab\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::OaksLab), 5, 10) != pokered::MoveBlocker::Npc) {
    std::cerr << "expected OaksLab interior forward tile to remain NPC-blocked\n";
    return 1;
  }
  pokered::WorldState oaks_forward_exit = oaks_entry;
  const pokered::MoveResult oaks_forward_move = pokered::TryMoveWithResult(oaks_forward_exit, pokered::Facing::Up);
  if (!oaks_forward_move.moved || !oaks_forward_move.warped ||
      oaks_forward_move.source_map != pokered::WorldId::OaksLab || oaks_forward_move.source_warp != 2 ||
      oaks_forward_move.target_map != pokered::WorldId::PalletTown || oaks_forward_move.target_warp != 3 ||
      oaks_forward_move.message != pokered::MessageId::None || oaks_forward_move.to_x != 12 ||
      oaks_forward_move.to_y != 12 || oaks_forward_move.blocker != pokered::MoveBlocker::None ||
      oaks_forward_exit.map_id != pokered::WorldId::PalletTown || oaks_forward_exit.player.x != 12 ||
      oaks_forward_exit.player.y != 12 ||
      oaks_forward_exit.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_forward_exit.last_warp != 2 || oaks_forward_exit.player.facing != pokered::Facing::Down ||
      oaks_forward_exit.step_counter != 1) {
    std::cerr << "expected OaksLab interior forward NPC block to re-exit into PalletTown\n";
    return 1;
  }
  const pokered::MoveResult oaks_immediate_exit = pokered::TryMoveWithResult(oaks_entry, pokered::Facing::Down);
  if (!oaks_immediate_exit.moved || !oaks_immediate_exit.warped ||
      oaks_immediate_exit.source_map != pokered::WorldId::OaksLab || oaks_immediate_exit.source_warp != 2 ||
      oaks_immediate_exit.target_map != pokered::WorldId::PalletTown ||
      oaks_immediate_exit.target_warp != 3 || oaks_immediate_exit.message != pokered::MessageId::None ||
      oaks_immediate_exit.to_x != 12 || oaks_immediate_exit.to_y != 12 ||
      oaks_immediate_exit.blocker != pokered::MoveBlocker::None ||
      oaks_entry.map_id != pokered::WorldId::PalletTown || oaks_entry.player.x != 12 ||
      oaks_entry.player.y != 12 || oaks_entry.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_entry.last_warp != 2 || oaks_entry.player.facing != pokered::Facing::Down ||
      oaks_entry.step_counter != 1) {
    std::cerr << "expected OaksLab immediate door re-exit into PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::OaksLab), 4, 10) != pokered::MoveBlocker::None) {
    std::cerr << "expected OaksLab paired interior forward tile to remain passable\n";
    return 1;
  }
  pokered::WorldState oaks_left_forward_step {};
  oaks_left_forward_step.map_id = pokered::WorldId::OaksLab;
  oaks_left_forward_step.player = {4, 11, pokered::Facing::Up};
  oaks_left_forward_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  oaks_left_forward_step.last_warp = 3;
  const pokered::MoveResult oaks_left_forward_move =
      pokered::TryMoveWithResult(oaks_left_forward_step, pokered::Facing::Up);
  if (!oaks_left_forward_move.moved || oaks_left_forward_move.warped ||
      oaks_left_forward_move.source_map != pokered::WorldId::OaksLab ||
      oaks_left_forward_move.source_warp != 0 || oaks_left_forward_move.target_map != pokered::WorldId::OaksLab ||
      oaks_left_forward_move.target_warp != 0 || oaks_left_forward_move.message != pokered::MessageId::None ||
      oaks_left_forward_move.to_x != 4 || oaks_left_forward_move.to_y != 10 ||
      oaks_left_forward_move.blocker != pokered::MoveBlocker::None ||
      oaks_left_forward_step.map_id != pokered::WorldId::OaksLab || oaks_left_forward_step.player.x != 4 ||
      oaks_left_forward_step.player.y != 10 ||
      oaks_left_forward_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      oaks_left_forward_step.last_warp != 3 || oaks_left_forward_step.player.facing != pokered::Facing::Up ||
      oaks_left_forward_step.step_counter != 1) {
    std::cerr << "expected OaksLab paired doorway forward step to stay inside the lab\n";
    return 1;
  }
  pokered::WorldState oaks_left_return {};
  oaks_left_return.map_id = pokered::WorldId::OaksLab;
  oaks_left_return.player = {4, 10, pokered::Facing::Down};
  oaks_left_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  oaks_left_return.last_warp = 3;
  const pokered::MoveResult oaks_left_return_move =
      pokered::TryMoveWithResult(oaks_left_return, pokered::Facing::Down);
  if (!oaks_left_return_move.moved || !oaks_left_return_move.warped ||
      oaks_left_return_move.source_map != pokered::WorldId::OaksLab || oaks_left_return_move.source_warp != 1 ||
      oaks_left_return_move.target_map != pokered::WorldId::PalletTown || oaks_left_return_move.target_warp != 3 ||
      oaks_left_return_move.message != pokered::MessageId::None || oaks_left_return_move.to_x != 12 ||
      oaks_left_return_move.to_y != 12 || oaks_left_return_move.blocker != pokered::MoveBlocker::None ||
      oaks_left_return.map_id != pokered::WorldId::PalletTown || oaks_left_return.player.x != 12 ||
      oaks_left_return.player.y != 12 ||
      oaks_left_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_left_return.last_warp != 1 || oaks_left_return.player.facing != pokered::Facing::Down ||
      oaks_left_return.step_counter != 1) {
    std::cerr << "expected OaksLab left return tile to exit into PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::OaksLab), 3, 11) != pokered::MoveBlocker::None) {
    std::cerr << "expected OaksLab left doorway edge to remain passable\n";
    return 1;
  }
  pokered::WorldState oaks_left_step {};
  oaks_left_step.map_id = pokered::WorldId::OaksLab;
  oaks_left_step.player = {4, 11, pokered::Facing::Left};
  oaks_left_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  oaks_left_step.last_warp = 3;
  const pokered::MoveResult oaks_left_move = pokered::TryMoveWithResult(oaks_left_step, pokered::Facing::Left);
  if (!oaks_left_move.moved || oaks_left_move.warped || oaks_left_move.source_map != pokered::WorldId::OaksLab ||
      oaks_left_move.source_warp != 0 || oaks_left_move.target_map != pokered::WorldId::OaksLab ||
      oaks_left_move.target_warp != 0 || oaks_left_move.message != pokered::MessageId::None ||
      oaks_left_move.to_x != 3 || oaks_left_move.to_y != 11 || oaks_left_move.blocker != pokered::MoveBlocker::None ||
      oaks_left_step.map_id != pokered::WorldId::OaksLab || oaks_left_step.player.x != 3 ||
      oaks_left_step.player.y != 11 ||
      oaks_left_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      oaks_left_step.last_warp != 3 || oaks_left_step.player.facing != pokered::Facing::Left ||
      oaks_left_step.step_counter != 1) {
    std::cerr << "expected OaksLab left-hand doorway step to stay inside the lab\n";
    return 1;
  }
  pokered::WorldState oaks_paired_side_return {};
  oaks_paired_side_return.map_id = pokered::WorldId::OaksLab;
  oaks_paired_side_return.player = {3, 11, pokered::Facing::Right};
  oaks_paired_side_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  oaks_paired_side_return.last_warp = 3;
  const pokered::MoveResult oaks_paired_side_return_move =
      pokered::TryMoveWithResult(oaks_paired_side_return, pokered::Facing::Right);
  if (!oaks_paired_side_return_move.moved || !oaks_paired_side_return_move.warped ||
      oaks_paired_side_return_move.source_map != pokered::WorldId::OaksLab ||
      oaks_paired_side_return_move.source_warp != 1 ||
      oaks_paired_side_return_move.target_map != pokered::WorldId::PalletTown ||
      oaks_paired_side_return_move.target_warp != 3 ||
      oaks_paired_side_return_move.message != pokered::MessageId::None ||
      oaks_paired_side_return_move.to_x != 12 || oaks_paired_side_return_move.to_y != 12 ||
      oaks_paired_side_return_move.blocker != pokered::MoveBlocker::None ||
      oaks_paired_side_return.map_id != pokered::WorldId::PalletTown || oaks_paired_side_return.player.x != 12 ||
      oaks_paired_side_return.player.y != 12 ||
      oaks_paired_side_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_paired_side_return.last_warp != 1 ||
      oaks_paired_side_return.player.facing != pokered::Facing::Down ||
      oaks_paired_side_return.step_counter != 1) {
    std::cerr << "expected OaksLab lateral return onto the paired doorway to exit into PalletTown\n";
    return 1;
  }
  pokered::WorldState oaks_paired_right_exit {};
  oaks_paired_right_exit.map_id = pokered::WorldId::OaksLab;
  oaks_paired_right_exit.player = {4, 11, pokered::Facing::Right};
  oaks_paired_right_exit.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  oaks_paired_right_exit.last_warp = 3;
  const pokered::MoveResult oaks_paired_right_warp =
      pokered::TryMoveWithResult(oaks_paired_right_exit, pokered::Facing::Right);
  if (!oaks_paired_right_warp.moved || !oaks_paired_right_warp.warped ||
      oaks_paired_right_warp.source_map != pokered::WorldId::OaksLab ||
      oaks_paired_right_warp.source_warp != 2 ||
      oaks_paired_right_warp.target_map != pokered::WorldId::PalletTown ||
      oaks_paired_right_warp.target_warp != 3 ||
      oaks_paired_right_warp.message != pokered::MessageId::None || oaks_paired_right_warp.to_x != 12 ||
      oaks_paired_right_warp.to_y != 12 || oaks_paired_right_warp.blocker != pokered::MoveBlocker::None ||
      oaks_paired_right_exit.map_id != pokered::WorldId::PalletTown || oaks_paired_right_exit.player.x != 12 ||
      oaks_paired_right_exit.player.y != 12 ||
      oaks_paired_right_exit.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_paired_right_exit.last_warp != 2 || oaks_paired_right_exit.player.facing != pokered::Facing::Down ||
      oaks_paired_right_exit.step_counter != 1) {
    std::cerr << "expected OaksLab paired doorway reverse span to exit into PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::OaksLab), 6, 11) != pokered::MoveBlocker::None) {
    std::cerr << "expected OaksLab right doorway edge to remain passable\n";
    return 1;
  }
  pokered::WorldState oaks_right_step {};
  oaks_right_step.map_id = pokered::WorldId::OaksLab;
  oaks_right_step.player = {5, 11, pokered::Facing::Right};
  oaks_right_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  oaks_right_step.last_warp = 3;
  const pokered::MoveResult oaks_right_move = pokered::TryMoveWithResult(oaks_right_step, pokered::Facing::Right);
  if (!oaks_right_move.moved || oaks_right_move.warped || oaks_right_move.source_map != pokered::WorldId::OaksLab ||
      oaks_right_move.source_warp != 0 || oaks_right_move.target_map != pokered::WorldId::OaksLab ||
      oaks_right_move.target_warp != 0 || oaks_right_move.message != pokered::MessageId::None ||
      oaks_right_move.to_x != 6 || oaks_right_move.to_y != 11 ||
      oaks_right_move.blocker != pokered::MoveBlocker::None ||
      oaks_right_step.map_id != pokered::WorldId::OaksLab || oaks_right_step.player.x != 6 ||
      oaks_right_step.player.y != 11 ||
      oaks_right_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      oaks_right_step.last_warp != 3 || oaks_right_step.player.facing != pokered::Facing::Right ||
      oaks_right_step.step_counter != 1) {
    std::cerr << "expected OaksLab right-hand doorway step to stay inside the lab\n";
    return 1;
  }
  pokered::WorldState oaks_entry_side_return {};
  oaks_entry_side_return.map_id = pokered::WorldId::OaksLab;
  oaks_entry_side_return.player = {6, 11, pokered::Facing::Left};
  oaks_entry_side_return.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  oaks_entry_side_return.last_warp = 3;
  const pokered::MoveResult oaks_entry_side_return_move =
      pokered::TryMoveWithResult(oaks_entry_side_return, pokered::Facing::Left);
  if (!oaks_entry_side_return_move.moved || !oaks_entry_side_return_move.warped ||
      oaks_entry_side_return_move.source_map != pokered::WorldId::OaksLab ||
      oaks_entry_side_return_move.source_warp != 2 ||
      oaks_entry_side_return_move.target_map != pokered::WorldId::PalletTown ||
      oaks_entry_side_return_move.target_warp != 3 ||
      oaks_entry_side_return_move.message != pokered::MessageId::None ||
      oaks_entry_side_return_move.to_x != 12 || oaks_entry_side_return_move.to_y != 12 ||
      oaks_entry_side_return_move.blocker != pokered::MoveBlocker::None ||
      oaks_entry_side_return.map_id != pokered::WorldId::PalletTown || oaks_entry_side_return.player.x != 12 ||
      oaks_entry_side_return.player.y != 12 ||
      oaks_entry_side_return.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_entry_side_return.last_warp != 2 ||
      oaks_entry_side_return.player.facing != pokered::Facing::Down ||
      oaks_entry_side_return.step_counter != 1) {
    std::cerr << "expected OaksLab lateral return onto the entry doorway to exit into PalletTown\n";
    return 1;
  }
  pokered::WorldState oaks_left_immediate_exit {};
  oaks_left_immediate_exit.map_id = pokered::WorldId::OaksLab;
  oaks_left_immediate_exit.player = {4, 11, pokered::Facing::Down};
  oaks_left_immediate_exit.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  oaks_left_immediate_exit.last_warp = 3;
  const pokered::MoveResult oaks_left_blocked_exit =
      pokered::TryMoveWithResult(oaks_left_immediate_exit, pokered::Facing::Down);
  if (!oaks_left_blocked_exit.moved || !oaks_left_blocked_exit.warped ||
      oaks_left_blocked_exit.source_map != pokered::WorldId::OaksLab ||
      oaks_left_blocked_exit.source_warp != 1 ||
      oaks_left_blocked_exit.target_map != pokered::WorldId::PalletTown ||
      oaks_left_blocked_exit.target_warp != 3 || oaks_left_blocked_exit.message != pokered::MessageId::None ||
      oaks_left_blocked_exit.to_x != 12 || oaks_left_blocked_exit.to_y != 12 ||
      oaks_left_blocked_exit.blocker != pokered::MoveBlocker::None ||
      oaks_left_immediate_exit.map_id != pokered::WorldId::PalletTown ||
      oaks_left_immediate_exit.player.x != 12 || oaks_left_immediate_exit.player.y != 12 ||
      oaks_left_immediate_exit.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_left_immediate_exit.last_warp != 1 ||
      oaks_left_immediate_exit.player.facing != pokered::Facing::Down ||
      oaks_left_immediate_exit.step_counter != 0) {
    std::cerr << "expected OaksLab left-hand immediate door re-exit into PalletTown\n";
    return 1;
  }
  pokered::WorldState oaks_left_entry {};
  oaks_left_entry.map_id = pokered::WorldId::OaksLab;
  oaks_left_entry.player = {5, 11, pokered::Facing::Down};
  oaks_left_entry.last_map = static_cast<std::uint16_t>(pokered::WorldId::PalletTown);
  oaks_left_entry.last_warp = 3;
  const pokered::MoveResult oaks_exit = pokered::TryMoveWithResult(oaks_left_entry, pokered::Facing::Left);
  if (!oaks_exit.moved || !oaks_exit.warped || oaks_exit.source_map != pokered::WorldId::OaksLab ||
      oaks_exit.source_warp != 1 || oaks_exit.target_map != pokered::WorldId::PalletTown ||
      oaks_exit.target_warp != 3 || oaks_exit.message != pokered::MessageId::None || oaks_exit.to_x != 12 ||
      oaks_exit.to_y != 12 || oaks_left_entry.map_id != pokered::WorldId::PalletTown ||
      oaks_left_entry.player.x != 12 || oaks_left_entry.player.y != 12 ||
      oaks_left_entry.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_left_entry.last_warp != 1 || oaks_left_entry.player.facing != pokered::Facing::Down ||
      oaks_left_entry.step_counter != 1) {
    std::cerr << "expected OaksLab door warp to return to PalletTown\n";
    return 1;
  }
  const pokered::MoveResult oaks_reentry = pokered::TryMoveWithResult(oaks_left_entry, pokered::Facing::Up);
  if (!oaks_reentry.moved || !oaks_reentry.warped || oaks_reentry.source_map != pokered::WorldId::PalletTown ||
      oaks_reentry.source_warp != 3 || oaks_reentry.target_map != pokered::WorldId::OaksLab ||
      oaks_reentry.target_warp != 2 || oaks_reentry.message != pokered::MessageId::None ||
      oaks_reentry.to_x != 5 || oaks_reentry.to_y != 11 || oaks_left_entry.map_id != pokered::WorldId::OaksLab ||
      oaks_left_entry.player.x != 5 || oaks_left_entry.player.y != 11 ||
      oaks_left_entry.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      oaks_left_entry.last_warp != 3 || oaks_left_entry.player.facing != pokered::Facing::Down ||
      oaks_left_entry.step_counter != 2) {
    std::cerr << "expected PalletTown lab door to re-enter OaksLab\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::PalletTown), 11, 12) != pokered::MoveBlocker::None) {
    std::cerr << "expected OaksLab outdoor landing tile left edge to remain passable\n";
    return 1;
  }
  pokered::WorldState oaks_outdoor_left_step {};
  oaks_outdoor_left_step.map_id = pokered::WorldId::PalletTown;
  oaks_outdoor_left_step.player = {12, 12, pokered::Facing::Left};
  oaks_outdoor_left_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::OaksLab);
  oaks_outdoor_left_step.last_warp = 1;
  const pokered::MoveResult oaks_outdoor_left_move =
      pokered::TryMoveWithResult(oaks_outdoor_left_step, pokered::Facing::Left);
  if (!oaks_outdoor_left_move.moved || oaks_outdoor_left_move.warped ||
      oaks_outdoor_left_move.source_map != pokered::WorldId::PalletTown ||
      oaks_outdoor_left_move.source_warp != 0 || oaks_outdoor_left_move.target_map != pokered::WorldId::PalletTown ||
      oaks_outdoor_left_move.target_warp != 0 || oaks_outdoor_left_move.message != pokered::MessageId::None ||
      oaks_outdoor_left_move.to_x != 11 || oaks_outdoor_left_move.to_y != 12 ||
      oaks_outdoor_left_move.blocker != pokered::MoveBlocker::None ||
      oaks_outdoor_left_step.map_id != pokered::WorldId::PalletTown ||
      oaks_outdoor_left_step.player.x != 11 || oaks_outdoor_left_step.player.y != 12 ||
      oaks_outdoor_left_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_outdoor_left_step.last_warp != 1 ||
      oaks_outdoor_left_step.player.facing != pokered::Facing::Left ||
      oaks_outdoor_left_step.step_counter != 1) {
    std::cerr << "expected OaksLab outdoor landing tile left step to stay in PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::PalletTown), 13, 12) != pokered::MoveBlocker::None) {
    std::cerr << "expected OaksLab outdoor landing tile right edge to remain passable\n";
    return 1;
  }
  pokered::WorldState oaks_outdoor_right_step {};
  oaks_outdoor_right_step.map_id = pokered::WorldId::PalletTown;
  oaks_outdoor_right_step.player = {12, 12, pokered::Facing::Right};
  oaks_outdoor_right_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::OaksLab);
  oaks_outdoor_right_step.last_warp = 1;
  const pokered::MoveResult oaks_outdoor_right_move =
      pokered::TryMoveWithResult(oaks_outdoor_right_step, pokered::Facing::Right);
  if (!oaks_outdoor_right_move.moved || oaks_outdoor_right_move.warped ||
      oaks_outdoor_right_move.source_map != pokered::WorldId::PalletTown ||
      oaks_outdoor_right_move.source_warp != 0 ||
      oaks_outdoor_right_move.target_map != pokered::WorldId::PalletTown ||
      oaks_outdoor_right_move.target_warp != 0 || oaks_outdoor_right_move.message != pokered::MessageId::None ||
      oaks_outdoor_right_move.to_x != 13 || oaks_outdoor_right_move.to_y != 12 ||
      oaks_outdoor_right_move.blocker != pokered::MoveBlocker::None ||
      oaks_outdoor_right_step.map_id != pokered::WorldId::PalletTown ||
      oaks_outdoor_right_step.player.x != 13 || oaks_outdoor_right_step.player.y != 12 ||
      oaks_outdoor_right_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_outdoor_right_step.last_warp != 1 ||
      oaks_outdoor_right_step.player.facing != pokered::Facing::Right ||
      oaks_outdoor_right_step.step_counter != 1) {
    std::cerr << "expected OaksLab outdoor landing tile right step to stay in PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::PalletTown), 12, 13) !=
      pokered::MoveBlocker::Collision) {
    std::cerr << "expected OaksLab outdoor landing tile forward edge to stay collision-blocked\n";
    return 1;
  }
  pokered::WorldState oaks_outdoor_forward_step {};
  oaks_outdoor_forward_step.map_id = pokered::WorldId::PalletTown;
  oaks_outdoor_forward_step.player = {12, 12, pokered::Facing::Down};
  oaks_outdoor_forward_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::OaksLab);
  oaks_outdoor_forward_step.last_warp = 1;
  const pokered::MoveResult oaks_outdoor_forward_move =
      pokered::TryMoveWithResult(oaks_outdoor_forward_step, pokered::Facing::Down);
  if (oaks_outdoor_forward_move.moved || oaks_outdoor_forward_move.warped ||
      oaks_outdoor_forward_move.source_map != pokered::WorldId::PalletTown ||
      oaks_outdoor_forward_move.source_warp != 0 ||
      oaks_outdoor_forward_move.target_map != pokered::WorldId::PalletTown ||
      oaks_outdoor_forward_move.target_warp != 0 ||
      oaks_outdoor_forward_move.message != pokered::MessageId::None ||
      oaks_outdoor_forward_move.to_x != 12 || oaks_outdoor_forward_move.to_y != 13 ||
      oaks_outdoor_forward_move.blocker != pokered::MoveBlocker::Collision ||
      oaks_outdoor_forward_step.map_id != pokered::WorldId::PalletTown ||
      oaks_outdoor_forward_step.player.x != 12 || oaks_outdoor_forward_step.player.y != 12 ||
      oaks_outdoor_forward_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::OaksLab) ||
      oaks_outdoor_forward_step.last_warp != 1 ||
      oaks_outdoor_forward_step.player.facing != pokered::Facing::Down ||
      oaks_outdoor_forward_step.step_counter != 0) {
    std::cerr << "expected OaksLab outdoor landing tile forward move to stay blocked in PalletTown\n";
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
      house_exit.target_warp != 1 || house_exit.message != pokered::MessageId::None || house_exit.to_x != 5 ||
      house_exit.to_y != 6 ||
      warp_world.map_id != pokered::WorldId::PalletTown ||
      warp_world.player.x != 5 || warp_world.player.y != 6 ||
      warp_world.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      warp_world.last_warp != 2 || warp_world.player.facing != pokered::Facing::Down ||
      warp_world.step_counter != 1) {
    std::cerr << "expected RedsHouse1F door warp to exit into PalletTown\n";
    return 1;
  }
  const pokered::MoveResult house_reentry = pokered::TryMoveWithResult(warp_world, pokered::Facing::Up);
  if (!house_reentry.moved || !house_reentry.warped || house_reentry.source_map != pokered::WorldId::PalletTown ||
      house_reentry.source_warp != 1 || house_reentry.target_map != pokered::WorldId::RedsHouse1F ||
      house_reentry.target_warp != 1 || house_reentry.message != pokered::MessageId::None ||
      house_reentry.to_x != 2 || house_reentry.to_y != 7 || warp_world.map_id != pokered::WorldId::RedsHouse1F ||
      warp_world.player.x != 2 || warp_world.player.y != 7 ||
      warp_world.last_map != static_cast<std::uint16_t>(pokered::WorldId::PalletTown) ||
      warp_world.last_warp != 1 || warp_world.player.facing != pokered::Facing::Down ||
      warp_world.step_counter != 2) {
    std::cerr << "expected PalletTown house door to re-enter RedsHouse1F\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::PalletTown), 4, 6) != pokered::MoveBlocker::None) {
    std::cerr << "expected RedsHouse1F outdoor landing tile left edge to remain passable\n";
    return 1;
  }
  pokered::WorldState house_outdoor_left_step {};
  house_outdoor_left_step.map_id = pokered::WorldId::PalletTown;
  house_outdoor_left_step.player = {5, 6, pokered::Facing::Left};
  house_outdoor_left_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F);
  house_outdoor_left_step.last_warp = 2;
  const pokered::MoveResult house_outdoor_left_move =
      pokered::TryMoveWithResult(house_outdoor_left_step, pokered::Facing::Left);
  if (!house_outdoor_left_move.moved || house_outdoor_left_move.warped ||
      house_outdoor_left_move.source_map != pokered::WorldId::PalletTown ||
      house_outdoor_left_move.source_warp != 0 || house_outdoor_left_move.target_map != pokered::WorldId::PalletTown ||
      house_outdoor_left_move.target_warp != 0 || house_outdoor_left_move.message != pokered::MessageId::None ||
      house_outdoor_left_move.to_x != 4 || house_outdoor_left_move.to_y != 6 ||
      house_outdoor_left_move.blocker != pokered::MoveBlocker::None ||
      house_outdoor_left_step.map_id != pokered::WorldId::PalletTown ||
      house_outdoor_left_step.player.x != 4 || house_outdoor_left_step.player.y != 6 ||
      house_outdoor_left_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      house_outdoor_left_step.last_warp != 2 ||
      house_outdoor_left_step.player.facing != pokered::Facing::Left ||
      house_outdoor_left_step.step_counter != 1) {
    std::cerr << "expected RedsHouse1F outdoor landing tile left step to stay in PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::PalletTown), 6, 6) != pokered::MoveBlocker::None) {
    std::cerr << "expected RedsHouse1F outdoor landing tile right edge to remain passable\n";
    return 1;
  }
  pokered::WorldState house_outdoor_right_step {};
  house_outdoor_right_step.map_id = pokered::WorldId::PalletTown;
  house_outdoor_right_step.player = {5, 6, pokered::Facing::Right};
  house_outdoor_right_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F);
  house_outdoor_right_step.last_warp = 2;
  const pokered::MoveResult house_outdoor_right_move =
      pokered::TryMoveWithResult(house_outdoor_right_step, pokered::Facing::Right);
  if (!house_outdoor_right_move.moved || house_outdoor_right_move.warped ||
      house_outdoor_right_move.source_map != pokered::WorldId::PalletTown ||
      house_outdoor_right_move.source_warp != 0 ||
      house_outdoor_right_move.target_map != pokered::WorldId::PalletTown ||
      house_outdoor_right_move.target_warp != 0 || house_outdoor_right_move.message != pokered::MessageId::None ||
      house_outdoor_right_move.to_x != 6 || house_outdoor_right_move.to_y != 6 ||
      house_outdoor_right_move.blocker != pokered::MoveBlocker::None ||
      house_outdoor_right_step.map_id != pokered::WorldId::PalletTown ||
      house_outdoor_right_step.player.x != 6 || house_outdoor_right_step.player.y != 6 ||
      house_outdoor_right_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      house_outdoor_right_step.last_warp != 2 ||
      house_outdoor_right_step.player.facing != pokered::Facing::Right ||
      house_outdoor_right_step.step_counter != 1) {
    std::cerr << "expected RedsHouse1F outdoor landing tile right step to stay in PalletTown\n";
    return 1;
  }
  if (pokered::BlockerAt(pokered::GetMapData(pokered::WorldId::PalletTown), 5, 7) != pokered::MoveBlocker::None) {
    std::cerr << "expected RedsHouse1F outdoor landing tile forward edge to remain passable\n";
    return 1;
  }
  pokered::WorldState house_outdoor_forward_step {};
  house_outdoor_forward_step.map_id = pokered::WorldId::PalletTown;
  house_outdoor_forward_step.player = {5, 6, pokered::Facing::Down};
  house_outdoor_forward_step.last_map = static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F);
  house_outdoor_forward_step.last_warp = 2;
  const pokered::MoveResult house_outdoor_forward_move =
      pokered::TryMoveWithResult(house_outdoor_forward_step, pokered::Facing::Down);
  if (!house_outdoor_forward_move.moved || house_outdoor_forward_move.warped ||
      house_outdoor_forward_move.source_map != pokered::WorldId::PalletTown ||
      house_outdoor_forward_move.source_warp != 0 ||
      house_outdoor_forward_move.target_map != pokered::WorldId::PalletTown ||
      house_outdoor_forward_move.target_warp != 0 ||
      house_outdoor_forward_move.message != pokered::MessageId::None ||
      house_outdoor_forward_move.to_x != 5 || house_outdoor_forward_move.to_y != 7 ||
      house_outdoor_forward_move.blocker != pokered::MoveBlocker::None ||
      house_outdoor_forward_step.map_id != pokered::WorldId::PalletTown ||
      house_outdoor_forward_step.player.x != 5 || house_outdoor_forward_step.player.y != 7 ||
      house_outdoor_forward_step.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      house_outdoor_forward_step.last_warp != 2 ||
      house_outdoor_forward_step.player.facing != pokered::Facing::Down ||
      house_outdoor_forward_step.step_counter != 1) {
    std::cerr << "expected RedsHouse1F outdoor landing tile forward step to stay in PalletTown\n";
    return 1;
  }

  pokered::WorldState outdoor_walk {};
  outdoor_walk.map_id = pokered::WorldId::PalletTown;
  outdoor_walk.player.x = 4;
  outdoor_walk.player.y = 8;
  outdoor_walk.player.facing = pokered::Facing::Left;
  const pokered::InteractionResult girl_interaction = pokered::InspectFacingTile(pallet_town, outdoor_walk);
  if (girl_interaction.kind != pokered::InteractionKind::Npc ||
      girl_interaction.origin_message != pokered::MessageId::PalletTownGirl ||
      girl_interaction.message != pokered::MessageId::PalletTownGirl ||
      girl_interaction.state_gate != pokered::StateGate::None || girl_interaction.state_gate_value ||
      girl_interaction.target_x != 3 || girl_interaction.target_y != 8) {
    std::cerr << "expected PalletTown girl interaction result metadata\n";
    return 1;
  }
  if (pokered::InteractionForFacingTile(pallet_town, outdoor_walk) != pokered::MessageId::PalletTownGirl) {
    std::cerr << "expected PalletTown girl interaction\n";
    return 1;
  }
  outdoor_walk.player.x = 10;
  outdoor_walk.player.y = 14;
  outdoor_walk.player.facing = pokered::Facing::Right;
  const pokered::InteractionResult fisher_interaction = pokered::InspectFacingTile(pallet_town, outdoor_walk);
  if (fisher_interaction.kind != pokered::InteractionKind::Npc ||
      fisher_interaction.origin_message != pokered::MessageId::PalletTownFisher ||
      fisher_interaction.message != pokered::MessageId::PalletTownFisher ||
      fisher_interaction.state_gate != pokered::StateGate::None || fisher_interaction.state_gate_value ||
      fisher_interaction.target_x != 11 || fisher_interaction.target_y != 14) {
    std::cerr << "expected PalletTown fisher interaction result metadata\n";
    return 1;
  }
  if (pokered::InteractionForFacingTile(pallet_town, outdoor_walk) != pokered::MessageId::PalletTownFisher) {
    std::cerr << "expected PalletTown fisher interaction\n";
    return 1;
  }
  outdoor_walk.player.x = 13;
  outdoor_walk.player.y = 14;
  outdoor_walk.player.facing = pokered::Facing::Up;
  const pokered::InteractionResult sign_interaction = pokered::InspectFacingTile(pallet_town, outdoor_walk);
  if (sign_interaction.kind != pokered::InteractionKind::BgEvent ||
      sign_interaction.origin_message != pokered::MessageId::PalletTownOaksLabSign ||
      sign_interaction.message != pokered::MessageId::PalletTownOaksLabSign ||
      sign_interaction.state_gate != pokered::StateGate::None || sign_interaction.state_gate_value ||
      sign_interaction.target_x != 13 || sign_interaction.target_y != 13) {
    std::cerr << "expected PalletTown sign interaction result metadata\n";
    return 1;
  }
  if (pokered::InteractionForFacingTile(pallet_town, outdoor_walk) != pokered::MessageId::PalletTownOaksLabSign) {
    std::cerr << "expected PalletTown sign interaction\n";
    return 1;
  }
  outdoor_walk.player.x = 10;
  outdoor_walk.player.y = 10;
  outdoor_walk.player.facing = pokered::Facing::Right;
  const pokered::InteractionResult miss_interaction = pokered::InspectFacingTile(pallet_town, outdoor_walk);
  if (miss_interaction.kind != pokered::InteractionKind::None ||
      miss_interaction.origin_message != pokered::MessageId::None ||
      miss_interaction.message != pokered::MessageId::None ||
      miss_interaction.state_gate != pokered::StateGate::None || miss_interaction.state_gate_value ||
      miss_interaction.target_x != 11 || miss_interaction.target_y != 10) {
    std::cerr << "expected empty interaction result metadata\n";
    return 1;
  }

  oaks_world = {};
  oaks_world.map_id = pokered::WorldId::OaksLab;
  oaks_world.player = {4, 4, pokered::Facing::Up};
  const pokered::InteractionResult rival_default = pokered::InspectFacingTile(oaks_lab, oaks_world);
  if (rival_default.kind != pokered::InteractionKind::Npc ||
      rival_default.origin_message != pokered::MessageId::OaksLabRival ||
      rival_default.message != pokered::MessageId::OaksLabRivalGrampsIsntAround ||
      rival_default.state_gate != pokered::StateGate::GotStarter || rival_default.state_gate_value) {
    std::cerr << "expected OaksLab rival default interaction result metadata\n";
    return 1;
  }
  oaks_world.got_starter = true;
  const pokered::InteractionResult rival_post_starter = pokered::InspectFacingTile(oaks_lab, oaks_world);
  if (rival_post_starter.kind != pokered::InteractionKind::Npc ||
      rival_post_starter.origin_message != pokered::MessageId::OaksLabRival ||
      rival_post_starter.message != pokered::MessageId::OaksLabRivalMyPokemonLooksStronger ||
      rival_post_starter.state_gate != pokered::StateGate::GotStarter || !rival_post_starter.state_gate_value) {
    std::cerr << "expected OaksLab rival post-starter interaction result metadata\n";
    return 1;
  }
  oaks_world.got_starter = false;
  oaks_world.player = {6, 4, pokered::Facing::Up};
  const pokered::InteractionResult pokeball_default = pokered::InspectFacingTile(oaks_lab, oaks_world);
  if (pokeball_default.kind != pokered::InteractionKind::Npc ||
      pokeball_default.origin_message != pokered::MessageId::OaksLabPokeBall ||
      pokeball_default.message != pokered::MessageId::OaksLabThoseArePokeBalls ||
      pokeball_default.state_gate != pokered::StateGate::GotStarter || pokeball_default.state_gate_value) {
    std::cerr << "expected OaksLab pokeball default interaction result metadata\n";
    return 1;
  }
  oaks_world.got_starter = true;
  const pokered::InteractionResult pokeball_post_starter = pokered::InspectFacingTile(oaks_lab, oaks_world);
  if (pokeball_post_starter.kind != pokered::InteractionKind::Npc ||
      pokeball_post_starter.origin_message != pokered::MessageId::OaksLabPokeBall ||
      pokeball_post_starter.message != pokered::MessageId::OaksLabLastMon ||
      pokeball_post_starter.state_gate != pokered::StateGate::GotStarter ||
      !pokeball_post_starter.state_gate_value) {
    std::cerr << "expected OaksLab pokeball post-starter interaction result metadata\n";
    return 1;
  }
  oaks_world.got_starter = false;
  oaks_world.player = {5, 3, pokered::Facing::Up};
  const pokered::InteractionResult oak_default = pokered::InspectFacingTile(oaks_lab, oaks_world);
  if (oak_default.kind != pokered::InteractionKind::Npc ||
      oak_default.origin_message != pokered::MessageId::OaksLabOak1 ||
      oak_default.message != pokered::MessageId::OaksLabOak1WhichPokemonDoYouWant ||
      oak_default.state_gate != pokered::StateGate::GotStarter || oak_default.state_gate_value) {
    std::cerr << "expected OaksLab Oak1 default interaction result metadata\n";
    return 1;
  }
  oaks_world.got_starter = true;
  const pokered::InteractionResult oak_post_starter = pokered::InspectFacingTile(oaks_lab, oaks_world);
  if (oak_post_starter.kind != pokered::InteractionKind::Npc ||
      oak_post_starter.origin_message != pokered::MessageId::OaksLabOak1 ||
      oak_post_starter.message != pokered::MessageId::OaksLabOak1YourPokemonCanFight ||
      oak_post_starter.state_gate != pokered::StateGate::GotStarter || !oak_post_starter.state_gate_value) {
    std::cerr << "expected OaksLab Oak1 post-starter interaction result metadata\n";
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
  const pokered::MoveResult blocked_stair = pokered::TryMoveWithResult(stairs_world, pokered::Facing::Right);
  if (blocked_stair.moved || blocked_stair.warped || blocked_stair.source_map != pokered::WorldId::RedsHouse2F ||
      blocked_stair.source_warp != 0 || blocked_stair.target_map != pokered::WorldId::RedsHouse2F ||
      blocked_stair.target_warp != 0 || blocked_stair.message != pokered::MessageId::None ||
      blocked_stair.from_x != 7 || blocked_stair.from_y != 1 || blocked_stair.to_x != 8 || blocked_stair.to_y != 1 ||
      blocked_stair.blocker != pokered::MoveBlocker::Bounds || stairs_world.map_id != pokered::WorldId::RedsHouse2F ||
      stairs_world.player.x != 7 || stairs_world.player.y != 1 ||
      stairs_world.last_map != static_cast<std::uint16_t>(pokered::WorldId::RedsHouse1F) ||
      stairs_world.last_warp != 3 || stairs_world.player.facing != pokered::Facing::Right ||
      stairs_world.step_counter != 1) {
    std::cerr << "expected blocked movement on RedsHouse2F stairs to stay local\n";
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
