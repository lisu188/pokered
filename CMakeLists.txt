cmake_minimum_required(VERSION 3.24)

project(pokered_native LANGUAGES CXX)

option(POKERED_NATIVE_BUILD_TESTS "Build native smoke/unit tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(POKERED_NATIVE_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${POKERED_NATIVE_GENERATED_DIR}")

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/reds_house_1f_blk.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/maps/RedsHouse1F.blk
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/reds_house_1f_blk.hpp
            -DVAR_NAME=kRedsHouse1FBlocks
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/maps/RedsHouse1F.blk"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for RedsHouse1F block data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/reds_house_2f_blk.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/maps/RedsHouse2F.blk
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/reds_house_2f_blk.hpp
            -DVAR_NAME=kRedsHouse2FBlocks
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/maps/RedsHouse2F.blk"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for RedsHouse2F block data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/pewter_speech_house_blk.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/maps/PewterSpeechHouse.blk
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/pewter_speech_house_blk.hpp
            -DVAR_NAME=kPewterSpeechHouseBlocks
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/maps/PewterSpeechHouse.blk"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for PewterSpeechHouse block data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/blues_house_blk.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/maps/BluesHouse.blk
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/blues_house_blk.hpp
            -DVAR_NAME=kBluesHouseBlocks
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/maps/BluesHouse.blk"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for BluesHouse block data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/oaks_lab_blk.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/maps/OaksLab.blk
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/oaks_lab_blk.hpp
            -DVAR_NAME=kOaksLabBlocks
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/maps/OaksLab.blk"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for OaksLab block data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/pallet_town_blk.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/maps/PalletTown.blk
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/pallet_town_blk.hpp
            -DVAR_NAME=kPalletTownBlocks
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/maps/PalletTown.blk"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for PalletTown block data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/route_1_blk.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/maps/Route1.blk
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/route_1_blk.hpp
            -DVAR_NAME=kRoute1Blocks
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/maps/Route1.blk"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for Route1 block data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/reds_house_blockset.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/gfx/blocksets/reds_house.bst
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/reds_house_blockset.hpp
            -DVAR_NAME=kRedsHouseBlockset
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/gfx/blocksets/reds_house.bst"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for RedsHouse blockset data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/house_blockset.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/gfx/blocksets/house.bst
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/house_blockset.hpp
            -DVAR_NAME=kHouseBlockset
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/gfx/blocksets/house.bst"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for House blockset data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/dojo_blockset.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/gfx/blocksets/gym.bst
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/dojo_blockset.hpp
            -DVAR_NAME=kDojoBlockset
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/gfx/blocksets/gym.bst"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for Dojo blockset data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/overworld_blockset.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DINPUT=${CMAKE_CURRENT_SOURCE_DIR}/gfx/blocksets/overworld.bst
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/overworld_blockset.hpp
            -DVAR_NAME=kOverworldBlockset
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/gfx/blocksets/overworld.bst"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBinaryHeader.cmake"
    COMMENT "Generating native header for Overworld blockset data"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/reds_house_1f_metadata.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DOBJECTS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/RedsHouse1F.asm
            -DTEXT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/text/RedsHouse1F.asm
            -DCOLLISION_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/reds_house_1f_metadata.hpp
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateRedsHouse1FMetadata.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/RedsHouse1F.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/text/RedsHouse1F.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateRedsHouse1FMetadata.cmake"
    COMMENT "Generating native header for RedsHouse1F metadata"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/pewter_speech_house_metadata.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DOBJECTS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/PewterSpeechHouse.asm
            -DTEXT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/text/PewterSpeechHouse.asm
            -DCOLLISION_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/pewter_speech_house_metadata.hpp
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GeneratePewterSpeechHouseMetadata.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/PewterSpeechHouse.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/text/PewterSpeechHouse.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GeneratePewterSpeechHouseMetadata.cmake"
    COMMENT "Generating native header for PewterSpeechHouse metadata"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/blues_house_metadata.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DOBJECTS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/BluesHouse.asm
            -DTEXT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/text/BluesHouse.asm
            -DCOLLISION_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/blues_house_metadata.hpp
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBluesHouseMetadata.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/BluesHouse.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/text/BluesHouse.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateBluesHouseMetadata.cmake"
    COMMENT "Generating native header for BluesHouse metadata"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/oaks_lab_metadata.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DOBJECTS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/OaksLab.asm
            -DTEXT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/text/OaksLab.asm
            -DCOLLISION_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/oaks_lab_metadata.hpp
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateOaksLabMetadata.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/OaksLab.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/text/OaksLab.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateOaksLabMetadata.cmake"
    COMMENT "Generating native header for OaksLab metadata"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/reds_house_2f_metadata.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DOBJECTS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/RedsHouse2F.asm
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/reds_house_2f_metadata.hpp
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateRedsHouse2FMetadata.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/RedsHouse2F.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateRedsHouse2FMetadata.cmake"
    COMMENT "Generating native header for RedsHouse2F metadata"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/pallet_town_metadata.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DOBJECTS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/PalletTown.asm
            -DHEADER_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/maps/headers/PalletTown.asm
            -DTEXT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/text/PalletTown.asm
            -DCOLLISION_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/pallet_town_metadata.hpp
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GeneratePalletTownMetadata.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/PalletTown.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/maps/headers/PalletTown.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/text/PalletTown.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GeneratePalletTownMetadata.cmake"
    COMMENT "Generating native header for PalletTown metadata"
    VERBATIM
)

