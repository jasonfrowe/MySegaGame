cmake_minimum_required(VERSION 3.22)

# Project definition
project(MySegaGame
    VERSION 1.0.0
    LANGUAGES C ASM
    DESCRIPTION "Mega Super Fighter Challenge - A Sega Genesis Game"
)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Set the toolchain (must be done before project())
# This is typically set via -DCMAKE_TOOLCHAIN_FILE
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/SGDKToolchain.cmake" CACHE FILEPATH "Toolchain file")
endif()

# Include SGDK CMake helper functions
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(SGDK)

# ============================================================================
# Project Configuration
# ============================================================================

# Output ROM name
set(ROM_NAME "out" CACHE STRING "Name of the output ROM file")
set(ROM_OUTPUT "${CMAKE_BINARY_DIR}/${ROM_NAME}.bin")

# ============================================================================
# Source Files
# ============================================================================

# Gather C source files
set(GAME_SOURCES
    src/main.c
    src/background.c
    src/bullets.c
    src/clear_sprites.c
    src/ebullets.c
    src/fighters.c
    src/game_data.c
    src/game_level_screen.c
    src/hud.c
    src/player.c
    src/sbullets.c
    src/shield.c
    src/spaceMines.c
    src/title_screen.c
)

# Boot files (rom_head.c is compiled separately to binary, not included in executable)
set(BOOT_SOURCES
    src/boot/sega.s
)

# ============================================================================
# Resources
# ============================================================================

# Resource definition file
set(RES_FILE "${CMAKE_SOURCE_DIR}/res/resources.res")

# Compile resources using SGDK rescomp
sgdk_compile_resources(
    RES_FILE ${RES_FILE}
    OUTPUT_DIR "${CMAKE_BINARY_DIR}/res"
    OUTPUT_SOURCES RES_SOURCES
    OUTPUT_HEADERS RES_HEADERS
)

# Create a custom target for resources to ensure they're built first
add_custom_target(compile_resources DEPENDS ${RES_SOURCES} ${RES_HEADERS})

# ============================================================================
# Executable Target
# ============================================================================

# Create the ROM binary
add_executable(${PROJECT_NAME} 
    ${GAME_SOURCES}
    ${BOOT_SOURCES}
    ${RES_SOURCES}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/inc
    ${CMAKE_SOURCE_DIR}/res
    ${CMAKE_BINARY_DIR}/res
    ${SGDK_INCLUDE_DIR}
    ${SGDK_RES_INCLUDE_DIR}
)

# Generate rom_head.bin for sega.s
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/out/rom_head.bin
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/out
    COMMAND ${CMAKE_C_COMPILER} -m68000 -I${SGDK_INCLUDE_DIR} -I${SGDK_RES_INCLUDE_DIR} -c -o ${CMAKE_BINARY_DIR}/out/rom_head.o ${CMAKE_SOURCE_DIR}/src/boot/rom_head.c
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_BINARY_DIR}/out/rom_head.o ${CMAKE_BINARY_DIR}/out/rom_head.bin
    DEPENDS ${CMAKE_SOURCE_DIR}/src/boot/rom_head.c
    COMMENT "Generating rom_head.bin"
)

# Add rom_head.bin as a dependency
add_custom_target(rom_head_bin DEPENDS ${CMAKE_BINARY_DIR}/out/rom_head.bin)
add_dependencies(${PROJECT_NAME} rom_head_bin)
add_dependencies(${PROJECT_NAME} compile_resources)

# Set source file properties for sega.s to depend on rom_head.bin
set_source_files_properties(
    ${CMAKE_SOURCE_DIR}/src/boot/sega.s
    PROPERTIES
    OBJECT_DEPENDS ${CMAKE_BINARY_DIR}/out/rom_head.bin
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -m68000
    -Wall
    -Wextra
    -Wno-shift-negative-value
    -fno-builtin
    -fomit-frame-pointer
)

# Compiler flags for different build types
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Release>:-O3 -fno-web -fno-gcse -ffunction-sections -fdata-sections>
    $<$<CONFIG:Debug>:-O0 -ggdb -DDEBUG=1>
)

# Link SGDK library and libgcc (for __divsi3, __modsi3, etc.)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${SGDK_LIBRARY}
    gcc  # GCC runtime library for division/modulo helpers
)

# Linker flags
target_link_options(${PROJECT_NAME} PRIVATE
    -T ${SGDK_LINKER_SCRIPT}
    -nostdlib
    -Wl,-Map=${CMAKE_BINARY_DIR}/${ROM_NAME}.map
    $<$<CONFIG:Release>:-Wl,--gc-sections>
)

# Set output name
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${ROM_NAME}
    SUFFIX ".elf"
)

# ============================================================================
# Custom Commands for ROM Generation
# ============================================================================

# Convert ELF to binary ROM
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${ROM_OUTPUT}
    COMMENT "Creating ROM file: ${ROM_OUTPUT}"
    BYPRODUCTS ${ROM_OUTPUT}
)

# Generate symbol file (optional, useful for debugging)
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_NM} --numeric-sort $<TARGET_FILE:${PROJECT_NAME}> > ${CMAKE_BINARY_DIR}/${ROM_NAME}.sym
    COMMENT "Generating symbol file"
)

# Display ROM size
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "ROM size information:"
)

# ============================================================================
# Install Target (optional)
# ============================================================================

install(FILES ${ROM_OUTPUT}
    DESTINATION .
    RENAME ${ROM_NAME}.bin
)

# ============================================================================
# Custom Targets
# ============================================================================

# Clean target
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    COMMENT "Cleaning all build artifacts"
)

# Run target (if you have an emulator configured)
if(DEFINED ENV{SGDK_EMULATOR})
    add_custom_target(run
        COMMAND $ENV{SGDK_EMULATOR} ${ROM_OUTPUT}
        DEPENDS ${PROJECT_NAME}
        COMMENT "Running ROM in emulator"
    )
endif()

# ============================================================================
# Display Configuration Summary
# ============================================================================

message(STATUS "========================================")
message(STATUS "MySegaGame Configuration")
message(STATUS "========================================")
message(STATUS "Build type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "ROM output:     ${ROM_OUTPUT}")
message(STATUS "SGDK library:   ${SGDK_LIBRARY}")
message(STATUS "SGDK include:   ${SGDK_INCLUDE_DIR}")
message(STATUS "Toolchain:      ${CMAKE_C_COMPILER}")
message(STATUS "========================================")
