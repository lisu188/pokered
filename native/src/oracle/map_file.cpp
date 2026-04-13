#include "pokered/oracle/map_file.hpp"

#include <cctype>
#include <fstream>
#include <sstream>
#include <string_view>

namespace pokered::oracle {
namespace {

struct BankContext {
  std::string memory_region;
  std::uint16_t bank = 0;
};

std::string_view TrimLeadingWhitespace(std::string_view text) {
  while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front())) != 0) {
    text.remove_prefix(1);
  }
  return text;
}

std::optional<std::uint16_t> ParseHex16(std::string_view text) {
  unsigned int value = 0;
  std::stringstream stream;
  stream << std::hex << text;
  stream >> value;
  if (stream.fail() || value > 0xFFFFu) {
    return std::nullopt;
  }
  return static_cast<std::uint16_t>(value);
}

std::optional<std::uint16_t> ParseDecimal16(std::string_view text) {
  unsigned int value = 0;
  std::stringstream stream;
  stream << text;
  stream >> value;
  if (stream.fail() || value > 0xFFFFu) {
    return std::nullopt;
  }
  return static_cast<std::uint16_t>(value);
}

std::optional<BankContext> ParseBankHeader(std::string_view line) {
  line = TrimLeadingWhitespace(line);
  if (line.empty() || line == "SUMMARY:" || line.starts_with("SECTION:") || line.starts_with("EMPTY:") ||
      line.starts_with("TOTAL EMPTY:") || line.back() != ':') {
    return std::nullopt;
  }

  const std::size_t bank_marker = line.find(" bank #");
  if (bank_marker == std::string_view::npos) {
    return BankContext {std::string(line.substr(0, line.size() - 1)), 0};
  }

  const std::size_t bank_start = bank_marker + 7;
  const std::size_t bank_end = line.find(':', bank_start);
  if (bank_end == std::string_view::npos || bank_end <= bank_start) {
    return std::nullopt;
  }

  const auto bank = ParseDecimal16(line.substr(bank_start, bank_end - bank_start));
  if (!bank) {
    return std::nullopt;
  }

  return BankContext {std::string(line.substr(0, bank_marker)), *bank};
}

std::optional<MapSection> ParseSectionLine(std::string_view line, const BankContext& bank_context) {
  line = TrimLeadingWhitespace(line);
  constexpr std::string_view kSectionPrefix = "SECTION: $";
  if (!line.starts_with(kSectionPrefix)) {
    return std::nullopt;
  }

  line.remove_prefix(kSectionPrefix.size());
  const std::size_t span_end = line.find(' ');
  if (span_end == std::string_view::npos) {
    return std::nullopt;
  }

  const std::string_view span = line.substr(0, span_end);
  std::uint16_t start = 0;
  std::uint16_t end = 0;
  const std::size_t range_separator = span.find("-$");
  if (range_separator == std::string_view::npos) {
    const auto single = ParseHex16(span);
    if (!single) {
      return std::nullopt;
    }
    start = *single;
    end = *single;
  } else {
    const auto parsed_start = ParseHex16(span.substr(0, range_separator));
    const auto parsed_end = ParseHex16(span.substr(range_separator + 2));
    if (!parsed_start || !parsed_end) {
      return std::nullopt;
    }
    start = *parsed_start;
    end = *parsed_end;
  }

  const std::size_t name_start = line.find("[\"");
  const std::size_t name_end = line.rfind("\"]");
  if (name_start == std::string_view::npos || name_end == std::string_view::npos || name_end <= name_start + 2) {
    return std::nullopt;
  }

  return MapSection {
      bank_context.memory_region,
      bank_context.bank,
      start,
      end,
      std::string(line.substr(name_start + 2, name_end - name_start - 2)),
  };
}

}  // namespace

MapSections MapFile::Load(const std::filesystem::path& path) {
  std::ifstream input(path);
  MapSections sections;
  if (!input) {
    return sections;
  }

  std::optional<BankContext> current_bank;
  std::string line;
  while (std::getline(input, line)) {
    if (const auto bank = ParseBankHeader(line)) {
      current_bank = *bank;
      continue;
    }

    if (!current_bank) {
      continue;
    }

    if (const auto section = ParseSectionLine(line, *current_bank)) {
      sections.push_back(*section);
    }
  }

  return sections;
}

std::optional<MapSection> MapFile::Lookup(const MapSections& sections, std::uint16_t bank, std::uint16_t address) {
  for (const MapSection& section : sections) {
    if (section.bank == bank && address >= section.start && address <= section.end) {
      return section;
    }
  }
  return std::nullopt;
}

std::optional<MapSection> MapFile::Lookup(const std::filesystem::path& path,
                                          std::uint16_t bank,
                                          std::uint16_t address) {
  return Lookup(Load(path), bank, address);
}

}  // namespace pokered::oracle
