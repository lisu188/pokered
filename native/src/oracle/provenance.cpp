#include "pokered/oracle/provenance.hpp"

#include <optional>
#include <string_view>
#include <utility>

#include "pokered/world/map_data.hpp"

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
    case WorldId::Route1:
      return std::pair {"Route1_h", "Route1_Object"};
  }
  return std::nullopt;
}

std::optional<std::string_view> ScriptLabelForWorld(WorldId world_id) {
  switch (world_id) {
    case WorldId::RedsHouse1F:
      return "RedsHouse1F_Script";
    case WorldId::RedsHouse2F:
      return "RedsHouse2F_Script";
    case WorldId::PewterSpeechHouse:
      return "PewterSpeechHouse_Script";
    case WorldId::PalletTown:
      return "PalletTown_Script";
    case WorldId::BluesHouse:
      return "BluesHouse_Script";
    case WorldId::OaksLab:
      return "OaksLab_Script";
    case WorldId::Route1:
      return "Route1_Script";
  }
  return std::nullopt;
}

std::optional<std::string_view> ScriptPointersLabelForWorld(WorldId world_id) {
  switch (world_id) {
    case WorldId::RedsHouse2F:
      return "RedsHouse2F_ScriptPointers";
    case WorldId::PalletTown:
      return "PalletTown_ScriptPointers";
    case WorldId::BluesHouse:
      return "BluesHouse_ScriptPointers";
    case WorldId::OaksLab:
      return "OaksLab_ScriptPointers";
    case WorldId::RedsHouse1F:
    case WorldId::PewterSpeechHouse:
    case WorldId::Route1:
      break;
  }
  return std::nullopt;
}

std::optional<std::string_view> CurrentScriptLabelForWorld(WorldId world_id) {
  switch (world_id) {
    case WorldId::RedsHouse2F:
      return "wRedsHouse2FCurScript";
    case WorldId::PalletTown:
      return "wPalletTownCurScript";
    case WorldId::BluesHouse:
      return "wBluesHouseCurScript";
    case WorldId::OaksLab:
      return "wOaksLabCurScript";
    case WorldId::RedsHouse1F:
    case WorldId::PewterSpeechHouse:
    case WorldId::Route1:
      break;
  }
  return std::nullopt;
}

std::optional<std::string_view> LabelForMessage(MessageId message_id) {
  switch (message_id) {
    case MessageId::None:
    case MessageId::OaksLabRival:
    case MessageId::OaksLabPokeBall:
    case MessageId::OaksLabOak1:
    case MessageId::Route1Youngster1:
    case MessageId::SaveOk:
    case MessageId::LoadOk:
    case MessageId::SaveMissing:
    case MessageId::SaveCorrupt:
      return std::nullopt;
    case MessageId::MomWakeUp:
      return "_RedsHouse1FMomWakeUpText";
    case MessageId::MomRest:
      return "_RedsHouse1FMomYouShouldRestText";
    case MessageId::TvMovie:
      return "_RedsHouse1FTVStandByMeMovieText";
    case MessageId::TvWrongSide:
      return "_RedsHouse1FTVWrongSideText";
    case MessageId::PewterSpeechHouseGambler:
      return "_PewterSpeechHouseGamblerText";
    case MessageId::PewterSpeechHouseYoungster:
      return "_PewterSpeechHouseYoungsterText";
    case MessageId::BluesHouseDaisyRivalAtLab:
      return "_BluesHouseDaisyRivalAtLabText";
    case MessageId::BluesHouseDaisyWalking:
      return "_BluesHouseDaisyWalkingText";
    case MessageId::BluesHouseTownMap:
      return "_BluesHouseTownMapText";
    case MessageId::OaksLabPokedex:
      return "_OaksLabPokedexText";
    case MessageId::OaksLabOak2:
      return "_OaksLabOak2Text";
    case MessageId::OaksLabGirl:
      return "_OaksLabGirlText";
    case MessageId::OaksLabScientist:
      return "_OaksLabScientistText";
    case MessageId::OaksLabRivalGrampsIsntAround:
      return "_OaksLabRivalGrampsIsntAroundText";
    case MessageId::OaksLabRivalMyPokemonLooksStronger:
      return "_OaksLabRivalMyPokemonLooksStrongerText";
    case MessageId::OaksLabThoseArePokeBalls:
      return "_OaksLabThoseArePokeBallsText";
    case MessageId::OaksLabLastMon:
      return "_OaksLabLastMonText";
    case MessageId::OaksLabOak1WhichPokemonDoYouWant:
      return "_OaksLabOak1WhichPokemonDoYouWantText";
    case MessageId::OaksLabOak1YourPokemonCanFight:
      return "_OaksLabOak1YourPokemonCanFightText";
    case MessageId::PalletTownOakHeyWaitDontGoOut:
      return "_PalletTownOakHeyWaitDontGoOutText";
    case MessageId::PalletTownOak:
      return "_PalletTownOakItsUnsafeText";
    case MessageId::PalletTownGirl:
      return "_PalletTownGirlText";
    case MessageId::PalletTownFisher:
      return "_PalletTownFisherText";
    case MessageId::PalletTownOaksLabSign:
      return "_PalletTownOaksLabSignText";
    case MessageId::PalletTownSign:
      return "_PalletTownSignText";
    case MessageId::PalletTownPlayersHouseSign:
      return "_PalletTownPlayersHouseSignText";
    case MessageId::PalletTownRivalsHouseSign:
      return "_PalletTownRivalsHouseSignText";
    case MessageId::Route1Youngster1MartSample:
      return "_Route1Youngster1MartSampleText";
    case MessageId::Route1Youngster1GotPotion:
      return "_Route1Youngster1GotPotionText";
    case MessageId::Route1Youngster1AlsoGotPokeballs:
      return "_Route1Youngster1AlsoGotPokeballsText";
    case MessageId::Route1Youngster1NoRoom:
      return "_Route1Youngster1NoRoomText";
    case MessageId::Route1Youngster2:
      return "_Route1Youngster2Text";
    case MessageId::Route1Sign:
      return "_Route1SignText";
  }
  return std::nullopt;
}

