#include "pokered/oracle/symbol_file.hpp"

#include <fstream>
#include <sstream>

namespace pokered::oracle {
namespace {

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

std::optional<SymbolAddress> ParseSymbolAddress(std::string_view token) {
  const std::size_t colon = token.find(':');
  if (colon == std::string_view::npos) {
    return std::nullopt;
  }

  const auto bank = ParseHex16(token.substr(0, colon));
  const auto address = ParseHex16(token.substr(colon + 1));
  if (!bank || !address) {
    return std::nullopt;
  }

  return SymbolAddress {*bank, *address};
}

}  // namespace

SymbolTable SymbolFile::Load(const std::filesystem::path& path) {
  std::ifstream input(path);
  SymbolTable table;
  if (!input) {
    return table;
  }

  std::string line;
  while (std::getline(input, line)) {
    if (line.empty() || line.front() == ';') {
      continue;
    }

    const std::size_t separator = line.find(' ');
    if (separator == std::string::npos) {
      continue;
    }

    const std::string_view address_token(line.data(), separator);
    const std::string_view name_token(line.data() + separator + 1, line.size() - separator - 1);
    const auto parsed = ParseSymbolAddress(address_token);
    if (!parsed || name_token.empty()) {
      continue;
    }

    table.emplace(std::string(name_token), *parsed);
  }

  return table;
}

std::optional<SymbolAddress> SymbolFile::Lookup(const std::filesystem::path& path, std::string_view name) {
  const SymbolTable table = Load(path);
  const auto found = table.find(std::string(name));
  if (found == table.end()) {
    return std::nullopt;
  }
  return found->second;
}

}  // namespace pokered::oracle
