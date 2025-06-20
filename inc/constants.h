// constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

// --- Background & Scrolling Constants ---
// #define MAP_HW_WIDTH            64
// #define MAP_HW_HEIGHT           32
// #define BG_FAR_TILE_INDEX       TILE_USER_INDEX

#define BBX                     100
#define BBY                     80
#define PARALLAX_FACTOR_BG_B    1
#define PARALLAX_FACTOR_BG_A    1

#define MAPSIZE                 1024
#define MMAPSIZE                -1024
#define MAPSIZED2               512
#define MMAPSIZED2              -512
#define MAPSIZEM1               1023

// --- Sine/Cosine Table ---
// Calculate the number of unique steps (excluding the wrap-around entry)
// SINCOS_TABLE_STEPS definition will need to be near the actual table definition (e.g., in game_data.c or a math_utils.c)

// --- Sound effects ---
#define SFX_LASER               64  // 0-63 are reserved for music <- not used with XGM2

// --- Spacecraft properties ---
#define SHIP_ROT_SPEED          3

// --- Bullet properties ---
#define NBULLET                 8
#define NBULLET_TIMER_MAX       8

// --- EBullet properties ---
#define NEBULLET                8
#define NEBULLET_TIMER_MAX      8

// --- SBullet properties ---
#define NSBULLET                3  // Only one spread-shot at a time
#define NSBULLET_TIMER_MAX      45

// --- Fighter properties ---
#define NFIGHTER_MAX            30
#define FIGHTER_RATE            128 // Rate at which Fighters regenerate (currently unused but good to keep)

// --- HUD properties ---
#define BAR_WIDTH_TILES 8
#define STRIPS_PER_TILE 8

// --- Debug Text ---
#define DEBUG_TEXT_LEN          16

#endif // CONSTANTS_H