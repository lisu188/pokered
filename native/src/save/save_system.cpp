#include "pokered/save/save_system.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string_view>
#include <system_error>
#include <vector>

namespace pokered {
namespace {

constexpr std::string_view kMagic = "PKRNTV1";
constexpr std::uint32_t kVersion = 1;
constexpr std::size_t kHeaderSize = kMagic.size() + 1 + 4 + 4;
constexpr std::size_t kChecksumSize = 4;

void AppendU8(std::vector<std::uint8_t>& out, std::uint8_t value) {
  out.push_back(value);
}

void AppendU32(std::vector<std::uint8_t>& out, std::uint32_t value) {
  out.push_back(static_cast<std::uint8_t>(value & 0xFFu));
  out.push_back(static_cast<std::uint8_t>((value >> 8) & 0xFFu));
  out.push_back(static_cast<std::uint8_t>((value >> 16) & 0xFFu));
  out.push_back(static_cast<std::uint8_t>((value >> 24) & 0xFFu));
}

void AppendU64(std::vector<std::uint8_t>& out, std::uint64_t value) {
  for (int shift = 0; shift < 64; shift += 8) {
    out.push_back(static_cast<std::uint8_t>((value >> shift) & 0xFFu));
  }
}

std::uint32_t ReadU32(const std::vector<std::uint8_t>& bytes, std::size_t& offset) {
  const std::uint32_t value = static_cast<std::uint32_t>(bytes.at(offset)) |
                              (static_cast<std::uint32_t>(bytes.at(offset + 1)) << 8) |
                              (static_cast<std::uint32_t>(bytes.at(offset + 2)) << 16) |
                              (static_cast<std::uint32_t>(bytes.at(offset + 3)) << 24);
  offset += 4;
  return value;
}

std::uint64_t ReadU64(const std::vector<std::uint8_t>& bytes, std::size_t& offset) {
  std::uint64_t value = 0;
  for (int shift = 0; shift < 64; shift += 8) {
    value |= static_cast<std::uint64_t>(bytes.at(offset++)) << shift;
  }
  return value;
}

std::uint32_t ComputeChecksum(const std::vector<std::uint8_t>& bytes) {
  std::uint32_t checksum = 0;
  for (const std::uint8_t byte : bytes) {
    checksum = (checksum * 33u) ^ byte;
  }
  return checksum;
}

std::vector<std::uint8_t> SerializePayload(const GameState& state) {
  std::vector<std::uint8_t> bytes;
  bytes.reserve(48);
  AppendU8(bytes, static_cast<std::uint8_t>(state.scene));
  AppendU32(bytes, static_cast<std::uint32_t>(state.menu_index));
  AppendU8(bytes, static_cast<std::uint8_t>(state.world.map_id));
  AppendU32(bytes, static_cast<std::uint32_t>(state.world.player.x));
  AppendU32(bytes, static_cast<std::uint32_t>(state.world.player.y));
  AppendU8(bytes, static_cast<std::uint8_t>(state.world.player.facing));
  AppendU8(bytes, state.world.got_starter ? 1 : 0);
  AppendU32(bytes, state.world.rng_state);
  AppendU64(bytes, state.world.step_counter);
  AppendU32(bytes, state.world.last_map);
  AppendU8(bytes, state.world.last_warp);
  return bytes;
}

std::vector<std::uint8_t> SerializeSaveFile(const GameState& state) {
  const std::vector<std::uint8_t> payload = SerializePayload(state);
  std::vector<std::uint8_t> bytes;
  bytes.reserve(kHeaderSize + payload.size() + kChecksumSize);
  bytes.insert(bytes.end(), kMagic.begin(), kMagic.end());
  AppendU8(bytes, 0);
  AppendU32(bytes, kVersion);
  AppendU32(bytes, static_cast<std::uint32_t>(payload.size()));
  bytes.insert(bytes.end(), payload.begin(), payload.end());
  AppendU32(bytes, ComputeChecksum(bytes));
  return bytes;
}

}  // namespace

bool SaveSystem::Exists(const std::filesystem::path& path) {
  return std::filesystem::exists(path);
}

bool SaveSystem::Save(const std::filesystem::path& path, const GameState& state) {
  const std::vector<std::uint8_t> bytes = SerializeSaveFile(state);

  std::error_code error;
  if (!path.parent_path().empty()) {
    std::filesystem::create_directories(path.parent_path(), error);
    if (error) {
      return false;
    }
  }

  const std::filesystem::path temp_path = path.parent_path() / (path.filename().string() + ".tmp");
  std::ofstream output(temp_path, std::ios::binary | std::ios::trunc);
  if (!output) {
    return false;
  }

  output.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
  output.close();
  if (!output.good()) {
    std::filesystem::remove(temp_path, error);
    return false;
  }

  std::filesystem::rename(temp_path, path, error);
  if (!error) {
    return true;
  }

  error.clear();
  std::filesystem::remove(path, error);
  error.clear();
  std::filesystem::rename(temp_path, path, error);
  if (error) {
    std::filesystem::remove(temp_path, error);
    return false;
  }
  return true;
}

LoadResult SaveSystem::Load(const std::filesystem::path& path) {
  if (!Exists(path)) {
    return {LoadStatus::Missing, {}};
  }

  std::ifstream input(path, std::ios::binary);
  if (!input) {
    return {LoadStatus::IoError, {}};
  }

  const std::vector<std::uint8_t> bytes((std::istreambuf_iterator<char>(input)),
                                        std::istreambuf_iterator<char>());
  if (bytes.size() < kHeaderSize + kChecksumSize) {
    return {LoadStatus::Corrupt, {}};
  }

  const std::size_t checksum_offset = bytes.size() - kChecksumSize;
  const std::uint32_t stored_checksum = static_cast<std::uint32_t>(bytes.at(checksum_offset)) |
                                        (static_cast<std::uint32_t>(bytes.at(checksum_offset + 1)) << 8) |
                                        (static_cast<std::uint32_t>(bytes.at(checksum_offset + 2)) << 16) |
                                        (static_cast<std::uint32_t>(bytes.at(checksum_offset + 3)) << 24);
  std::vector<std::uint8_t> payload(bytes.begin(), bytes.begin() + static_cast<std::ptrdiff_t>(checksum_offset));
  if (ComputeChecksum(payload) != stored_checksum) {
    return {LoadStatus::Corrupt, {}};
  }

  GameState state {};
  std::size_t offset = 0;
  for (const char expected : kMagic) {
    if (payload.at(offset++) != static_cast<std::uint8_t>(expected)) {
      return {LoadStatus::Corrupt, {}};
    }
  }
  if (payload.at(offset++) != 0) {
    return {LoadStatus::Corrupt, {}};
  }

  if (ReadU32(payload, offset) != kVersion) {
    return {LoadStatus::Corrupt, {}};
  }

  const std::size_t payload_size = ReadU32(payload, offset);
  if (payload.size() != kHeaderSize + payload_size) {
    return {LoadStatus::Corrupt, {}};
  }

  state.scene = static_cast<SceneId>(payload.at(offset++));
  state.menu_index = static_cast<int>(ReadU32(payload, offset));
  state.world.map_id = static_cast<WorldId>(payload.at(offset++));
  state.world.player.x = static_cast<int>(ReadU32(payload, offset));
  state.world.player.y = static_cast<int>(ReadU32(payload, offset));
  state.world.player.facing = static_cast<Facing>(payload.at(offset++));
  state.world.got_starter = payload.at(offset++) != 0;
  state.world.rng_state = ReadU32(payload, offset);
  state.world.step_counter = ReadU64(payload, offset);
  state.world.last_map = static_cast<std::uint16_t>(ReadU32(payload, offset));
  state.world.last_warp = payload.at(offset++);
  state.has_save = true;
  state.active_message = MessageId::None;
  state.running = true;

  if (offset != payload.size()) {
    return {LoadStatus::Corrupt, {}};
  }

  return {LoadStatus::Ok, state};
}

}  // namespace pokered
