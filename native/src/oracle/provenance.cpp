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

std::optional<std::string_view> LabelForMessage(MessageId message_id) {
  switch (message_id) {
    case MessageId::None:
    case MessageId::OaksLabRival:
    case MessageId::OaksLabPokeBall:
    case MessageId::OaksLabOak1:
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
  }
  return std::nullopt;
}

std::optional<std::string_view> SourceLabelForMessage(MessageId message_id) {
  switch (message_id) {
    case MessageId::None:
    case MessageId::OaksLabRival:
    case MessageId::OaksLabPokeBall:
    case MessageId::OaksLabOak1:
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

}  // namespace pokered::oracle