std::optional<std::string_view> SourceLabelForMessage(MessageId message_id) {
  switch (message_id) {
    case MessageId::None:
    case MessageId::OaksLabRival:
    case MessageId::OaksLabPokeBall:
    case MessageId::OaksLabOak1:
    case MessageId::Route1Youngster1:
    case MessageId::SaveOk:
    case MessageId::LoadOk:
    case MessageId::SaveMissing:
    case MessageId::SaveCorrupt:
      return std::nullopt;
    case MessageId::MomWakeUp:
      return "RedsHouse1FMomText.WakeUpText";
    case MessageId::MomRest:
      return "RedsHouse1FMomYouShouldRestText";
    case MessageId::TvMovie:
      return "RedsHouse1FTVText.StandByMeMovieText";
    case MessageId::TvWrongSide:
      return "RedsHouse1FTVText.WrongSideText";
    case MessageId::PewterSpeechHouseGambler:
      return "PewterSpeechHouseGamblerText";
    case MessageId::PewterSpeechHouseYoungster:
      return "PewterSpeechHouseYoungsterText";
    case MessageId::BluesHouseDaisyRivalAtLab:
      return "BluesHouseDaisyRivalAtLabText";
    case MessageId::BluesHouseDaisyWalking:
      return "BluesHouseDaisyWalkingText";
    case MessageId::BluesHouseTownMap:
      return "BluesHouseTownMapText";
    case MessageId::OaksLabPokedex:
      return "OaksLabPokedexText";
    case MessageId::OaksLabOak2:
      return "OaksLabOak2Text";
    case MessageId::OaksLabGirl:
      return "OaksLabGirlText";
    case MessageId::OaksLabScientist:
      return "OaksLabScientistText";
    case MessageId::OaksLabRivalGrampsIsntAround:
      return "OaksLabRivalText.GrampsIsntAroundText";
    case MessageId::OaksLabRivalMyPokemonLooksStronger:
      return "OaksLabRivalText.MyPokemonLooksStrongerText";
    case MessageId::OaksLabThoseArePokeBalls:
      return "OaksLabThoseArePokeBallsText";
    case MessageId::OaksLabLastMon:
      return "OaksLabLastMonText";
    case MessageId::OaksLabOak1WhichPokemonDoYouWant:
      return "OaksLabOak1Text.WhichPokemonDoYouWantText";
    case MessageId::OaksLabOak1YourPokemonCanFight:
      return "OaksLabOak1Text.YourPokemonCanFightText";
    case MessageId::PalletTownOakHeyWaitDontGoOut:
      return "PalletTownOakText.HeyWaitDontGoOutText";
    case MessageId::PalletTownOak:
      return "PalletTownOakText.ItsUnsafeText";
    case MessageId::PalletTownGirl:
      return "PalletTownGirlText";
    case MessageId::PalletTownFisher:
      return "PalletTownFisherText";
    case MessageId::PalletTownOaksLabSign:
      return "PalletTownOaksLabSignText";
    case MessageId::PalletTownSign:
      return "PalletTownSignText";
    case MessageId::PalletTownPlayersHouseSign:
      return "PalletTownPlayersHouseSignText";
    case MessageId::PalletTownRivalsHouseSign:
      return "PalletTownRivalsHouseSignText";
    case MessageId::Route1Youngster1MartSample:
      return "Route1Youngster1Text.MartSampleText";
    case MessageId::Route1Youngster1GotPotion:
      return "Route1Youngster1Text.GotPotionText";
    case MessageId::Route1Youngster1AlsoGotPokeballs:
      return "Route1Youngster1Text.AlsoGotPokeballsText";
    case MessageId::Route1Youngster1NoRoom:
      return "Route1Youngster1Text.NoRoomText";
    case MessageId::Route1Youngster2:
      return "Route1Youngster2Text";
    case MessageId::Route1Sign:
      return "Route1SignText";
  }
  return std::nullopt;
}