add_custom_command(
    OUTPUT "${POKERED_NATIVE_GENERATED_DIR}/route_1_metadata.hpp"
    COMMAND "${CMAKE_COMMAND}"
            -DOBJECTS_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/Route1.asm
            -DHEADER_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/maps/headers/Route1.asm
            -DTEXT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/text/Route1.asm
            -DCOLLISION_FILE=${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm
            -DOUTPUT=${POKERED_NATIVE_GENERATED_DIR}/route_1_metadata.hpp
            -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateRoute1Metadata.cmake"
    DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/data/maps/objects/Route1.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/maps/headers/Route1.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/text/Route1.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/data/tilesets/collision_tile_ids.asm"
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateRoute1Metadata.cmake"
    COMMENT "Generating native header for Route1 metadata"
    VERBATIM
)

add_custom_target(pokered_native_generated
    DEPENDS
        "${POKERED_NATIVE_GENERATED_DIR}/reds_house_1f_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/reds_house_2f_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/pewter_speech_house_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/blues_house_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/oaks_lab_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/pallet_town_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/route_1_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/reds_house_blockset.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/house_blockset.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/dojo_blockset.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/overworld_blockset.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/reds_house_1f_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/pewter_speech_house_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/blues_house_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/oaks_lab_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/reds_house_2f_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/pallet_town_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/route_1_metadata.hpp"
)

if(MINGW)
    find_package(SDL2 REQUIRED)
    set(POKERED_NATIVE_SDL_LIBS SDL2::SDL2)
    if(TARGET SDL2::SDL2main)
        list(PREPEND POKERED_NATIVE_SDL_LIBS SDL2::SDL2main)
    endif()
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    set(POKERED_NATIVE_SDL_LIBS PkgConfig::SDL2)
endif()

add_executable(pokered_native
    native/src/main.cpp
    native/src/app/application.cpp
    native/src/ui/bitmap_font.cpp
    native/src/core/game_state.cpp
    native/src/oracle/map_file.cpp
    native/src/oracle/provenance.cpp
    native/src/oracle/symbol_file.cpp
    native/src/world/map_data.cpp
    native/src/save/save_system.cpp
    "${POKERED_NATIVE_GENERATED_DIR}/reds_house_1f_blk.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/reds_house_2f_blk.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/pewter_speech_house_blk.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/blues_house_blk.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/oaks_lab_blk.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/pallet_town_blk.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/route_1_blk.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/reds_house_blockset.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/house_blockset.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/dojo_blockset.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/overworld_blockset.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/reds_house_1f_metadata.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/pewter_speech_house_metadata.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/blues_house_metadata.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/oaks_lab_metadata.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/reds_house_2f_metadata.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/pallet_town_metadata.hpp"
    "${POKERED_NATIVE_GENERATED_DIR}/route_1_metadata.hpp"
)
add_dependencies(pokered_native pokered_native_generated)

target_include_directories(pokered_native
    PRIVATE
        native/include
        "${POKERED_NATIVE_GENERATED_DIR}"
)
target_compile_definitions(pokered_native
    PRIVATE
        POKERED_NATIVE_SOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
)
target_compile_options(pokered_native PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(pokered_native PRIVATE ${POKERED_NATIVE_SDL_LIBS})

if(POKERED_NATIVE_BUILD_TESTS)
    enable_testing()

    add_executable(pokered_native_tests
        native/tests/save_system_tests.cpp
        native/src/core/game_state.cpp
        native/src/oracle/map_file.cpp
        native/src/oracle/provenance.cpp
        native/src/oracle/symbol_file.cpp
        native/src/world/map_data.cpp
        native/src/save/save_system.cpp
        "${POKERED_NATIVE_GENERATED_DIR}/reds_house_1f_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/reds_house_2f_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/pewter_speech_house_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/blues_house_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/oaks_lab_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/pallet_town_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/route_1_blk.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/reds_house_blockset.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/house_blockset.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/dojo_blockset.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/overworld_blockset.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/reds_house_1f_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/pewter_speech_house_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/blues_house_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/oaks_lab_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/reds_house_2f_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/pallet_town_metadata.hpp"
        "${POKERED_NATIVE_GENERATED_DIR}/route_1_metadata.hpp"
    )
    add_dependencies(pokered_native_tests pokered_native_generated)
    target_include_directories(pokered_native_tests
        PRIVATE
            native/include
            "${POKERED_NATIVE_GENERATED_DIR}"
    )
    target_compile_definitions(pokered_native_tests
        PRIVATE
            POKERED_NATIVE_SOURCE_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"
    )
    target_compile_options(pokered_native_tests PRIVATE -Wall -Wextra -Wpedantic)

    add_test(NAME pokered_native_tests COMMAND pokered_native_tests)
    add_test(NAME pokered_native_smoke COMMAND pokered_native --smoke-test)
endif()
