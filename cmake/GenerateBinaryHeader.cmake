if(NOT DEFINED INPUT OR NOT DEFINED OUTPUT OR NOT DEFINED VAR_NAME)
  message(FATAL_ERROR "INPUT, OUTPUT, and VAR_NAME must be provided")
endif()

file(READ "${INPUT}" BINARY_DATA HEX)
string(LENGTH "${BINARY_DATA}" HEX_LENGTH)
math(EXPR BYTE_COUNT "${HEX_LENGTH} / 2")

set(BYTES "")
foreach(INDEX RANGE 0 ${BYTE_COUNT})
  math(EXPR START "${INDEX} * 2")
  if(START GREATER_EQUAL HEX_LENGTH)
    break()
  endif()
  string(SUBSTRING "${BINARY_DATA}" "${START}" 2 HEX_BYTE)
  string(APPEND BYTES "0x${HEX_BYTE}, ")
endforeach()

file(WRITE "${OUTPUT}" "#pragma once\n\n#include <array>\n#include <cstdint>\n\nnamespace generated {\ninline constexpr std::array<std::uint8_t, ${BYTE_COUNT}> ${VAR_NAME} = { ${BYTES}};\n}  // namespace generated\n")