std::optional<std::string_view> LabelForMoveScript(WorldId world_id,
                                                   MoveBlocker blocker,
                                                   MessageId message_id) {
  if (blocker != MoveBlocker::Script) {
    return std::nullopt;
  }

  switch (world_id) {
    case WorldId::PalletTown:
      if (message_id == MessageId::PalletTownOakHeyWaitDontGoOut) {
        return "PalletTownDefaultScript";
      }
      break;
    case WorldId::RedsHouse1F:
    case WorldId::RedsHouse2F:
    case WorldId::PewterSpeechHouse:
    case WorldId::BluesHouse:
    case WorldId::OaksLab:
    case WorldId::Route1:
      break;
  }

  return std::nullopt;
}

std::optional<std::string_view> ConditionLabelForMoveStateGate(WorldId world_id,
                                                               MoveBlocker blocker,
                                                               MessageId message_id,
                                                               StateGate gate) {
  if (gate != StateGate::GotStarter) {
    return std::nullopt;
  }

  if (world_id == WorldId::PalletTown && blocker == MoveBlocker::Script &&
      message_id == MessageId::PalletTownOakHeyWaitDontGoOut) {
    return "EVENT_FOLLOWED_OAK_INTO_LAB";
  }

  return std::nullopt;
}

std::optional<std::string_view> ConditionLabelForInteractionStateGate(WorldId world_id,
                                                                      MessageId origin_message,
                                                                      StateGate gate) {
  switch (gate) {
    case StateGate::GotStarter:
      switch (world_id) {
        case WorldId::RedsHouse1F:
          if (origin_message == MessageId::MomWakeUp) {
            return "BIT_GOT_STARTER";
          }
          break;
        case WorldId::OaksLab:
          if (origin_message == MessageId::OaksLabRival || origin_message == MessageId::OaksLabPokeBall ||
              origin_message == MessageId::OaksLabOak1) {
            return "BIT_GOT_STARTER";
          }
          break;
        case WorldId::RedsHouse2F:
        case WorldId::PewterSpeechHouse:
        case WorldId::PalletTown:
        case WorldId::BluesHouse:
        case WorldId::Route1:
          break;
      }
      break;
    case StateGate::FacingUp:
      if (world_id == WorldId::RedsHouse1F && origin_message == MessageId::TvMovie) {
        return "SPRITE_FACING_UP";
      }
      break;
    case StateGate::None:
      break;
  }

  return std::nullopt;
}

std::optional<std::string_view> StorageLabelForInteractionStateGate(WorldId world_id,
                                                                    MessageId origin_message,
                                                                    StateGate gate) {
  switch (gate) {
    case StateGate::GotStarter:
      switch (world_id) {
        case WorldId::RedsHouse1F:
          if (origin_message == MessageId::MomWakeUp) {
            return "wStatusFlags4";
          }
          break;
        case WorldId::OaksLab:
          if (origin_message == MessageId::OaksLabRival || origin_message == MessageId::OaksLabPokeBall ||
              origin_message == MessageId::OaksLabOak1) {
            return "wStatusFlags4";
          }
          break;
        case WorldId::RedsHouse2F:
        case WorldId::PewterSpeechHouse:
        case WorldId::PalletTown:
        case WorldId::BluesHouse:
        case WorldId::Route1:
          break;
      }
      break;
    case StateGate::FacingUp:
      if (world_id == WorldId::RedsHouse1F && origin_message == MessageId::TvMovie) {
        return "wSpritePlayerStateData1FacingDirection";
      }
      break;
    case StateGate::None:
      break;
  }

  return std::nullopt;
}

std::optional<std::string_view> LabelForInteractionBranch(WorldId world_id,
                                                          MessageId origin_message,
                                                          MessageId message_id) {
  switch (world_id) {
    case WorldId::RedsHouse1F:
      if (origin_message == MessageId::MomWakeUp) {
        if (message_id == MessageId::MomWakeUp) {
          return "RedsHouse1FMomText";
        }
        if (message_id == MessageId::MomRest) {
          return "RedsHouse1FMomText.heal";
        }
      }
      if (origin_message == MessageId::TvMovie &&
          (message_id == MessageId::TvMovie || message_id == MessageId::TvWrongSide)) {
        return "RedsHouse1FTVText";
      }
      break;
    case WorldId::BluesHouse:
      if (origin_message == MessageId::BluesHouseDaisyRivalAtLab &&
          message_id == MessageId::BluesHouseDaisyRivalAtLab) {
        return "BluesHouseDaisySittingText";
      }
      break;
    case WorldId::OaksLab:
      if (origin_message == MessageId::OaksLabRival) {
        if (message_id == MessageId::OaksLabRivalGrampsIsntAround) {
          return "OaksLabRivalText";
        }
        if (message_id == MessageId::OaksLabRivalMyPokemonLooksStronger) {
          return "OaksLabRivalText.afterChooseMon";
        }
      }
      if (origin_message == MessageId::OaksLabPokeBall) {
        if (message_id == MessageId::OaksLabThoseArePokeBalls) {
          return "OaksLabSelectedPokeBallScript";
        }
        if (message_id == MessageId::OaksLabLastMon) {
          return "OaksLabLastMonScript";
        }
      }
      if (origin_message == MessageId::OaksLabOak1) {
        if (message_id == MessageId::OaksLabOak1WhichPokemonDoYouWant) {
          return "OaksLabOak1Text.check_for_poke_balls";
        }
        if (message_id == MessageId::OaksLabOak1YourPokemonCanFight) {
          return "OaksLabOak1Text.already_got_pokemon";
        }
      }
      break;
    case WorldId::Route1:
      if (origin_message == MessageId::Route1Youngster1) {
        if (message_id == MessageId::Route1Youngster1MartSample) {
          return "Route1Youngster1Text.MartSampleText";
        }
        if (message_id == MessageId::Route1Youngster1GotPotion) {
          return "Route1Youngster1Text.GotPotionText";
        }
        if (message_id == MessageId::Route1Youngster1AlsoGotPokeballs) {
          return "Route1Youngster1Text.AlsoGotPokeballsText";
        }
        if (message_id == MessageId::Route1Youngster1NoRoom) {
          return "Route1Youngster1Text.NoRoomText";
        }
      }
      break;
    case WorldId::RedsHouse2F:
    case WorldId::PewterSpeechHouse:
    case WorldId::PalletTown:
      break;
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

std::optional<MapScriptProvenance> LookupMapScriptProvenance(const SymbolTable& symbols,
                                                             const MapSections& sections,
                                                             WorldId world_id) {
  const auto script_label = ScriptLabelForWorld(world_id);
  if (!script_label) {
    return std::nullopt;
  }

  const auto script = LookupSymbolProvenance(symbols, sections, *script_label);
  if (!script) {
    return std::nullopt;
  }

  std::optional<ProvenanceSymbol> script_pointers;
  if (const auto label = ScriptPointersLabelForWorld(world_id)) {
    script_pointers = LookupSymbolProvenance(symbols, sections, *label);
  }

  std::optional<ProvenanceSymbol> current_script;
  if (const auto label = CurrentScriptLabelForWorld(world_id)) {
    current_script = LookupSymbolProvenance(symbols, sections, *label);
  }

  return MapScriptProvenance {
      .world_id = world_id,
      .script = *script,
      .script_pointers = script_pointers,
      .current_script = current_script,
  };
}

std::optional<LastMapProvenance> LookupLastMapProvenance(const SymbolTable& symbols,
                                                         const MapSections& sections,
                                                         WorldId world_id,
                                                         std::uint8_t warp_id) {
  if (warp_id == 0 || !HasMapData(world_id)) {
    return std::nullopt;
  }

  const MapData& map = GetMapData(world_id);
  if (warp_id > map.warps.size()) {
    return std::nullopt;
  }

  const auto provenance = LookupMapProvenance(symbols, sections, world_id);
  if (!provenance) {
    return std::nullopt;
  }

  return LastMapProvenance {
      .world_id = world_id,
      .warp_id = warp_id,
      .object = provenance->object,
  };
}

std::optional<StateGateProvenance> LookupMoveStateGateProvenance(const SymbolTable& symbols,
                                                                 const MapSections& sections,
                                                                 WorldId world_id,
                                                                 MoveBlocker blocker,
                                                                 MessageId message_id,
                                                                 StateGate gate) {
  const auto condition = ConditionLabelForMoveStateGate(world_id, blocker, message_id, gate);
  if (!condition) {
    return std::nullopt;
  }

  std::optional<ProvenanceSymbol> context_symbol;
  if (const auto label = LabelForMoveScript(world_id, blocker, message_id)) {
    context_symbol = LookupSymbolProvenance(symbols, sections, *label);
  }

  return StateGateProvenance {
      .world_id = world_id,
      .gate = gate,
      .condition_label = std::string(*condition),
      .state_symbol = std::nullopt,
      .context_symbol = context_symbol,
  };
}

std::optional<StateGateProvenance> LookupInteractionStateGateProvenance(const SymbolTable& symbols,
                                                                        const MapSections& sections,
                                                                        WorldId world_id,
                                                                        MessageId origin_message,
                                                                        MessageId message_id,
                                                                        StateGate gate) {
  (void)message_id;

  const auto condition = ConditionLabelForInteractionStateGate(world_id, origin_message, gate);
  const auto storage = StorageLabelForInteractionStateGate(world_id, origin_message, gate);
  if (!condition || !storage) {
    return std::nullopt;
  }

  const auto state_symbol = LookupSymbolProvenance(symbols, sections, *storage);
  if (!state_symbol) {
    return std::nullopt;
  }

  return StateGateProvenance {
      .world_id = world_id,
      .gate = gate,
      .condition_label = std::string(*condition),
      .state_symbol = *state_symbol,
      .context_symbol = std::nullopt,
  };
}

std::optional<FacingProvenance> LookupFacingProvenance(const SymbolTable& symbols,
                                                       const MapSections& sections,
                                                       const WorldState& world) {
  if (!HasMapData(world.map_id)) {
    return std::nullopt;
  }

  const InteractionResult result = InspectFacingTile(GetMapData(world.map_id), world);
  if (result.kind == InteractionKind::None || result.message == MessageId::None) {
    return std::nullopt;
  }

  const auto provenance =
      LookupInteractionProvenance(symbols, sections, world.map_id, result.origin_message, result.message);
  if (!provenance) {
    return std::nullopt;
  }

  return FacingProvenance {
      .world_id = world.map_id,
      .facing = world.player.facing,
      .kind = result.kind,
      .target_x = result.target_x,
      .target_y = result.target_y,
      .origin_message = result.origin_message,
      .message_id = result.message,
      .object = provenance->object,
      .source = provenance->source,
  };
}

std::optional<FacingMessageProvenance> LookupFacingMessageProvenance(const SymbolTable& symbols,
                                                                     const MapSections& sections,
                                                                     const WorldState& world) {
  if (!HasMapData(world.map_id)) {
    return std::nullopt;
  }

  const InteractionResult result = InspectFacingTile(GetMapData(world.map_id), world);
  if (result.kind == InteractionKind::None || result.message == MessageId::None) {
    return std::nullopt;
  }

  const auto provenance = LookupMessageProvenance(symbols, sections, result.message);
  if (!provenance) {
    return std::nullopt;
  }

  return FacingMessageProvenance {
      .world_id = world.map_id,
      .facing = world.player.facing,
      .kind = result.kind,
      .target_x = result.target_x,
      .target_y = result.target_y,
      .origin_message = result.origin_message,
      .message_id = result.message,
      .text = provenance->text,
  };
}

std::optional<FacingBranchProvenance> LookupFacingBranchProvenance(const SymbolTable& symbols,
                                                                   const MapSections& sections,
                                                                   const WorldState& world) {
  if (!HasMapData(world.map_id)) {
    return std::nullopt;
  }

  const InteractionResult result = InspectFacingTile(GetMapData(world.map_id), world);
  if (result.kind == InteractionKind::None || result.message == MessageId::None) {
    return std::nullopt;
  }

  const auto provenance =
      LookupInteractionBranchProvenance(symbols, sections, world.map_id, result.origin_message, result.message);
  if (!provenance) {
    return std::nullopt;
  }

  return FacingBranchProvenance {
      .world_id = world.map_id,
      .facing = world.player.facing,
      .target_x = result.target_x,
      .target_y = result.target_y,
      .origin_message = result.origin_message,
      .message_id = result.message,
      .branch = provenance->branch,
  };
}

std::optional<FacingStateGateProvenance> LookupFacingStateGateProvenance(const SymbolTable& symbols,
                                                                         const MapSections& sections,
                                                                         const WorldState& world) {
  if (!HasMapData(world.map_id)) {
    return std::nullopt;
  }

  const InteractionResult result = InspectFacingTile(GetMapData(world.map_id), world);
  if (result.kind == InteractionKind::None || result.message == MessageId::None ||
      result.state_gate == StateGate::None) {
    return std::nullopt;
  }

  const auto provenance = LookupInteractionStateGateProvenance(
      symbols, sections, world.map_id, result.origin_message, result.message, result.state_gate);
  if (!provenance) {
    return std::nullopt;
  }

  return FacingStateGateProvenance {
      .world_id = world.map_id,
      .facing = world.player.facing,
      .target_x = result.target_x,
      .target_y = result.target_y,
      .origin_message = result.origin_message,
      .message_id = result.message,
      .gate_value = result.state_gate_value,
      .gate_source = *provenance,
  };
}

std::optional<MessageProvenance> LookupMessageProvenance(const SymbolTable& symbols,
                                                         const MapSections& sections,
                                                         MessageId message_id) {
  const auto label = LabelForMessage(message_id);
  if (!label) {
    return std::nullopt;
  }

  const auto text = LookupSymbolProvenance(symbols, sections, *label);
  if (!text) {
    return std::nullopt;
  }

  return MessageProvenance {
      .message_id = message_id,
      .text = *text,
  };
}

std::optional<MessageSourceProvenance> LookupMessageSourceProvenance(const SymbolTable& symbols,
                                                                     const MapSections& sections,
                                                                     MessageId message_id) {
  const auto label = SourceLabelForMessage(message_id);
  if (!label) {
    return std::nullopt;
  }

  const auto source = LookupSymbolProvenance(symbols, sections, *label);
  if (!source) {
    return std::nullopt;
  }

  return MessageSourceProvenance {
      .message_id = message_id,
      .source = *source,
  };
}

std::optional<MoveScriptProvenance> LookupMoveScriptProvenance(const SymbolTable& symbols,
                                                               const MapSections& sections,
                                                               WorldId world_id,
                                                               MoveBlocker blocker,
                                                               MessageId message_id) {
  const auto label = LabelForMoveScript(world_id, blocker, message_id);
  if (!label) {
    return std::nullopt;
  }

  const auto script = LookupSymbolProvenance(symbols, sections, *label);
  if (!script) {
    return std::nullopt;
  }

  return MoveScriptProvenance {
      .world_id = world_id,
      .blocker = blocker,
      .message_id = message_id,
      .script = *script,
  };
}

std::optional<InteractionProvenance> LookupInteractionProvenance(const SymbolTable& symbols,
                                                                 const MapSections& sections,
                                                                 WorldId world_id,
                                                                 MessageId origin_message,
                                                                 MessageId message_id) {
  const auto map = LookupMapProvenance(symbols, sections, world_id);
  const auto source = LookupMessageSourceProvenance(symbols, sections, message_id);
  if (!map || !source) {
    return std::nullopt;
  }

  return InteractionProvenance {
      .world_id = world_id,
      .origin_message = origin_message,
      .message_id = message_id,
      .object = map->object,
      .source = source->source,
  };
}

std::optional<InteractionBranchProvenance> LookupInteractionBranchProvenance(const SymbolTable& symbols,
                                                                             const MapSections& sections,
                                                                             WorldId world_id,
                                                                             MessageId origin_message,
                                                                             MessageId message_id) {
  const auto label = LabelForInteractionBranch(world_id, origin_message, message_id);
  if (!label) {
    return std::nullopt;
  }

  const auto branch = LookupSymbolProvenance(symbols, sections, *label);
  if (!branch) {
    return std::nullopt;
  }

  return InteractionBranchProvenance {
      .world_id = world_id,
      .origin_message = origin_message,
      .message_id = message_id,
      .branch = *branch,
  };
}

}  // namespace pokered::oracle
